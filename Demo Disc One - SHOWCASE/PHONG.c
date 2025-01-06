#include "PHONG.h"

int DITH[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};

MATRIX lmat;
MATRIX cmat;

u_short tpage;

int	PG= 100;		/*Phong/Gouraud border line opz*/
int	TBX= 8;			/* Texture Base address X*/



static get_minmax_tri(v, min, max) short v[3][2];
int min[2], max[2];
{
  min[0] = min3(v[0][0], v[1][0], v[2][0]);
  min[1] = min3(v[0][1], v[1][1], v[2][1]);
  max[0] = max3(v[0][0], v[1][0], v[2][0]);
  max[1] = max3(v[0][1], v[1][1], v[2][1]);
}

static min3(a, b, c) int a, b, c;
{
  int min;
  min = a;
  if (b < min)
    min = b;
  if (c < min)
    min = c;
  return (min);
}

static max3(a, b, c) short a, b, c;
{
  int max;
  max = a;
  if (b > max)
    max = b;
  if (c > max)
    max = c;
  return (max);
}

/*sort 3 vertices by Y-coordinates*/
/*generate border vertex number for left&right border*/
/*from minimum vertex to maximum vertex*/
static sort_vertex(
    v, bd1, bd2) short v[3][2]; /*screen coordinates of 3 vertices of tri.*/
int bd1[3];                     /*left border vertex number(max 3 vertices)*/
int bd2[3];                     /*right border vertex number(max 3 vertices)*/
{
  int i;
  int f0, f1, f2; /*flags of vertex (min=0,max=1,otherwise=2)*/
  int flag;       /*flag= (f0<<4)+(f1<<2)+(f2)*/

  for (i = 0; i < 3; i++) {
    bd1[i] = -1;
    bd2[i] = -1;
  }

  /*if(V0:min->f0=0,V0:max->f0=1,else:f0=2*/
  if ((v[0][1] <= v[1][1]) && (v[0][1] <= v[2][1]))
    f0 = 0;
  else if ((v[0][1] >= v[1][1]) && (v[0][1] >= v[2][1]))
    f0 = 1;
  else
    f0 = 2;

  /*if(V1:min->f1=0,V1:max->f1=1,else:f1=2*/
  if ((v[1][1] <= v[0][1]) && (v[1][1] <= v[2][1]))
    f1 = 0;
  else if ((v[1][1] >= v[0][1]) && (v[1][1] >= v[2][1]))
    f1 = 1;
  else
    f1 = 2;

  /*if(V2:min->f2=0,V2:max->f2=1,else:f2=2*/
  if ((v[2][1] <= v[0][1]) && (v[2][1] <= v[1][1]))
    f2 = 0;
  else if ((v[2][1] >= v[0][1]) && (v[2][1] >= v[1][1]))
    f2 = 1;
  else
    f2 = 2;

  flag = (f0 << 4) + (f1 << 2) + (f2);

  switch (flag) {
  case (0 << 4) + (0 << 2) + (0): /*no draw*/
    break;
  case (0 << 4) + (0 << 2) + (1):
    bd1[0] = 0;
    bd1[1] = 2;
    bd2[0] = 1;
    bd2[1] = 2;
    break;
  case (0 << 4) + (0 << 2) + (2): /*impossible*/
    break;
  case (0 << 4) + (1 << 2) + (0):
    bd1[0] = 2;
    bd1[1] = 1;
    bd2[0] = 0;
    bd2[1] = 1;
    break;
  case (0 << 4) + (1 << 2) + (1):
    bd1[0] = 0;
    bd1[1] = 1;
    bd2[0] = 0;
    bd2[1] = 2;
    break;
  case (0 << 4) + (1 << 2) + (2):
    bd1[0] = 0;
    bd1[1] = 2;
    bd1[2] = 1;
    bd2[0] = 0;
    bd2[1] = 1;
    break;
  case (0 << 4) + (2 << 2) + (0): /*impossible*/
    break;
  case (0 << 4) + (2 << 2) + (1):
    bd1[0] = 0;
    bd1[1] = 2;
    bd2[0] = 0;
    bd2[1] = 1;
    bd2[2] = 2;
    break;
  case (0 << 4) + (2 << 2) + (2): /*impossible*/
    break;
  case (1 << 4) + (0 << 2) + (0):
    bd1[0] = 1;
    bd1[1] = 0;
    bd2[0] = 2;
    bd2[1] = 0;
    break;
  case (1 << 4) + (0 << 2) + (1):
    bd1[0] = 1;
    bd1[1] = 0;
    bd2[0] = 1;
    bd2[1] = 2;
    break;
  case (1 << 4) + (0 << 2) + (2):
    bd1[0] = 1;
    bd1[1] = 0;
    bd2[0] = 1;
    bd2[1] = 2;
    bd2[2] = 0;
    break;
  case (1 << 4) + (1 << 2) + (0):
    bd1[0] = 2;
    bd1[1] = 1;
    bd2[0] = 2;
    bd2[1] = 0;
    break;
  case (1 << 4) + (1 << 2) + (1): /*impossible*/
    break;
  case (1 << 4) + (1 << 2) + (2): /*impossible*/
    break;
  case (1 << 4) + (2 << 2) + (0):
    bd1[0] = 2;
    bd1[1] = 1;
    bd1[2] = 0;
    bd2[0] = 2;
    bd2[1] = 0;
    break;
  case (1 << 4) + (2 << 2) + (1): /*impossible*/
    break;
  case (1 << 4) + (2 << 2) + (2): /*impossible*/
    break;
  case (2 << 4) + (0 << 2) + (0): /*impossible*/
    break;
  case (2 << 4) + (0 << 2) + (1):
    bd1[0] = 1;
    bd1[1] = 0;
    bd1[2] = 2;
    bd2[0] = 1;
    bd2[1] = 2;
    break;
  case (2 << 4) + (0 << 2) + (2): /*impossible*/
    break;
  case (2 << 4) + (1 << 2) + (0):
    bd1[0] = 2;
    bd1[1] = 1;
    bd2[0] = 2;
    bd2[1] = 0;
    bd2[2] = 1;
    break;
  case (2 << 4) + (1 << 2) + (1): /*impossible*/
    break;
  case (2 << 4) + (1 << 2) + (2): /*impossible*/
    break;
  case (2 << 4) + (2 << 2) + (0): /*impossible*/
    break;
  case (2 << 4) + (2 << 2) + (1): /*impossible*/
    break;
  case (2 << 4) + (2 << 2) + (2): /*impossible*/
    break;
  default:
    break;
  }
}

/* return strict slant by (1,19,12)format*/
static get_fslant(v1, v2) short v1[2], v2[2];
{
  if (v1[1] == v2[1])
    return (0x7fffffff);
  else
    return ((long)(4096 * (v2[0] - v1[0])) / (v2[1] - v1[1]));
}

/* get strict slant of four edge*/
static get_fslant_edge(v, s) short v[3][2];
int s[3];
{
  s[0] = get_fslant(v[0], v[1]);
  s[1] = get_fslant(v[1], v[2]);
  s[2] = get_fslant(v[2], v[0]);
}

