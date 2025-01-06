/* $PSLibId: Run-time Library Release 4.3$ */
/*
 *   Phong Shading program
 *
 *   This program includes the function phong_tri, which performs Phong
 *   shading on one triangle. "Triangle color", "normal vectors of vertices",
*    and "screen coordinates of vertices" are provided to phong_tri, which
 *   then performs Phong shading and renders the triangle.
 *   Since phong_tri renders directly to video memory, Z-sorting with other
 *   polygons requires rendering and re-texturing in a non-display area.
 *
 *        1995,3,29 by Oka
 *        Copyright  (C)  1995 by Sony Computer Entertainment
 *             All rights Reserved */

#ifndef PHONG_H
#define PHONG_H

#define PIH	320
#define PIV	240

#define	DITHER	1

#define SCR_Z	(1024)			/* screen depth (h) */
#define OTLEN	10
#define	OTSIZE	(1<<OTLEN)		/* ordering table size */
#define CUBESIZ	200			/* cube size */


#define PIH             320
#define PIV             240

#define OFX             (PIH/2)                 /* screen offset X */
#define OFY             (PIV/2)                 /* screen offset Y */
#define BGR             60                      /* BG color R */
#define BGG             120                     /* BG color G */
#define BGB             120                     /* BG color B */
#define RBK             0                       /* Back color R */
#define GBK             0                       /* Back color G */
#define BBK             0                       /* Back color B */
#define RFC             BGR                     /* Far color R */
#define GFC             BGG                     /* Far color G */
#define BFC             BGB                     /* Far color B */
#define FOGNEAR         1000                    /* Fog near point */
#define DPQ             1                       /*1:ON,0:OFF*/

#include	    <sys/types.h>
#include        <libetc.h>
#include        <libgte.h>
#include        <libgpu.h>


static min3();
static max3();

int pad_read();
void init_prim();


typedef struct {
	DRAWENV		draw;			/* drawing environment */
	DISPENV		disp;			/* display environment */
	u_long		ot[OTSIZE];		/* ordering table */
	/*right object*/
	POLY_F3		top[6];			/* Flat */
	POLY_F3		bot[6];			/* Flat */
	POLY_F3		sid[8];			/* Flat */
	POLY_G3		ege[8];			/* Gouraud */

	/*left object*/
	POLY_F3		topP[6];		/* Flat */
	POLY_F3		botP[6];		/* Flat */
	POLY_F3		sidP[8];		/* Flat */
	POLY_FT3	egeP[8];		/* Flat texture(Phong) */
	POLY_G3		egeG[8];		/* Gouraud */

	/*right object otz save area*/
	int		otz_top[6];		/* Flat */
	int		otz_bot[6];		/* Flat */
	int		otz_sid[8];		/* Flat */
	int		otz_ege[8];		/* Gouraud */

	/*left object otz save area*/
	int		otz_topP[6];		/* Flat */
	int		otz_botP[6];		/* Flat */
	int		otz_sidP[8];		/* Flat */
	int		otz_egeP[8];		/* Flat texture(Phong) */
	int		otz_egeG[8];		/* Gouraud */
} DB;

CVECTOR object_col;		/*object color*/
MATRIX	WLmat;			/*2 objects*/
MATRIX	WL1mat;			/*Phong object*/
MATRIX	WL2mat;			/*Gouraud object*/

MATRIX	lmat;			/*light vector matrix*/
MATRIX	cmat;			/*light color matrix*/



#endif

