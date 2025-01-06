#ifndef CAMERA_H
#define CAMERA_H

#include "globals.h"

#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

typedef struct Camera {
    VECTOR position;
    SVECTOR rotation;
    MATRIX lookat;
    GsCOORDINATE2 coord;
    GsCOORDINATE2 coord2;

    int		x,y,z;
    int		pan,til,rol;
	GsRVIEW2 view;
} Camera;

extern Camera camera;


void LookAt(Camera *camera, VECTOR *eye, VECTOR *target, VECTOR *up);

#endif