/*Phong Shading Triangle*/
phong_tri(COL, nml, v) CVECTOR *COL; /*original color of triangle*/
SVECTOR nml[3];                      /*normal vectors on each vertex*/
short v[3][2];                       /*screen coordinates of vertex*/
{
  int i, j;
  int min[2], max[2];   /*Left-Upper and Right-down points*/
  int SVn[3], EVn[3];   /*vertex number on
                          start&end vertex on left&right border*/
  int SVf[3], EVf[3];   /*start&end vertex strict slant*/
  int SVa[3], EVa[3];   /*start&end vertex Y coordinates*/
  int fstart_x, fend_x; /*strict start&end X coordinates(1,19,12)*/
  int istart_x, iend_x; /*integer start&end X coordinates*/
  int fslt[3];          /*strict slants(1,19,12) of 3 edges*/
  int mat[2][2];
  int imat[2][2];
  int x[2];
  int p, q;
  VECTOR normal;
  SVECTOR snormal;
  int det;
  RECT rect;
  u_short pix[PIH * PIV]; /*drawing area in Main Memory*/
  VECTOR lef;
  int col[3];
  MATRIX nmat;
  SVECTOR stvec;
  long flag;
  MATRIX lm;
  u_short *pixx; /*drawing area pointer*/
  int idet;
  int fs, ft;
  int i4; /*for Dither*/
  int dith;
  MATRIX WLmat;
  MATRIX lvmat;

  /*normals matrix for interpolation*/
  nmat.m[0][0] = nml[0].vx;
  nmat.m[1][0] = nml[0].vy;
  nmat.m[2][0] = nml[0].vz;
  nmat.m[0][1] = nml[1].vx;
  nmat.m[1][1] = nml[1].vy;
  nmat.m[2][1] = nml[1].vz;
  nmat.m[0][2] = nml[2].vx;
  nmat.m[1][2] = nml[2].vy;
  nmat.m[2][2] = nml[2].vz;
  nmat.t[0] = 0;
  nmat.t[1] = 0;
  nmat.t[2] = 0;

  /*save Rotation Matrix*/
  ReadRotMatrix(&WLmat);

  /*make lighting matrix*/
  MulMatrix0(&lmat, &WLmat, &lvmat);
  MulMatrix0(&cmat, &lvmat, &lm);

  /*Rotation Matrix set for normal vector interpolation*/
  SetRotMatrix(&nmat);
  SetTransMatrix(&nmat);

  /*make lighting matrix include original color*/
  lm.m[0][0] = (COL->r * lm.m[0][0]) >> 12;
  lm.m[0][1] = (COL->r * lm.m[0][1]) >> 12;
  lm.m[0][2] = (COL->r * lm.m[0][2]) >> 12;
  lm.m[1][0] = (COL->g * lm.m[1][0]) >> 12;
  lm.m[1][1] = (COL->g * lm.m[1][1]) >> 12;
  lm.m[1][2] = (COL->g * lm.m[1][2]) >> 12;
  lm.m[2][0] = (COL->b * lm.m[2][0]) >> 12;
  lm.m[2][1] = (COL->b * lm.m[2][1]) >> 12;
  lm.m[2][2] = (COL->b * lm.m[2][2]) >> 12;

  SetLightMatrix(&lm);

  /*make vertex matrix for calculation of interpoaltoin coeff.*/
  mat[0][0] = v[1][0] - v[0][0];
  mat[1][0] = v[1][1] - v[0][1];
  mat[0][1] = v[2][0] - v[0][0];
  mat[1][1] = v[2][1] - v[0][1];

  imat[0][0] = mat[1][1];
  imat[1][0] = -mat[1][0];
  imat[0][1] = -mat[0][1];
  imat[1][1] = mat[0][0];

  det = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];

  idet = 4096 * 4096 / det;

  /*get minmax of tri. LeftUpper=min[2],RightDown=max[2]*/
  get_minmax_tri(v, min, max);

  /*get strict slant of each 2D line*/
  get_fslant_edge(v, fslt);

  /*sort vertex: get vertex number on left&right border*/
  sort_vertex(v, SVn, EVn);

  /*set start&end vertex Y-address and fslant*/
  /*non start|end vertex is don't care	*/
  /*					*/
  /*        SVn[0]=EVn[0]	   ---SVa[0],EVa[0]	*/
  /*	    0					*/
  /*   SVf[0]/ \					*/
  /*        /   \					*/
  /*SVn[1]=2     \ EVf[0]				*/
  /*   SVf[1] \   \	   ---SVa[1]		*/
  /*             \ \				*/
  /*          SV[2]=1=EVn[1] ---SVa[2],EVa[1]	*/
  /*						*/
  /*	SVf[2],EVf[1],EVf[2]: not used		*/
  /*						*/
  for (i = 0; i < 3; i++) {
    SVf[i] = fslt[(SVn[i] + 2) % 3];
    EVf[i] = fslt[EVn[i]];
    SVa[i] = v[SVn[i]][1];
    EVa[i] = v[EVn[i]][1];
  }

  /*set initial strict start_x&end_x*/
  fstart_x = v[SVn[0]][0] << 12;
  fend_x = v[EVn[0]][0] << 12;

  /*read texture area*/
  /*because phong shading destroys previous phong shading*/
  rect.x = min[0];
  rect.y = min[1];
  rect.w = max[0] - min[0] + 1;
  rect.h = max[1] - min[1] + 1;
  if (rect.h % 2 == 1)
    rect.h++;

  DrawSync(0);
  StoreImage(&rect, (u_long *)pix);

  /*pixel pointer initialiaze to drawing area head*/
  pixx = pix;

  /*scan minimum Y to maximum Y*/
  for (i = min[1]; i <= max[1]; i++) {
    /*set integer start X and end X coordinates*/
    istart_x = fstart_x >> 12;
    iend_x = fend_x >> 12;

    /*limitter*/
    if (istart_x < min[0])
      istart_x = min[0];
    if (istart_x > max[0])
      istart_x = max[0];
    if (iend_x < min[0])
      iend_x = min[0];
    if (iend_x > max[0])
      iend_x = max[0];

    /*start vector from v[0]*/
    x[1] = i - v[0][1];
    x[0] = istart_x - v[0][0];

    /*interpolation coeff. of (x[0],x[1])
                    by (v[1]-v[0]) and (v2[1]-v[0])*/
    /* [mat]*[fs]=[x[0]]	*/
    /*       [ft] [x[1]]	*/
    /*			*/
    /* [fs]=[imat]*[x[0]]	*/
    /* [ft]	       [x[1]]	*/
    fs = (imat[0][0] * x[0] + imat[0][1] * x[1]) * idet;
    ft = (imat[1][0] * x[0] + imat[1][1] * x[1]) * idet;

    /*diffrential of fs,ft by x[0])*/
    p = imat[0][0] * idet;
    q = imat[1][0] * idet;

    /*for Dither*/
    i4 = i % 4;

    /*move pixel pointer to start point of line*/
    pixx += istart_x - min[0];

#ifndef EQUIVALENT
    /*phong shade 1 line*/
    PhongLine(istart_x, iend_x, p, q, &pixx, fs, ft, i4, det);

#else
    /*equivalent to PhongLine*/
    for (j = istart_x; j <= iend_x; j++, fs += p, ft += q, pixx++) {
      /*(1.0-fs-ft,fs,ft) is interpolation coeff.
        for interpolation of 3 vertices normal vectors*/
      stvec.vx = (4096 * 4096 - fs - ft) >> 12;
      stvec.vy = fs >> 12;
      stvec.vz = ft >> 12;

      if (det < 100) {
        if (stvec.vx > 4096)
          stvec.vx = 4096;
        if (stvec.vy > 4096)
          stvec.vy = 4096;
        if (stvec.vz > 4096)
          stvec.vz = 4096;
        if (stvec.vx < 0)
          stvec.vx = 0;
        if (stvec.vy < 0)
          stvec.vy = 0;
        if (stvec.vz < 0)
          stvec.vz = 0;
      }

      /*interpolation of normals*/
      RotTrans(&stvec, &normal, &flag);

      /*vector normalize*/
      VectorNormalS(&normal, &snormal);

      /*lighting*/
      LocalLight(&snormal, &lef);

      /*Dithering*/
      dith = DITH[i4][j % 4] / 2 - 4;

      col[0] = (lef.vx + dith) >> 3;
      col[1] = (lef.vy + dith) >> 3;
      col[2] = (lef.vz + dith) >> 3;

      if (col[0] > 0x1f)
        col[0] = 0x1f;
      if (col[0] < 0)
        col[0] = 0;
      if (col[1] > 0x1f)
        col[1] = 0x1f;
      if (col[1] < 0)
        col[1] = 0;
      if (col[2] > 0x1f)
        col[2] = 0x1f;
      if (col[2] <= 0)
        col[2] = 1; /*no transparent*/

      /*pixel draw*/
      *pixx = (col[2] << 10) + (col[1] << 5) + (col[0]);
    }
#endif

    /*move pixel pointer to end of drawing Right end*/
    pixx += max[0] - iend_x;

    /*update strict start X*/
    if (i < SVa[1])
      fstart_x += SVf[0];
    else
      fstart_x += SVf[1];

    /*update strict end X*/
    if (i < EVa[1])
      fend_x += EVf[0];
    else
      fend_x += EVf[1];

    /*limiter*/
    if (fstart_x > fend_x)
      fstart_x = fend_x;
  }
  DrawSync(0);
  LoadImage(&rect, (u_long *)pix);

  /*reset Rotation Matrix*/
  SetRotMatrix(&WLmat);
  SetTransMatrix(&WLmat);
}



int pad_read()
{
	static SVECTOR	ang  = { 0, 0, 0};	/* rotate angle */
	static SVECTOR	angL  = { 0, 0, 0};	/* rotate angle */
	static VECTOR	vec  = {0, 0, 10000};	/* rottranslate vector */
	static VECTOR	vec1  = {-500, 0, 0};/* rottranslate vector */
	static VECTOR	vec2  = {500, 0, 0};/* rottranslate vector */
	static MATRIX	LL1mat;
	static MATRIX	LL2mat;

	int	ret = 0;

/*	u_long	padd = PadRead(0);*/
	u_long	padd;

	padd = PadRead(0);

//	if (padd & PADRup)	    vec.vx -= 32;
//	if (padd & PADRdown)	vec.vy -= 32;
//	if (padd & PADRleft) 	vec.vx += 32;
//	if (padd & PADRright)	vec.vy += 32;



	/* rotate L */
	if (padd & PADLup)	    angL.vz += 32;
	if (padd & PADLdown)	angL.vz -= 32;
	if (padd & PADLleft)	angL.vy += 32;
	if (padd & PADLright)	angL.vy -= 32;

	if (padd & PADk) 	ret = -1;
/*
	if(padd & PADl) PG += 10;
	if(padd & PADn) PG -= 10;
*/

    if (padd & PADselect){
        ang.vx  = 0;ang.vy  = 0;ang.vz  = 0;
        angL.vx = 0;angL.vy = 0;angL.vz = 0;
    }

	/* set matrix */
	RotMatrix(&ang, &LL1mat);
	RotMatrix(&ang, &LL2mat);
	RotMatrix(&angL, &WLmat);

	TransMatrix(&WLmat, &vec);
	TransMatrix(&LL1mat, &vec1);
	TransMatrix(&LL2mat, &vec2);

	CompMatrix(&WLmat,&LL1mat,&WL1mat);
	CompMatrix(&WLmat,&LL2mat,&WL2mat);

	return(ret);
}


