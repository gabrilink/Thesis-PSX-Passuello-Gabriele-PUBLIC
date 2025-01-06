#ifndef OBJECT_H
#define OBJECT_H

#include "globals.h"
#include "camera.h"
#include "utils.h"

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <libcd.h>

typedef struct Obj {
  SVECTOR rotation;
  VECTOR position;
  VECTOR scale;

  short numverts;
  SVECTOR *vertices;

  short numfaces;
  short *faces;

  short numcolors;
  CVECTOR *colors;
} Obj;

// Maximum number of objects
#define MAX_OBJECTS 3

// Screen resolution and dither mode
#define SCREEN_XRES		640
#define SCREEN_YRES 	480
#define DITHER			1

#define CENTERX			SCREEN_XRES/2
#define CENTERY			SCREEN_YRES/2

// Increasing this value (max is 14) reduces sorting errors in certain cases
#define OT_LENGTH	12

#define OT_ENTRIES	1<<OT_LENGTH
#define PACKETMAX	2048

extern GsOT		    myOT[2];						    // OT handlers
extern GsOT_TAG	    myOT_TAG[2][OT_ENTRIES];		    // OT tables
extern PACKET		myPacketArea[2][PACKETMAX*24];	// Packet buffers
extern int			myActiveBuff;					// Page index counter

extern Obj obj;
extern u_long timmode; /* Pixel mode of the TIM */
extern RECT timprect;  /* Stores the X,Y location of the texture */
extern RECT timcrect;  /* Stores the X,Y location of the CLUT */

// Object handler
extern GsDOBJ2	Object[MAX_OBJECTS];
extern int		ObjectCount;

// Lighting coordinates
extern GsF_LIGHT pslt[1];

// Prototypes
void LoadTexture_v2(char *filename);
void LoadModel(char *filename);

void CalculateCamera();
void PutObject(VECTOR position, SVECTOR rotation, GsDOBJ2 *obj);
int LinkModel(u_long *tmd, GsDOBJ2 *obj);
void LoadTexture(u_long *addr);
void init();
void PrepDisplay();
void Display();


#endif
