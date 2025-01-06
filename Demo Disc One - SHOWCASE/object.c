#include "object.h"
#include "globals.h"
#include "camera.h"
#include "utils.h"

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <libcd.h>


int myActiveBuff= 0;
GsOT		    myOT[2];						    // OT handlers
GsOT_TAG	    myOT_TAG[2][OT_ENTRIES];		    // OT tables
PACKET		myPacketArea[2][PACKETMAX*24];	// Packet buffers
int			myActiveBuff;					// Page index counter


// Object handler
GsDOBJ2	Object[MAX_OBJECTS]={0};
int		ObjectCount=0;

// Lighting coordinates
GsF_LIGHT pslt[1];

///////////////////////////////////////////////////////////////////////////////
// Load texture data from the CD (TIM file)
///////////////////////////////////////////////////////////////////////////////
void LoadTexture_v2(char *filename) {
  u_long i, b, length;
  u_long *bytes;
  TIM_IMAGE tim;

  // Start reading bytes from the CD-ROM
  bytes = (u_long *)FileRead(filename, &length);

  OpenTIM(bytes);
  ReadTIM(&tim);

  // Load texture into VRAM and wait for copy to complete
  LoadImage(tim.prect, tim.paddr);
  DrawSync(0);

  // Load CLUT into VRAM (if present) and wait for copy to complete
  if (tim.mode & 0x8) {
    LoadImage(tim.crect, tim.caddr);
    DrawSync(0);
  }

  // Saving a backup of the rectangle and mode values before we deallocate
  timmode = tim.mode;
  timprect = *tim.prect;
  timcrect = *tim.crect;

  // Deallocate the file buffer
  free(bytes);
}

///////////////////////////////////////////////////////////////////////////////
// Load object data from the CD (vertices, faces, and colors)
///////////////////////////////////////////////////////////////////////////////
void LoadModel(char *filename) {
  u_long i, b, length;
  char *bytes;

  // Start reading bytes from the CD-ROM
  bytes = FileRead(filename, &length);

  b = 0;

  // Read the vertices from the file
  obj.numverts = GetShortBE(bytes, &b);
  obj.vertices = malloc(obj.numverts * sizeof(SVECTOR));
  for (i = 0; i < obj.numverts; i++) {
    obj.vertices[i].vx = GetShortBE(bytes, &b);
    obj.vertices[i].vy = GetShortBE(bytes, &b);
    obj.vertices[i].vz = GetShortBE(bytes, &b);
    printf("VERTICES[%d] X=%d, Y=%d, Z=%d\n", i, obj.vertices[i].vx,
           obj.vertices[i].vy, obj.vertices[i].vz);
  }

  // Read the face indices from the file
  obj.numfaces = GetShortBE(bytes, &b) * 4; // 4 indices per quad
  obj.faces = malloc(obj.numfaces * sizeof(short));
  for (i = 0; i < obj.numfaces; i++) {
    obj.faces[i] = GetShortBE(bytes, &b);
    printf("FACES[%d] %d\n", i, obj.faces[i]);
  }

  // Read the color values from the file
  obj.numcolors = GetChar(bytes, &b);
  obj.colors = malloc(obj.numcolors * sizeof(CVECTOR));
  for (i = 0; i < obj.numcolors; i++) {
    obj.colors[i].r = GetChar(bytes, &b);
    obj.colors[i].g = GetChar(bytes, &b);
    obj.colors[i].b = GetChar(bytes, &b);
    obj.colors[i].cd = GetChar(bytes, &b);
    printf("COLORS[%d] R=%d, G=%d, B=%d\n", i, obj.colors[i].r,
           obj.colors[i].g, obj.colors[i].b);
  }

  // Deallocate the file buffer after reading
  free(bytes);
}

void CalculateCamera() {

	// This function simply calculates the viewpoint matrix based on the camera coordinates...
	// It must be called on every frame before drawing any objects.

	VECTOR	vec;
	GsVIEW2 view;

	// Copy the camera (base) matrix for the viewpoint matrix
	view.view = camera.coord2.coord;
	view.super = WORLD;

	// I really can't explain how this works but I found it in one of the ZIMEN examples
	RotMatrix(&camera.rotation, &view.view);
	ApplyMatrixLV(&view.view, &camera.position, &vec);
	TransMatrix(&view.view, &vec);

	// Set the viewpoint matrix to the GTE
	GsSetView2(&view);

}