/*
 *	initialize primitive parameters
 */
void init_prim(DB *db) {
    long i;

    db->draw.isbg = 1; // Set background
    setRGB0(&db->draw, 60, 120, 180); // Set background color to (60,120,120)

    for (i = 0; i < 6; i++) {
        SetPolyF3(&db->top[i]);
        SetPolyF3(&db->bot[i]);
        SetPolyF3(&db->topP[i]);
        SetPolyF3(&db->botP[i]);
    }
    for (i = 0; i < 8; i++) {
        SetPolyF3(&db->sid[i]);
        SetPolyG3(&db->ege[i]);
        SetPolyF3(&db->sidP[i]);
        SetPolyFT3(&db->egeP[i]);
        SetShadeTex(&db->egeP[i], 1); // No Shading
        db->egeP[i].tpage = 0x0100 + TBX; // 16-bit texture 64*8 offset
        SetPolyG3(&db->egeG[i]); // Alternate Gouraud
    }
}


/*set light vector & light color*/
set_light_color(lmat,cmat)
MATRIX	*lmat,*cmat;
{
	lmat->m[0][0]= 1024; lmat->m[0][1]= 1024; lmat->m[0][2]= 3831;
	lmat->m[1][0]= 0; lmat->m[1][1]= 0; lmat->m[1][2]= 0;
	lmat->m[2][0]= 0; lmat->m[2][1]= 0; lmat->m[2][2]= 0;

	cmat->m[0][0]= 4096; cmat->m[0][1]= 0; cmat->m[0][2]= 0;
	cmat->m[1][0]= 4096; cmat->m[1][1]= 0; cmat->m[1][2]= 0;
	cmat->m[2][0]= 4096; cmat->m[2][1]= 0; cmat->m[2][2]= 0;
}