void PutObject(VECTOR pos, SVECTOR rotation, GsDOBJ2 *obj) {

	/*	This function draws (or sorts) a TMD model linked to a GsDOBJ2 structure... All
		matrix calculations are done automatically for simplified object placement.

		Parameters:
			pos 	- Object position.
			rot		- Object orientation.
			*obj	- Pointer to a GsDOBJ2 structure that is linked to a TMD model.

	*/

	MATRIX lmtx,omtx;
	GsCOORDINATE2 coord;

	// Copy the camera (base) matrix for the model
	coord = camera.coord2;

	// Rotate and translate the matrix according to the specified coordinates
	RotMatrix(&rotation, &omtx);
	TransMatrix(&omtx, &pos);
	CompMatrixLV(&camera.coord2.coord, &omtx, &coord.coord);
	coord.flg = 0;

	// Apply coordinate matrix to the object
	obj->coord2 = &coord;

	// Calculate Local-World (for lighting) and Local-Screen (for projection) matrices and set both to the GTE
	GsGetLws(obj->coord2, &lmtx, &omtx);
	GsSetLightMatrix(&lmtx);
	GsSetLsMatrix(&omtx);

	// Sort the object!
	GsSortObject4(obj, &myOT[myActiveBuff], 14-OT_LENGTH, getScratchAddr(0));

}

int LinkModel(u_long *tmd, GsDOBJ2 *obj) {

	/*	This function prepares the specified TMD model for drawing and then
		links it to a GsDOBJ2 structure so it can be drawn using GsSortObject4().

		By default, light source calculation is disabled but can be re-enabled by
		simply setting the attribute variable in your GsDOBJ2 structure to 0.

		Parameters:
			*tmd - Pointer to a TMD model file loaded in memory.
			*obj - Pointer to an empty GsDOBJ2 structure.

		Returns:
			Number of objects found inside the TMD file.

	*/

	u_long *dop;
	int i,NumObj;

	// Copy pointer to TMD file so that the original pointer won't get destroyed
	dop = tmd;

	// Skip header and then remap the addresses inside the TMD file
	dop++; GsMapModelingData(dop);

	// Get object count
	dop++; NumObj = *dop;

	// Link object handler with the specified TMD
	dop++;
	for(i=0; i<NumObj; i++) {
		GsLinkObject4((u_long)dop, &obj[i], i);
		obj[i].attribute = (1<<6);	// Disables light source calculation
	}

	// Return the object count found inside the TMD
	return(NumObj);

}

void LoadTexture(u_long *addr) {

	// A simple TIM loader... Not much to explain

	RECT rect;
	GsIMAGE tim;

	// Get TIM information
	GsGetTimInfo((addr+1), &tim);

	// Load the texture image
	rect.x = tim.px;	rect.y = tim.py;
	rect.w = tim.pw;	rect.h = tim.ph;
	LoadImage(&rect, tim.pixel);
	DrawSync(0);

	// Load the CLUT (if present)
	if ((tim.pmode>>3) & 0x01) {
		rect.x = tim.cx;	rect.y = tim.cy;
		rect.w = tim.cw;	rect.h = tim.ch;
		LoadImage(&rect, tim.clut);
		DrawSync(0);
	}

}

void Display() {

	// Flush the font stream
	FntFlush(-1);

	// Wait for VSync, switch buffers, and draw the new frame.
	VSync(0);
	GsSwapDispBuff();
	GsSortClear(70, 20, 255, &myOT[myActiveBuff]);
	GsDrawOt(&myOT[myActiveBuff]);

}

void PrepDisplay() {

	// Get active buffer ID and clear the OT to be processed for the next frame
	myActiveBuff = GsGetActiveBuff();
	GsSetWorkBase((PACKET*)myPacketArea[myActiveBuff]);
	GsClearOt(0, 0, &myOT[myActiveBuff]);

}

void init() {

	SVECTOR VScale={0};

	// Initialize the GS
	ResetGraph(0);
	if (SCREEN_YRES <= 240) {
		GsInitGraph(SCREEN_XRES, SCREEN_YRES, GsOFSGPU|GsNONINTER, DITHER, 0);
		GsDefDispBuff(0, 0, 0, 256);
	} else {
		GsInitGraph(SCREEN_XRES, SCREEN_YRES, GsOFSGPU|GsINTER, DITHER, 0);
		GsDefDispBuff(0, 0, 0, 0);
	}

	// Prepare the ordering tables
	myOT[0].length	=OT_LENGTH;
	myOT[1].length	=OT_LENGTH;
	myOT[0].org		=myOT_TAG[0];
	myOT[1].org		=myOT_TAG[1];

	GsClearOt(0, 0, &myOT[0]);
	GsClearOt(0, 0, &myOT[1]);


	// Initialize debug font stream
	FntLoad(960, 0);
	FntOpen(-CENTERX, -CENTERY, SCREEN_XRES, SCREEN_YRES, 0, 512);


	// Setup 3D and projection matrix
	GsInit3D();
	GsSetProjection(CENTERX);


	// Initialize coordinates for the camera (it will be used as a base for future matrix calculations)
	GsInitCoordinate2(WORLD, &camera.coord2);


	// Set ambient color (for lighting)
	GsSetAmbient(ONE/4, ONE/4, ONE/4);

	// Set default lighting mode
	GsSetLightMode(0);


	// Initialize controller
	PadInit(0);

}