/*set vertex 3D coordinates*/
set_column_vertex(QCOL,center,hight,width,depth,cut)
SVECTOR	QCOL[8][2];
int	center[3];
int	hight;
int	width;
int	depth;
int	cut;
{
	int	i;

	for(i=0;i<2;i++){
		QCOL[0][i].vx= center[0]-width/2;
		QCOL[0][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[0][i].vz= center[2]-depth/2+cut;

		QCOL[1][i].vx= center[0]-width/2+cut;
		QCOL[1][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[1][i].vz= center[2]-depth/2;

		QCOL[2][i].vx= center[0]+width/2-cut;
		QCOL[2][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[2][i].vz= center[2]-depth/2;

		QCOL[3][i].vx= center[0]+width/2;
		QCOL[3][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[3][i].vz= center[2]-depth/2+cut;

		QCOL[4][i].vx= center[0]+width/2;
		QCOL[4][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[4][i].vz= center[2]+depth/2-cut;

		QCOL[5][i].vx= center[0]+width/2-cut;
		QCOL[5][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[5][i].vz= center[2]+depth/2;

		QCOL[6][i].vx= center[0]-width/2+cut;
		QCOL[6][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[6][i].vz= center[2]+depth/2;

		QCOL[7][i].vx= center[0]-width/2;
		QCOL[7][i].vy= center[1]-hight/2*(1-2*i);
		QCOL[7][i].vz= center[2]+depth/2-cut;
	}
}

/*set normal vector of face*/
set_column_normal_4096(NML_TB,NML_SD)
SVECTOR *NML_TB;
SVECTOR *NML_SD;
{
	/*top&bottom*/
	NML_TB[0].vx= 0; NML_TB[0].vy= 4096; NML_TB[0].vz= 0;
	NML_TB[1].vx= 0; NML_TB[1].vy= -4096; NML_TB[1].vz= 0;

	/*side*/
	NML_SD[0].vx= 0; NML_SD[0].vy= 0; NML_SD[0].vz= 4096;
	NML_SD[1].vx= -4096; NML_SD[1].vy= 0; NML_SD[1].vz= 0;
	NML_SD[2].vx= 0; NML_SD[2].vy= 0; NML_SD[2].vz= -4096;
	NML_SD[3].vx= 4096; NML_SD[3].vy= 0; NML_SD[3].vz= 0;
}

/*rotate&transfer&perspective of 3D coordinate*/
rot_column(QCOL,SV_top,SV_bot,OTZ_top,OTZ_bot)
SVECTOR		QCOL[8][2];		/*3D coordinate*/
short		SV_top[8][2];		/*top 2D coordinate*/
short		SV_bot[8][2];		/*bottom 2D coordinate*/
int		OTZ_top[8];		/*OTZ of vertex*/
int		OTZ_bot[8];		/*OTZ of vertex*/
{
	int	i;
	long	p,flag;

	for(i=0;i<8;i++){
		OTZ_top[i]=RotTransPers(&QCOL[i][0],(long*)SV_top[i],&p,&flag);
		OTZ_bot[i]=RotTransPers(&QCOL[i][1],(long*)SV_bot[i],&p,&flag);
	}
}


/*lighting*/
col_column(NML_TB,NML_SD,COL_TB,COL_SD)
SVECTOR	*NML_TB;
SVECTOR	*NML_SD;
CVECTOR	*COL_TB;
CVECTOR	*COL_SD;
{
        int     i;
	MATRIX	WLmat;
        MATRIX  lm;

	/*rotate local light vector according to rotation of local coordinates*/
	ReadRotMatrix(&WLmat);		/*read local coodinates*/
	MulMatrix0(&lmat,&WLmat,&lm);	/*rotate local light*/
	SetRotMatrix(&WLmat);		/*MulMatrix destroies GTE RotMatrix*/
	SetLightMatrix(&lm);		/*set LightMatrix*/

	/*top&bottom face */
        for(i=0;i<2;i++) NormalColorCol(&NML_TB[i],&object_col,&COL_TB[i]);

	/*side face*/
        for(i=0;i<4;i++) NormalColorCol(&NML_SD[i],&object_col,&COL_SD[i]);
}

/*make GPU packet from vertex 2D coordinates & colors GOURAUD*/
set_prim(SV_top,SV_bot,COL_TB,COL_SD,db,OTZ_top,OTZ_bot)
short		SV_top[8][2];
short		SV_bot[8][2];
CVECTOR		COL_TB[2];
CVECTOR		COL_SD[4];
DB		*db;
int		OTZ_top[8];
int		OTZ_bot[8];
{
	int	i,j;

	/*coordinate: top&bottom vertices' screen coordinates->
	  top&bottom&side&edge triangles' vertices' coordinates */
	/*top*/
	db->top[0].x0= SV_top[0][0];	db->top[0].y0= SV_top[0][1];
	db->top[0].x1= SV_top[7][0];	db->top[0].y1= SV_top[7][1];
	db->top[0].x2= SV_top[6][0];	db->top[0].y2= SV_top[6][1];

	db->top[1].x0= SV_top[0][0];	db->top[1].y0= SV_top[0][1];
	db->top[1].x1= SV_top[6][0];	db->top[1].y1= SV_top[6][1];
	db->top[1].x2= SV_top[1][0];	db->top[1].y2= SV_top[1][1];

	db->top[2].x0= SV_top[1][0];	db->top[2].y0= SV_top[1][1];
	db->top[2].x1= SV_top[6][0];	db->top[2].y1= SV_top[6][1];
	db->top[2].x2= SV_top[5][0];	db->top[2].y2= SV_top[5][1];

	db->top[3].x0= SV_top[1][0];	db->top[3].y0= SV_top[1][1];
	db->top[3].x1= SV_top[5][0];	db->top[3].y1= SV_top[5][1];
	db->top[3].x2= SV_top[2][0];	db->top[3].y2= SV_top[2][1];

	db->top[4].x0= SV_top[2][0];	db->top[4].y0= SV_top[2][1];
	db->top[4].x1= SV_top[5][0];	db->top[4].y1= SV_top[5][1];
	db->top[4].x2= SV_top[4][0];	db->top[4].y2= SV_top[4][1];

	db->top[5].x0= SV_top[2][0];	db->top[5].y0= SV_top[2][1];
	db->top[5].x1= SV_top[4][0];	db->top[5].y1= SV_top[4][1];
	db->top[5].x2= SV_top[3][0];	db->top[5].y2= SV_top[3][1];

	/*bottom*/
	db->bot[0].x0= SV_bot[0][0];	db->bot[0].y0= SV_bot[0][1];
	db->bot[0].x1= SV_bot[6][0];	db->bot[0].y1= SV_bot[6][1];
	db->bot[0].x2= SV_bot[7][0];	db->bot[0].y2= SV_bot[7][1];

	db->bot[1].x0= SV_bot[0][0];	db->bot[1].y0= SV_bot[0][1];
	db->bot[1].x1= SV_bot[1][0];	db->bot[1].y1= SV_bot[1][1];
	db->bot[1].x2= SV_bot[6][0];	db->bot[1].y2= SV_bot[6][1];

	db->bot[2].x0= SV_bot[1][0];	db->bot[2].y0= SV_bot[1][1];
	db->bot[2].x1= SV_bot[5][0];	db->bot[2].y1= SV_bot[5][1];
	db->bot[2].x2= SV_bot[6][0];	db->bot[2].y2= SV_bot[6][1];

	db->bot[3].x0= SV_bot[1][0];	db->bot[3].y0= SV_bot[1][1];
	db->bot[3].x1= SV_bot[2][0];	db->bot[3].y1= SV_bot[2][1];
	db->bot[3].x2= SV_bot[5][0];	db->bot[3].y2= SV_bot[5][1];

	db->bot[4].x0= SV_bot[2][0];	db->bot[4].y0= SV_bot[2][1];
	db->bot[4].x1= SV_bot[4][0];	db->bot[4].y1= SV_bot[4][1];
	db->bot[4].x2= SV_bot[5][0];	db->bot[4].y2= SV_bot[5][1];

	db->bot[5].x0= SV_bot[2][0];	db->bot[5].y0= SV_bot[2][1];
	db->bot[5].x1= SV_bot[3][0];	db->bot[5].y1= SV_bot[3][1];
	db->bot[5].x2= SV_bot[4][0];	db->bot[5].y2= SV_bot[4][1];

	/*side*/
	db->sid[0].x0= SV_top[1][0];	db->sid[0].y0= SV_top[1][1];
	db->sid[0].x1= SV_top[2][0];	db->sid[0].y1= SV_top[2][1];
	db->sid[0].x2= SV_bot[1][0];	db->sid[0].y2= SV_bot[1][1];

	db->sid[1].x0= SV_bot[1][0];	db->sid[1].y0= SV_bot[1][1];
	db->sid[1].x1= SV_top[2][0];	db->sid[1].y1= SV_top[2][1];
	db->sid[1].x2= SV_bot[2][0];	db->sid[1].y2= SV_bot[2][1];

	db->sid[2].x0= SV_top[3][0];	db->sid[2].y0= SV_top[3][1];
	db->sid[2].x1= SV_top[4][0];	db->sid[2].y1= SV_top[4][1];
	db->sid[2].x2= SV_bot[3][0];	db->sid[2].y2= SV_bot[3][1];

	db->sid[3].x0= SV_bot[3][0];	db->sid[3].y0= SV_bot[3][1];
	db->sid[3].x1= SV_top[4][0];	db->sid[3].y1= SV_top[4][1];
	db->sid[3].x2= SV_bot[4][0];	db->sid[3].y2= SV_bot[4][1];

	db->sid[4].x0= SV_top[5][0];	db->sid[4].y0= SV_top[5][1];
	db->sid[4].x1= SV_top[6][0];	db->sid[4].y1= SV_top[6][1];
	db->sid[4].x2= SV_bot[5][0];	db->sid[4].y2= SV_bot[5][1];

	db->sid[5].x0= SV_bot[5][0];	db->sid[5].y0= SV_bot[5][1];
	db->sid[5].x1= SV_top[6][0];	db->sid[5].y1= SV_top[6][1];
	db->sid[5].x2= SV_bot[6][0];	db->sid[5].y2= SV_bot[6][1];

	db->sid[6].x0= SV_top[7][0];	db->sid[6].y0= SV_top[7][1];
	db->sid[6].x1= SV_top[0][0];	db->sid[6].y1= SV_top[0][1];
	db->sid[6].x2= SV_bot[7][0];	db->sid[6].y2= SV_bot[7][1];

	db->sid[7].x0= SV_bot[7][0];	db->sid[7].y0= SV_bot[7][1];
	db->sid[7].x1= SV_top[0][0];	db->sid[7].y1= SV_top[0][1];
	db->sid[7].x2= SV_bot[0][0];	db->sid[7].y2= SV_bot[0][1];

	/*edge*/
	db->ege[0].x0= SV_top[0][0];	db->ege[0].y0= SV_top[0][1];
	db->ege[0].x1= SV_top[1][0];	db->ege[0].y1= SV_top[1][1];
	db->ege[0].x2= SV_bot[0][0];	db->ege[0].y2= SV_bot[0][1];

	db->ege[1].x0= SV_bot[0][0];	db->ege[1].y0= SV_bot[0][1];
	db->ege[1].x1= SV_top[1][0];	db->ege[1].y1= SV_top[1][1];
	db->ege[1].x2= SV_bot[1][0];	db->ege[1].y2= SV_bot[1][1];

	db->ege[2].x0= SV_top[2][0];	db->ege[2].y0= SV_top[2][1];
	db->ege[2].x1= SV_top[3][0];	db->ege[2].y1= SV_top[3][1];
	db->ege[2].x2= SV_bot[2][0];	db->ege[2].y2= SV_bot[2][1];

	db->ege[3].x0= SV_bot[2][0];	db->ege[3].y0= SV_bot[2][1];
	db->ege[3].x1= SV_top[3][0];	db->ege[3].y1= SV_top[3][1];
	db->ege[3].x2= SV_bot[3][0];	db->ege[3].y2= SV_bot[3][1];

	db->ege[4].x0= SV_top[4][0];	db->ege[4].y0= SV_top[4][1];
	db->ege[4].x1= SV_top[5][0];	db->ege[4].y1= SV_top[5][1];
	db->ege[4].x2= SV_bot[4][0];	db->ege[4].y2= SV_bot[4][1];

	db->ege[5].x0= SV_bot[4][0];	db->ege[5].y0= SV_bot[4][1];
	db->ege[5].x1= SV_top[5][0];	db->ege[5].y1= SV_top[5][1];
	db->ege[5].x2= SV_bot[5][0];	db->ege[5].y2= SV_bot[5][1];

	db->ege[6].x0= SV_top[6][0];	db->ege[6].y0= SV_top[6][1];
	db->ege[6].x1= SV_top[7][0];	db->ege[6].y1= SV_top[7][1];
	db->ege[6].x2= SV_bot[6][0];	db->ege[6].y2= SV_bot[6][1];

	db->ege[7].x0= SV_bot[6][0];	db->ege[7].y0= SV_bot[6][1];
	db->ege[7].x1= SV_top[7][0];	db->ege[7].y1= SV_top[7][1];
	db->ege[7].x2= SV_bot[7][0];	db->ege[7].y2= SV_bot[7][1];

	/*color: top&bottom&side faces' color ->
		top&bottom&side faces' color and edge vertices' color*/
	/*top,bottom*/
	for(i=0;i<6;i++){
		db->top[i].r0= COL_TB[0].r;
		db->top[i].g0= COL_TB[0].g;
		db->top[i].b0= COL_TB[0].b;
		db->bot[i].r0= COL_TB[1].r;
		db->bot[i].g0= COL_TB[1].g;
		db->bot[i].b0= COL_TB[1].b;
	}
	/*side*/
	for(i=0;i<4;i++){
		db->sid[2*i].r0= COL_SD[i].r;
		db->sid[2*i].g0= COL_SD[i].g;
		db->sid[2*i].b0= COL_SD[i].b;
		db->sid[2*i+1].r0= COL_SD[i].r;
		db->sid[2*i+1].g0= COL_SD[i].g;
		db->sid[2*i+1].b0= COL_SD[i].b;
	}
	/*edge*/
	db->ege[0].r0= COL_SD[3].r;
	db->ege[0].g0= COL_SD[3].g;
	db->ege[0].b0= COL_SD[3].b;
	db->ege[0].r1= COL_SD[0].r;
	db->ege[0].g1= COL_SD[0].g;
	db->ege[0].b1= COL_SD[0].b;
	db->ege[0].r2= COL_SD[3].r;
	db->ege[0].g2= COL_SD[3].g;
	db->ege[0].b2= COL_SD[3].b;

	db->ege[1].r0= COL_SD[3].r;
	db->ege[1].g0= COL_SD[3].g;
	db->ege[1].b0= COL_SD[3].b;
	db->ege[1].r1= COL_SD[0].r;
	db->ege[1].g1= COL_SD[0].g;
	db->ege[1].b1= COL_SD[0].b;
	db->ege[1].r2= COL_SD[0].r;
	db->ege[1].g2= COL_SD[0].g;
	db->ege[1].b2= COL_SD[0].b;

	db->ege[2].r0= COL_SD[0].r;
	db->ege[2].g0= COL_SD[0].g;
	db->ege[2].b0= COL_SD[0].b;
	db->ege[2].r1= COL_SD[1].r;
	db->ege[2].g1= COL_SD[1].g;
	db->ege[2].b1= COL_SD[1].b;
	db->ege[2].r2= COL_SD[0].r;
	db->ege[2].g2= COL_SD[0].g;
	db->ege[2].b2= COL_SD[0].b;

	db->ege[3].r0= COL_SD[0].r;
	db->ege[3].g0= COL_SD[0].g;
	db->ege[3].b0= COL_SD[0].b;
	db->ege[3].r1= COL_SD[1].r;
	db->ege[3].g1= COL_SD[1].g;
	db->ege[3].b1= COL_SD[1].b;
	db->ege[3].r2= COL_SD[1].r;
	db->ege[3].g2= COL_SD[1].g;
	db->ege[3].b2= COL_SD[1].b;

	db->ege[4].r0= COL_SD[1].r;
	db->ege[4].g0= COL_SD[1].g;
	db->ege[4].b0= COL_SD[1].b;
	db->ege[4].r1= COL_SD[2].r;
	db->ege[4].g1= COL_SD[2].g;
	db->ege[4].b1= COL_SD[2].b;
	db->ege[4].r2= COL_SD[1].r;
	db->ege[4].g2= COL_SD[1].g;
	db->ege[4].b2= COL_SD[1].b;

	db->ege[5].r0= COL_SD[1].r;
	db->ege[5].g0= COL_SD[1].g;
	db->ege[5].b0= COL_SD[1].b;
	db->ege[5].r1= COL_SD[2].r;
	db->ege[5].g1= COL_SD[2].g;
	db->ege[5].b1= COL_SD[2].b;
	db->ege[5].r2= COL_SD[2].r;
	db->ege[5].g2= COL_SD[2].g;
	db->ege[5].b2= COL_SD[2].b;

	db->ege[6].r0= COL_SD[2].r;
	db->ege[6].g0= COL_SD[2].g;
	db->ege[6].b0= COL_SD[2].b;
	db->ege[6].r1= COL_SD[3].r;
	db->ege[6].g1= COL_SD[3].g;
	db->ege[6].b1= COL_SD[3].b;
	db->ege[6].r2= COL_SD[2].r;
	db->ege[6].g2= COL_SD[2].g;
	db->ege[6].b2= COL_SD[2].b;

	db->ege[7].r0= COL_SD[2].r;
	db->ege[7].g0= COL_SD[2].g;
	db->ege[7].b0= COL_SD[2].b;
	db->ege[7].r1= COL_SD[3].r;
	db->ege[7].g1= COL_SD[3].g;
	db->ege[7].b1= COL_SD[3].b;
	db->ege[7].r2= COL_SD[3].r;
	db->ege[7].g2= COL_SD[3].g;
	db->ege[7].b2= COL_SD[3].b;

	/*OTZ: top&bottom vertices' OTZ-> top&bottom&side&edge faces' OTZ*/
	/*top*/
	db->otz_top[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_top[1]= OTZ_top[0]>>(14-OTLEN);
	db->otz_top[2]= OTZ_top[1]>>(14-OTLEN);
	db->otz_top[3]= OTZ_top[1]>>(14-OTLEN);
	db->otz_top[4]= OTZ_top[2]>>(14-OTLEN);
	db->otz_top[5]= OTZ_top[2]>>(14-OTLEN);

	/*bot*/
	db->otz_bot[0]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_bot[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_bot[2]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_bot[3]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_bot[4]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_bot[5]= OTZ_bot[2]>>(14-OTLEN);

	/*side*/
	db->otz_sid[0]= OTZ_top[1]>>(14-OTLEN);
	db->otz_sid[1]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_sid[2]= OTZ_top[3]>>(14-OTLEN);
	db->otz_sid[3]= OTZ_bot[3]>>(14-OTLEN);
	db->otz_sid[4]= OTZ_top[5]>>(14-OTLEN);
	db->otz_sid[5]= OTZ_bot[5]>>(14-OTLEN);
	db->otz_sid[6]= OTZ_top[7]>>(14-OTLEN);
	db->otz_sid[7]= OTZ_bot[7]>>(14-OTLEN);

	/*edge*/
	db->otz_ege[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_ege[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_ege[2]= OTZ_top[2]>>(14-OTLEN);
	db->otz_ege[3]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_ege[4]= OTZ_top[4]>>(14-OTLEN);
	db->otz_ege[5]= OTZ_bot[4]>>(14-OTLEN);
	db->otz_ege[6]= OTZ_top[6]>>(14-OTLEN);
	db->otz_ege[7]= OTZ_bot[6]>>(14-OTLEN);
}

/*add GPU packet to OT*/
add_prim(db)
DB	*db;
{
	int	i,j;
	long	opz;

	for(i=0;i<6;i++){
                opz= NormalClip(*(long*)&db->top[i].x0,
                                *(long*)&db->top[i].x1,
                                *(long*)&db->top[i].x2);

		if(opz>0) AddPrim(db->ot+db->otz_top[i],&db->top[i]);

                opz= NormalClip(*(long*)&db->bot[i].x0,
                                *(long*)&db->bot[i].x1,
                                *(long*)&db->bot[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_bot[i],&db->bot[i]);

	}

	for(i=0;i<8;i++){
                opz= NormalClip(*(long*)&db->sid[i].x0,
                                *(long*)&db->sid[i].x1,
                                *(long*)&db->sid[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_sid[i],&db->sid[i]);

                opz= NormalClip(*(long*)&db->ege[i].x0,
                                *(long*)&db->ege[i].x1,
                                *(long*)&db->ege[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_ege[i],&db->ege[i]);

	}
}

/*make GPU packet from vertex 2D coordinates & colors PHONG*/
set_primP(SV_topP,SV_botP,COL_TB,COL_SD,db,OTZ_top,OTZ_bot)
short		SV_topP[8][2];
short		SV_botP[8][2];
CVECTOR		COL_TB[2];
CVECTOR		COL_SD[4];
DB		*db;
int		OTZ_top[8];
int		OTZ_bot[8];
{
	int	i,j;

	/*coordinate: top&bottom vertices' screen coordinates->
	  top&bottom&side&edge triangles' vertices' coordinates
	  and edge triangles' vertices' texture address*/
	/*top*/
	db->topP[0].x0= SV_topP[0][0];	db->topP[0].y0= SV_topP[0][1];
	db->topP[0].x1= SV_topP[7][0];	db->topP[0].y1= SV_topP[7][1];
	db->topP[0].x2= SV_topP[6][0];	db->topP[0].y2= SV_topP[6][1];

	db->topP[1].x0= SV_topP[0][0];	db->topP[1].y0= SV_topP[0][1];
	db->topP[1].x1= SV_topP[6][0];	db->topP[1].y1= SV_topP[6][1];
	db->topP[1].x2= SV_topP[1][0];	db->topP[1].y2= SV_topP[1][1];

	db->topP[2].x0= SV_topP[1][0];	db->topP[2].y0= SV_topP[1][1];
	db->topP[2].x1= SV_topP[6][0];	db->topP[2].y1= SV_topP[6][1];
	db->topP[2].x2= SV_topP[5][0];	db->topP[2].y2= SV_topP[5][1];

	db->topP[3].x0= SV_topP[1][0];	db->topP[3].y0= SV_topP[1][1];
	db->topP[3].x1= SV_topP[5][0];	db->topP[3].y1= SV_topP[5][1];
	db->topP[3].x2= SV_topP[2][0];	db->topP[3].y2= SV_topP[2][1];

	db->topP[4].x0= SV_topP[2][0];	db->topP[4].y0= SV_topP[2][1];
	db->topP[4].x1= SV_topP[5][0];	db->topP[4].y1= SV_topP[5][1];
	db->topP[4].x2= SV_topP[4][0];	db->topP[4].y2= SV_topP[4][1];

	db->topP[5].x0= SV_topP[2][0];	db->topP[5].y0= SV_topP[2][1];
	db->topP[5].x1= SV_topP[4][0];	db->topP[5].y1= SV_topP[4][1];
	db->topP[5].x2= SV_topP[3][0];	db->topP[5].y2= SV_topP[3][1];

	/*bottom*/
	db->botP[0].x0= SV_botP[0][0];	db->botP[0].y0= SV_botP[0][1];
	db->botP[0].x1= SV_botP[6][0];	db->botP[0].y1= SV_botP[6][1];
	db->botP[0].x2= SV_botP[7][0];	db->botP[0].y2= SV_botP[7][1];

	db->botP[1].x0= SV_botP[0][0];	db->botP[1].y0= SV_botP[0][1];
	db->botP[1].x1= SV_botP[1][0];	db->botP[1].y1= SV_botP[1][1];
	db->botP[1].x2= SV_botP[6][0];	db->botP[1].y2= SV_botP[6][1];

	db->botP[2].x0= SV_botP[1][0];	db->botP[2].y0= SV_botP[1][1];
	db->botP[2].x1= SV_botP[5][0];	db->botP[2].y1= SV_botP[5][1];
	db->botP[2].x2= SV_botP[6][0];	db->botP[2].y2= SV_botP[6][1];

	db->botP[3].x0= SV_botP[1][0];	db->botP[3].y0= SV_botP[1][1];
	db->botP[3].x1= SV_botP[2][0];	db->botP[3].y1= SV_botP[2][1];
	db->botP[3].x2= SV_botP[5][0];	db->botP[3].y2= SV_botP[5][1];

	db->botP[4].x0= SV_botP[2][0];	db->botP[4].y0= SV_botP[2][1];
	db->botP[4].x1= SV_botP[4][0];	db->botP[4].y1= SV_botP[4][1];
	db->botP[4].x2= SV_botP[5][0];	db->botP[4].y2= SV_botP[5][1];

	db->botP[5].x0= SV_botP[2][0];	db->botP[5].y0= SV_botP[2][1];
	db->botP[5].x1= SV_botP[3][0];	db->botP[5].y1= SV_botP[3][1];
	db->botP[5].x2= SV_botP[4][0];	db->botP[5].y2= SV_botP[4][1];

	/*side*/
	db->sidP[0].x0= SV_topP[1][0];	db->sidP[0].y0= SV_topP[1][1];
	db->sidP[0].x1= SV_topP[2][0];	db->sidP[0].y1= SV_topP[2][1];
	db->sidP[0].x2= SV_botP[1][0];	db->sidP[0].y2= SV_botP[1][1];

	db->sidP[1].x0= SV_botP[1][0];	db->sidP[1].y0= SV_botP[1][1];
	db->sidP[1].x1= SV_topP[2][0];	db->sidP[1].y1= SV_topP[2][1];
	db->sidP[1].x2= SV_botP[2][0];	db->sidP[1].y2= SV_botP[2][1];

	db->sidP[2].x0= SV_topP[3][0];	db->sidP[2].y0= SV_topP[3][1];
	db->sidP[2].x1= SV_topP[4][0];	db->sidP[2].y1= SV_topP[4][1];
	db->sidP[2].x2= SV_botP[3][0];	db->sidP[2].y2= SV_botP[3][1];

	db->sidP[3].x0= SV_botP[3][0];	db->sidP[3].y0= SV_botP[3][1];
	db->sidP[3].x1= SV_topP[4][0];	db->sidP[3].y1= SV_topP[4][1];
	db->sidP[3].x2= SV_botP[4][0];	db->sidP[3].y2= SV_botP[4][1];

	db->sidP[4].x0= SV_topP[5][0];	db->sidP[4].y0= SV_topP[5][1];
	db->sidP[4].x1= SV_topP[6][0];	db->sidP[4].y1= SV_topP[6][1];
	db->sidP[4].x2= SV_botP[5][0];	db->sidP[4].y2= SV_botP[5][1];

	db->sidP[5].x0= SV_botP[5][0];	db->sidP[5].y0= SV_botP[5][1];
	db->sidP[5].x1= SV_topP[6][0];	db->sidP[5].y1= SV_topP[6][1];
	db->sidP[5].x2= SV_botP[6][0];	db->sidP[5].y2= SV_botP[6][1];

	db->sidP[6].x0= SV_topP[7][0];	db->sidP[6].y0= SV_topP[7][1];
	db->sidP[6].x1= SV_topP[0][0];	db->sidP[6].y1= SV_topP[0][1];
	db->sidP[6].x2= SV_botP[7][0];	db->sidP[6].y2= SV_botP[7][1];

	db->sidP[7].x0= SV_botP[7][0];	db->sidP[7].y0= SV_botP[7][1];
	db->sidP[7].x1= SV_topP[0][0];	db->sidP[7].y1= SV_topP[0][1];
	db->sidP[7].x2= SV_botP[0][0];	db->sidP[7].y2= SV_botP[0][1];

	/*edge*/
	db->egeP[0].x0= SV_topP[0][0];	db->egeP[0].y0= SV_topP[0][1];
	db->egeP[0].x1= SV_topP[1][0];	db->egeP[0].y1= SV_topP[1][1];
	db->egeP[0].x2= SV_botP[0][0];	db->egeP[0].y2= SV_botP[0][1];

	db->egeP[1].x0= SV_botP[0][0];	db->egeP[1].y0= SV_botP[0][1];
	db->egeP[1].x1= SV_topP[1][0];	db->egeP[1].y1= SV_topP[1][1];
	db->egeP[1].x2= SV_botP[1][0];	db->egeP[1].y2= SV_botP[1][1];

	db->egeP[2].x0= SV_topP[2][0];	db->egeP[2].y0= SV_topP[2][1];
	db->egeP[2].x1= SV_topP[3][0];	db->egeP[2].y1= SV_topP[3][1];
	db->egeP[2].x2= SV_botP[2][0];	db->egeP[2].y2= SV_botP[2][1];

	db->egeP[3].x0= SV_botP[2][0];	db->egeP[3].y0= SV_botP[2][1];
	db->egeP[3].x1= SV_topP[3][0];	db->egeP[3].y1= SV_topP[3][1];
	db->egeP[3].x2= SV_botP[3][0];	db->egeP[3].y2= SV_botP[3][1];

	db->egeP[4].x0= SV_topP[4][0];	db->egeP[4].y0= SV_topP[4][1];
	db->egeP[4].x1= SV_topP[5][0];	db->egeP[4].y1= SV_topP[5][1];
	db->egeP[4].x2= SV_botP[4][0];	db->egeP[4].y2= SV_botP[4][1];

	db->egeP[5].x0= SV_botP[4][0];	db->egeP[5].y0= SV_botP[4][1];
	db->egeP[5].x1= SV_topP[5][0];	db->egeP[5].y1= SV_topP[5][1];
	db->egeP[5].x2= SV_botP[5][0];	db->egeP[5].y2= SV_botP[5][1];

	db->egeP[6].x0= SV_topP[6][0];	db->egeP[6].y0= SV_topP[6][1];
	db->egeP[6].x1= SV_topP[7][0];	db->egeP[6].y1= SV_topP[7][1];
	db->egeP[6].x2= SV_botP[6][0];	db->egeP[6].y2= SV_botP[6][1];

	db->egeP[7].x0= SV_botP[6][0];	db->egeP[7].y0= SV_botP[6][1];
	db->egeP[7].x1= SV_topP[7][0];	db->egeP[7].y1= SV_topP[7][1];
	db->egeP[7].x2= SV_botP[7][0];	db->egeP[7].y2= SV_botP[7][1];

	/*edge GOURAUD*/
	db->egeG[0].x0= SV_topP[0][0];	db->egeG[0].y0= SV_topP[0][1];
	db->egeG[0].x1= SV_topP[1][0];	db->egeG[0].y1= SV_topP[1][1];
	db->egeG[0].x2= SV_botP[0][0];	db->egeG[0].y2= SV_botP[0][1];

	db->egeG[1].x0= SV_botP[0][0];	db->egeG[1].y0= SV_botP[0][1];
	db->egeG[1].x1= SV_topP[1][0];	db->egeG[1].y1= SV_topP[1][1];
	db->egeG[1].x2= SV_botP[1][0];	db->egeG[1].y2= SV_botP[1][1];

	db->egeG[2].x0= SV_topP[2][0];	db->egeG[2].y0= SV_topP[2][1];
	db->egeG[2].x1= SV_topP[3][0];	db->egeG[2].y1= SV_topP[3][1];
	db->egeG[2].x2= SV_botP[2][0];	db->egeG[2].y2= SV_botP[2][1];

	db->egeG[3].x0= SV_botP[2][0];	db->egeG[3].y0= SV_botP[2][1];
	db->egeG[3].x1= SV_topP[3][0];	db->egeG[3].y1= SV_topP[3][1];
	db->egeG[3].x2= SV_botP[3][0];	db->egeG[3].y2= SV_botP[3][1];

	db->egeG[4].x0= SV_topP[4][0];	db->egeG[4].y0= SV_topP[4][1];
	db->egeG[4].x1= SV_topP[5][0];	db->egeG[4].y1= SV_topP[5][1];
	db->egeG[4].x2= SV_botP[4][0];	db->egeG[4].y2= SV_botP[4][1];

	db->egeG[5].x0= SV_botP[4][0];	db->egeG[5].y0= SV_botP[4][1];
	db->egeG[5].x1= SV_topP[5][0];	db->egeG[5].y1= SV_topP[5][1];
	db->egeG[5].x2= SV_botP[5][0];	db->egeG[5].y2= SV_botP[5][1];

	db->egeG[6].x0= SV_topP[6][0];	db->egeG[6].y0= SV_topP[6][1];
	db->egeG[6].x1= SV_topP[7][0];	db->egeG[6].y1= SV_topP[7][1];
	db->egeG[6].x2= SV_botP[6][0];	db->egeG[6].y2= SV_botP[6][1];

	db->egeG[7].x0= SV_botP[6][0];	db->egeG[7].y0= SV_botP[6][1];
	db->egeG[7].x1= SV_topP[7][0];	db->egeG[7].y1= SV_topP[7][1];
	db->egeG[7].x2= SV_botP[7][0];	db->egeG[7].y2= SV_botP[7][1];

	/*edgeUV*/
	db->egeP[0].u0= SV_topP[0][0];	db->egeP[0].v0= SV_topP[0][1];
	db->egeP[0].u1= SV_topP[1][0];	db->egeP[0].v1= SV_topP[1][1];
	db->egeP[0].u2= SV_botP[0][0];	db->egeP[0].v2= SV_botP[0][1];

	db->egeP[1].u0= SV_botP[0][0];	db->egeP[1].v0= SV_botP[0][1];
	db->egeP[1].u1= SV_topP[1][0];	db->egeP[1].v1= SV_topP[1][1];
	db->egeP[1].u2= SV_botP[1][0];	db->egeP[1].v2= SV_botP[1][1];

	db->egeP[2].u0= SV_topP[2][0];	db->egeP[2].v0= SV_topP[2][1];
	db->egeP[2].u1= SV_topP[3][0];	db->egeP[2].v1= SV_topP[3][1];
	db->egeP[2].u2= SV_botP[2][0];	db->egeP[2].v2= SV_botP[2][1];

	db->egeP[3].u0= SV_botP[2][0];	db->egeP[3].v0= SV_botP[2][1];
	db->egeP[3].u1= SV_topP[3][0];	db->egeP[3].v1= SV_topP[3][1];
	db->egeP[3].u2= SV_botP[3][0];	db->egeP[3].v2= SV_botP[3][1];

	db->egeP[4].u0= SV_topP[4][0];	db->egeP[4].v0= SV_topP[4][1];
	db->egeP[4].u1= SV_topP[5][0];	db->egeP[4].v1= SV_topP[5][1];
	db->egeP[4].u2= SV_botP[4][0];	db->egeP[4].v2= SV_botP[4][1];

	db->egeP[5].u0= SV_botP[4][0];	db->egeP[5].v0= SV_botP[4][1];
	db->egeP[5].u1= SV_topP[5][0];	db->egeP[5].v1= SV_topP[5][1];
	db->egeP[5].u2= SV_botP[5][0];	db->egeP[5].v2= SV_botP[5][1];

	db->egeP[6].u0= SV_topP[6][0];	db->egeP[6].v0= SV_topP[6][1];
	db->egeP[6].u1= SV_topP[7][0];	db->egeP[6].v1= SV_topP[7][1];
	db->egeP[6].u2= SV_botP[6][0];	db->egeP[6].v2= SV_botP[6][1];

	db->egeP[7].u0= SV_botP[6][0];	db->egeP[7].v0= SV_botP[6][1];
	db->egeP[7].u1= SV_topP[7][0];	db->egeP[7].v1= SV_topP[7][1];
	db->egeP[7].u2= SV_botP[7][0];	db->egeP[7].v2= SV_botP[7][1];

	/*color: top&bottom&side faces' color ->
		top&bottom&side faces' color and edge vertices' color*/
	/*topP,botPtom*/
	for(i=0;i<6;i++){
		db->topP[i].r0= COL_TB[0].r;
		db->topP[i].g0= COL_TB[0].g;
		db->topP[i].b0= COL_TB[0].b;
		db->botP[i].r0= COL_TB[1].r;
		db->botP[i].g0= COL_TB[1].g;
		db->botP[i].b0= COL_TB[1].b;
	}
	/*sidPe*/
	for(i=0;i<4;i++){
		db->sidP[2*i].r0= COL_SD[i].r;
		db->sidP[2*i].g0= COL_SD[i].g;
		db->sidP[2*i].b0= COL_SD[i].b;
		db->sidP[2*i+1].r0= COL_SD[i].r;
		db->sidP[2*i+1].g0= COL_SD[i].g;
		db->sidP[2*i+1].b0= COL_SD[i].b;
	}
	/*edge*/
	db->egeG[0].r0= COL_SD[3].r;
	db->egeG[0].g0= COL_SD[3].g;
	db->egeG[0].b0= COL_SD[3].b;
	db->egeG[0].r1= COL_SD[0].r;
	db->egeG[0].g1= COL_SD[0].g;
	db->egeG[0].b1= COL_SD[0].b;
	db->egeG[0].r2= COL_SD[3].r;
	db->egeG[0].g2= COL_SD[3].g;
	db->egeG[0].b2= COL_SD[3].b;

	db->egeG[1].r0= COL_SD[3].r;
	db->egeG[1].g0= COL_SD[3].g;
	db->egeG[1].b0= COL_SD[3].b;
	db->egeG[1].r1= COL_SD[0].r;
	db->egeG[1].g1= COL_SD[0].g;
	db->egeG[1].b1= COL_SD[0].b;
	db->egeG[1].r2= COL_SD[0].r;
	db->egeG[1].g2= COL_SD[0].g;
	db->egeG[1].b2= COL_SD[0].b;

	db->egeG[2].r0= COL_SD[0].r;
	db->egeG[2].g0= COL_SD[0].g;
	db->egeG[2].b0= COL_SD[0].b;
	db->egeG[2].r1= COL_SD[1].r;
	db->egeG[2].g1= COL_SD[1].g;
	db->egeG[2].b1= COL_SD[1].b;
	db->egeG[2].r2= COL_SD[0].r;
	db->egeG[2].g2= COL_SD[0].g;
	db->egeG[2].b2= COL_SD[0].b;

	db->egeG[3].r0= COL_SD[0].r;
	db->egeG[3].g0= COL_SD[0].g;
	db->egeG[3].b0= COL_SD[0].b;
	db->egeG[3].r1= COL_SD[1].r;
	db->egeG[3].g1= COL_SD[1].g;
	db->egeG[3].b1= COL_SD[1].b;
	db->egeG[3].r2= COL_SD[1].r;
	db->egeG[3].g2= COL_SD[1].g;
	db->egeG[3].b2= COL_SD[1].b;

	db->egeG[4].r0= COL_SD[1].r;
	db->egeG[4].g0= COL_SD[1].g;
	db->egeG[4].b0= COL_SD[1].b;
	db->egeG[4].r1= COL_SD[2].r;
	db->egeG[4].g1= COL_SD[2].g;
	db->egeG[4].b1= COL_SD[2].b;
	db->egeG[4].r2= COL_SD[1].r;
	db->egeG[4].g2= COL_SD[1].g;
	db->egeG[4].b2= COL_SD[1].b;

	db->egeG[5].r0= COL_SD[1].r;
	db->egeG[5].g0= COL_SD[1].g;
	db->egeG[5].b0= COL_SD[1].b;
	db->egeG[5].r1= COL_SD[2].r;
	db->egeG[5].g1= COL_SD[2].g;
	db->egeG[5].b1= COL_SD[2].b;
	db->egeG[5].r2= COL_SD[2].r;
	db->egeG[5].g2= COL_SD[2].g;
	db->egeG[5].b2= COL_SD[2].b;

	db->egeG[6].r0= COL_SD[2].r;
	db->egeG[6].g0= COL_SD[2].g;
	db->egeG[6].b0= COL_SD[2].b;
	db->egeG[6].r1= COL_SD[3].r;
	db->egeG[6].g1= COL_SD[3].g;
	db->egeG[6].b1= COL_SD[3].b;
	db->egeG[6].r2= COL_SD[2].r;
	db->egeG[6].g2= COL_SD[2].g;
	db->egeG[6].b2= COL_SD[2].b;

	db->egeG[7].r0= COL_SD[2].r;
	db->egeG[7].g0= COL_SD[2].g;
	db->egeG[7].b0= COL_SD[2].b;
	db->egeG[7].r1= COL_SD[3].r;
	db->egeG[7].g1= COL_SD[3].g;
	db->egeG[7].b1= COL_SD[3].b;
	db->egeG[7].r2= COL_SD[3].r;
	db->egeG[7].g2= COL_SD[3].g;
	db->egeG[7].b2= COL_SD[3].b;

	/*OTZ: top&bottom vertices' OTZ-> top&bottom&side&edge faces' OTZ*/
	/*top*/
	db->otz_topP[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_topP[1]= OTZ_top[0]>>(14-OTLEN);
	db->otz_topP[2]= OTZ_top[1]>>(14-OTLEN);
	db->otz_topP[3]= OTZ_top[1]>>(14-OTLEN);
	db->otz_topP[4]= OTZ_top[2]>>(14-OTLEN);
	db->otz_topP[5]= OTZ_top[2]>>(14-OTLEN);

	/*bot*/
	db->otz_botP[0]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_botP[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_botP[2]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_botP[3]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_botP[4]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_botP[5]= OTZ_bot[2]>>(14-OTLEN);

	/*side*/
	db->otz_sidP[0]= OTZ_top[1]>>(14-OTLEN);
	db->otz_sidP[1]= OTZ_bot[1]>>(14-OTLEN);
	db->otz_sidP[2]= OTZ_top[3]>>(14-OTLEN);
	db->otz_sidP[3]= OTZ_bot[3]>>(14-OTLEN);
	db->otz_sidP[4]= OTZ_top[5]>>(14-OTLEN);
	db->otz_sidP[5]= OTZ_bot[5]>>(14-OTLEN);
	db->otz_sidP[6]= OTZ_top[7]>>(14-OTLEN);
	db->otz_sidP[7]= OTZ_bot[7]>>(14-OTLEN);

	/*edge*/
	db->otz_egeP[0]= OTZ_top[0]>>(14-OTLEN);
	db->otz_egeP[1]= OTZ_bot[0]>>(14-OTLEN);
	db->otz_egeP[2]= OTZ_top[2]>>(14-OTLEN);
	db->otz_egeP[3]= OTZ_bot[2]>>(14-OTLEN);
	db->otz_egeP[4]= OTZ_top[4]>>(14-OTLEN);
	db->otz_egeP[5]= OTZ_bot[4]>>(14-OTLEN);
	db->otz_egeP[6]= OTZ_top[6]>>(14-OTLEN);
	db->otz_egeP[7]= OTZ_bot[6]>>(14-OTLEN);
}

/*add GPU packet to OT*/
add_primP(db)
DB	*db;
{
	int	i,j;
	long	opz;

	for(i=0;i<6;i++){
                opz= NormalClip(*(long*)&db->topP[i].x0,
                                *(long*)&db->topP[i].x1,
                                *(long*)&db->topP[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_topP[i],&db->topP[i]);

                opz= NormalClip(*(long*)&db->botP[i].x0,
                                *(long*)&db->botP[i].x1,
                                *(long*)&db->botP[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_botP[i],&db->botP[i]);

	}

	for(i=0;i<8;i++){
                opz= NormalClip(*(long*)&db->sidP[i].x0,
                                *(long*)&db->sidP[i].x1,
                                *(long*)&db->sidP[i].x2);

                if(opz>0) AddPrim(db->ot+db->otz_sidP[i],&db->sidP[i]);

                opz= NormalClip(*(long*)&db->egeP[i].x0,
                                *(long*)&db->egeP[i].x1,
                                *(long*)&db->egeP[i].x2);

                if(opz>PG) AddPrim(db->ot+db->otz_egeP[i],&db->egeP[i]);
		if(opz>0&&opz<=PG) AddPrim(db->ot+db->otz_egeP[i],&db->egeG[i]);
	}
}

/*Phong Shading Column's 4 Edge faces(8 triangles)*/
phong_column_edge(SV_top,SV_bot,NML_TB,NML_SD)
short		SV_top[8][2];		/*top vertices' screen coordinates*/
short		SV_bot[8][2];		/*bottom vertices' screen coordinates*/
SVECTOR		NML_TB[2];		/*top&bottom normals(not used)*/
SVECTOR		NML_SD[4];		/*side normals*/
{
	SVECTOR nml[3];
	short   v[3][2];
	int	opz;


        opz= NormalClip(*(long*)SV_top[0],*(long*)SV_top[1],*(long*)SV_bot[0]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[3].vx;
		nml[0].vy= NML_SD[3].vy;
		nml[0].vz= NML_SD[3].vz;
		nml[1].vx= NML_SD[0].vx;
		nml[1].vy= NML_SD[0].vy;
		nml[1].vz= NML_SD[0].vz;
		nml[2].vx= NML_SD[3].vx;
		nml[2].vy= NML_SD[3].vy;
		nml[2].vz= NML_SD[3].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[0][0]+TBX*64;	v[0][1]= SV_top[0][1];
		v[1][0]= SV_top[1][0]+TBX*64;	v[1][1]= SV_top[1][1];
		v[2][0]= SV_bot[0][0]+TBX*64;	v[2][1]= SV_bot[0][1];

		/*phong shaded image generation*/
	        phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[0],*(long*)SV_top[1],*(long*)SV_bot[1]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[3].vx;
		nml[0].vy= NML_SD[3].vy;
		nml[0].vz= NML_SD[3].vz;
		nml[1].vx= NML_SD[0].vx;
		nml[1].vy= NML_SD[0].vy;
		nml[1].vz= NML_SD[0].vz;
		nml[2].vx= NML_SD[0].vx;
		nml[2].vy= NML_SD[0].vy;
		nml[2].vz= NML_SD[0].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[0][0]+TBX*64;	v[0][1]= SV_bot[0][1];
		v[1][0]= SV_top[1][0]+TBX*64;	v[1][1]= SV_top[1][1];
		v[2][0]= SV_bot[1][0]+TBX*64;	v[2][1]= SV_bot[1][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_top[2],*(long*)SV_top[3],*(long*)SV_bot[2]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[0].vx;
		nml[0].vy= NML_SD[0].vy;
		nml[0].vz= NML_SD[0].vz;
		nml[1].vx= NML_SD[1].vx;
		nml[1].vy= NML_SD[1].vy;
		nml[1].vz= NML_SD[1].vz;
		nml[2].vx= NML_SD[0].vx;
		nml[2].vy= NML_SD[0].vy;
		nml[2].vz= NML_SD[0].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[2][0]+TBX*64;	v[0][1]= SV_top[2][1];
		v[1][0]= SV_top[3][0]+TBX*64;	v[1][1]= SV_top[3][1];
		v[2][0]= SV_bot[2][0]+TBX*64;	v[2][1]= SV_bot[2][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[2],*(long*)SV_top[3],*(long*)SV_bot[3]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[0].vx;
		nml[0].vy= NML_SD[0].vy;
		nml[0].vz= NML_SD[0].vz;
		nml[1].vx= NML_SD[1].vx;
		nml[1].vy= NML_SD[1].vy;
		nml[1].vz= NML_SD[1].vz;
		nml[2].vx= NML_SD[1].vx;
		nml[2].vy= NML_SD[1].vy;
		nml[2].vz= NML_SD[1].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[2][0]+TBX*64;	v[0][1]= SV_bot[2][1];
		v[1][0]= SV_top[3][0]+TBX*64;	v[1][1]= SV_top[3][1];
		v[2][0]= SV_bot[3][0]+TBX*64;	v[2][1]= SV_bot[3][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_top[4],*(long*)SV_top[5],*(long*)SV_bot[4]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[1].vx;
		nml[0].vy= NML_SD[1].vy;
		nml[0].vz= NML_SD[1].vz;
		nml[1].vx= NML_SD[2].vx;
		nml[1].vy= NML_SD[2].vy;
		nml[1].vz= NML_SD[2].vz;
		nml[2].vx= NML_SD[1].vx;
		nml[2].vy= NML_SD[1].vy;
		nml[2].vz= NML_SD[1].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[4][0]+TBX*64;	v[0][1]= SV_top[4][1];
		v[1][0]= SV_top[5][0]+TBX*64;	v[1][1]= SV_top[5][1];
		v[2][0]= SV_bot[4][0]+TBX*64;	v[2][1]= SV_bot[4][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[4],*(long*)SV_top[5],*(long*)SV_bot[5]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[1].vx;
		nml[0].vy= NML_SD[1].vy;
		nml[0].vz= NML_SD[1].vz;
		nml[1].vx= NML_SD[2].vx;
		nml[1].vy= NML_SD[2].vy;
		nml[1].vz= NML_SD[2].vz;
		nml[2].vx= NML_SD[2].vx;
		nml[2].vy= NML_SD[2].vy;
		nml[2].vz= NML_SD[2].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[4][0]+TBX*64;	v[0][1]= SV_bot[4][1];
		v[1][0]= SV_top[5][0]+TBX*64;	v[1][1]= SV_top[5][1];
		v[2][0]= SV_bot[5][0]+TBX*64;	v[2][1]= SV_bot[5][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_top[6],*(long*)SV_top[7],*(long*)SV_bot[6]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[2].vx;
		nml[0].vy= NML_SD[2].vy;
		nml[0].vz= NML_SD[2].vz;
		nml[1].vx= NML_SD[3].vx;
		nml[1].vy= NML_SD[3].vy;
		nml[1].vz= NML_SD[3].vz;
		nml[2].vx= NML_SD[2].vx;
		nml[2].vy= NML_SD[2].vy;
		nml[2].vz= NML_SD[2].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_top[6][0]+TBX*64;	v[0][1]= SV_top[6][1];
		v[1][0]= SV_top[7][0]+TBX*64;	v[1][1]= SV_top[7][1];
		v[2][0]= SV_bot[6][0]+TBX*64;	v[2][1]= SV_bot[6][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}

        opz= NormalClip(*(long*)SV_bot[6],*(long*)SV_top[7],*(long*)SV_bot[7]);

	if(opz>0){
		/*set normals*/
		nml[0].vx= NML_SD[2].vx;
		nml[0].vy= NML_SD[2].vy;
		nml[0].vz= NML_SD[2].vz;
		nml[1].vx= NML_SD[3].vx;
		nml[1].vy= NML_SD[3].vy;
		nml[1].vz= NML_SD[3].vz;
		nml[2].vx= NML_SD[3].vx;
		nml[2].vy= NML_SD[3].vy;
		nml[2].vz= NML_SD[3].vz;

		/*texture area for phong shaded image generation*/
		v[0][0]= SV_bot[6][0]+TBX*64;	v[0][1]= SV_bot[6][1];
		v[1][0]= SV_top[7][0]+TBX*64;	v[1][1]= SV_top[7][1];
		v[2][0]= SV_bot[7][0]+TBX*64;	v[2][1]= SV_bot[7][1];

		/*phong shaded image generation*/
		phong_tri(&object_col,nml,v);
	}
}
