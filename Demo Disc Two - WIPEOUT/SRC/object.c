#include "object.h"
#include "globals.h"
#include "inline_n.h"
#include "utils.h"
#include "texture.h"
#include <stdlib.h>
#include <stdio.h>

Object *LoadObjectPRM(char *filename, u_short texturestart) {
  u_char *bytes;
  u_long b;
  u_long i;
  u_long length;
  u_short uoffset, voffset;
  Texture *texture;

  Object *firstobj;
  Object *prevobj;
  Object *object;

  bytes = (u_char*) FileRead(filename, &length);

  if (bytes == NULL) {
    printf("Error reading %s from the CD.\n", filename);
    return NULL;
  }

  b = 0;
  prevobj = NULL;

  while (b < length) {
    object = malloc(sizeof(Object));
    if (prevobj == NULL) {
      firstobj = object;
    } else {
      prevobj->next = object;
    }
    prevobj = object;

    for (i = 0; i < 16; i++) {
      object->name[i] = GetChar(bytes, &b);
    }
    printf("Loading object: %s \n", object->name);

    object->numvertices = GetShortBE(bytes, &b);
    object->vertices = NULL;
    b += 6;

    object->numnormals = GetShortBE(bytes, &b);
    object->normals = NULL;
    b += 6;

    object->numprimitives = GetShortBE(bytes, &b);
    object->primitives = NULL;
    b += 22;

    object->flags = GetShortBE(bytes, &b);
    b += 26;

    object->origin.vx = GetLongBE(bytes, &b);
    object->origin.vy = GetLongBE(bytes, &b);
    object->origin.vz = GetLongBE(bytes, &b);
    b += 48;

    object->vertices = (SVECTOR*) malloc(object->numvertices * sizeof(SVECTOR));
    for (i = 0; i < object->numvertices; i++) {
      object->vertices[i].vx = (GetShortBE(bytes, &b));
      object->vertices[i].vy = (GetShortBE(bytes, &b));
      object->vertices[i].vz = (GetShortBE(bytes, &b));
      b += 2;
    }

    object->normals = (SVECTOR*) malloc(object->numnormals * sizeof(SVECTOR));
    for (i = 0; i < object->numnormals; i++) {
      object->normals[i].vx = GetShortBE(bytes, &b);
      object->normals[i].vy = GetShortBE(bytes, &b);
      object->normals[i].vz = GetShortBE(bytes, &b);
      b += 2;
    }

    object->primitives = (PrimitiveNode*) malloc(object->numprimitives * sizeof(PrimitiveNode));
    for (i = 0; i < object->numprimitives; i++) {
      object->primitives[i].type = GetShortBE(bytes, &b);
      object->primitives[i].flag = GetShortBE(bytes, &b);
      switch (object->primitives[i].type) {
        case TYPE_F3: {
          F3 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(F3));
          prm            = (F3*) object->primitives[i].primitive;
          prm->type      = TYPE_F3;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->pad1      = GetShortBE(bytes, &b);
          prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          break;
        }
        case TYPE_FT3: {
          FT3 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(FT3));
          prm            = (FT3*) object->primitives[i].primitive;
          prm->type      = TYPE_FT3;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->texture   = GetShortBE(bytes, &b);
          prm->clut      = GetShortBE(bytes, &b);
          prm->tpage     = GetShortBE(bytes, &b);
          prm->u0        = GetChar(bytes, &b);
          prm->v0        = GetChar(bytes, &b);
          prm->u1        = GetChar(bytes, &b);
          prm->v1        = GetChar(bytes, &b);
          prm->u2        = GetChar(bytes, &b);
          prm->v2        = GetChar(bytes, &b);
          prm->pad1      = GetShortBE(bytes, &b);
          prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

          prm->texture += texturestart;
          texture = GetFromTextureStore(prm->texture);
          prm->tpage = texture->tpage;
          prm->clut  = texture->clut;
          uoffset = texture->u0;
          voffset = texture->v0;
          prm->u0 += uoffset;
          prm->v0 += voffset;
          prm->u1 += uoffset;
          prm->v1 += voffset;
          prm->u2 += uoffset;
          prm->v2 += voffset;
          break;
        }
        case TYPE_F4: {
          F4 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(F4));
          prm            = (F4*) object->primitives[i].primitive;
          prm->type      = TYPE_F4;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->coords[3] = GetShortBE(bytes, &b);
          prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          break;
        }
        case TYPE_FT4: {
          FT4 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(FT4));
          prm            = (FT4*) object->primitives[i].primitive;
          prm->type      = TYPE_FT4;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->coords[3] = GetShortBE(bytes, &b);
          prm->texture   = GetShortBE(bytes, &b);
          prm->clut      = GetShortBE(bytes, &b);
          prm->tpage     = GetShortBE(bytes, &b);
          prm->u0        = GetChar(bytes, &b);
          prm->v0        = GetChar(bytes, &b);
          prm->u1        = GetChar(bytes, &b);
          prm->v1        = GetChar(bytes, &b);
          prm->u2        = GetChar(bytes, &b);
          prm->v2        = GetChar(bytes, &b);
          prm->u3        = GetChar(bytes, &b);
          prm->v3        = GetChar(bytes, &b);
          prm->pad1      = GetShortBE(bytes, &b);
          prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

          prm->texture += texturestart;
          texture = GetFromTextureStore(prm->texture);
          prm->tpage = texture->tpage;
          prm->clut  = texture->clut;
          uoffset = texture->u0;
          voffset = texture->v0;
          prm->u0 += uoffset;
          prm->v0 += voffset;
          prm->u1 += uoffset;
          prm->v1 += voffset;
          prm->u2 += uoffset;
          prm->v2 += voffset;
          prm->u3 += uoffset;
          prm->v3 += voffset;

          break;
        }
        case TYPE_G3: {
          G3 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(G3));
          prm            = (G3*) object->primitives[i].primitive;
          prm->type      = TYPE_G3;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->pad1      = GetShortBE(bytes, &b);
          prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          break;
        }
        case TYPE_GT3: {
          GT3 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(GT3));
          prm            = (GT3*) object->primitives[i].primitive;
          prm->type      = TYPE_GT3;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->texture   = GetShortBE(bytes, &b);
          prm->clut      = GetShortBE(bytes, &b);
          prm->tpage     = GetShortBE(bytes, &b);
          prm->u0        = GetChar(bytes, &b);
          prm->v0        = GetChar(bytes, &b);
          prm->u1        = GetChar(bytes, &b);
          prm->v1        = GetChar(bytes, &b);
          prm->u2        = GetChar(bytes, &b);
          prm->v2        = GetChar(bytes, &b);
          prm->pad1      = GetShortBE(bytes, &b);
          prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

          prm->texture += texturestart;
          texture = GetFromTextureStore(prm->texture);
          prm->tpage = texture->tpage;
          prm->clut  = texture->clut;
          uoffset = texture->u0;
          voffset = texture->v0;
          prm->u0 += uoffset;
          prm->v0 += voffset;
          prm->u1 += uoffset;
          prm->v1 += voffset;
          prm->u2 += uoffset;
          prm->v2 += voffset;

          break;
        }
        case TYPE_G4: {
          G4 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(G4));
          prm            = (G4*) object->primitives[i].primitive;
          prm->type      = TYPE_G4;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->coords[3] = GetShortBE(bytes, &b);
          prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[3]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          break;
        }
        case TYPE_GT4: {
          GT4 *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(GT4));
          prm            = (GT4*) object->primitives[i].primitive;
          prm->type      = TYPE_GT4;
          prm->coords[0] = GetShortBE(bytes, &b);
          prm->coords[1] = GetShortBE(bytes, &b);
          prm->coords[2] = GetShortBE(bytes, &b);
          prm->coords[3] = GetShortBE(bytes, &b);
          prm->texture   = GetShortBE(bytes, &b);
          prm->clut      = GetShortBE(bytes, &b);
          prm->tpage     = GetShortBE(bytes, &b);
          prm->u0        = GetChar(bytes, &b);
          prm->v0        = GetChar(bytes, &b);
          prm->u1        = GetChar(bytes, &b);
          prm->v1        = GetChar(bytes, &b);
          prm->u2        = GetChar(bytes, &b);
          prm->v2        = GetChar(bytes, &b);
          prm->u3        = GetChar(bytes, &b);
          prm->v3        = GetChar(bytes, &b);
          prm->pad1      = GetShortBE(bytes, &b);
          prm->color[0]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[1]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[2]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          prm->color[3]  = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };

          prm->texture += texturestart;
          texture = GetFromTextureStore(prm->texture);
          prm->tpage = texture->tpage;
          prm->clut  = texture->clut;
          uoffset = texture->u0;
          voffset = texture->v0;
          prm->u0 += uoffset;
          prm->v0 += voffset;
          prm->u1 += uoffset;
          prm->v1 += voffset;
          prm->u2 += uoffset;
          prm->v2 += voffset;
          prm->u3 += uoffset;
          prm->v3 += voffset;

          break;
        }
        case TYPE_TSPR:
        case TYPE_BSPR: {
          SPR *prm;
          object->primitives[i].primitive = (Prm*) malloc(sizeof(SPR));
          prm            = (SPR*) object->primitives[i].primitive;
          prm->type      = TYPE_TSPR;
          prm->coord     = GetShortBE(bytes, &b);
          prm->width     = GetShortBE(bytes, &b);
          prm->height    = GetShortBE(bytes, &b);
          prm->texture   = GetShortBE(bytes, &b);
          prm->color     = (CVECTOR) { GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b), GetChar(bytes, &b) };
          break;
        }
        case TYPE_SPLINE: {
          b += 52; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_POINTLIGHT: {
          b += 24; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_SPOTLIGHT: {
          b += 36; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_INFINITELIGHT: {
          b += 12; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSF3: {
          b += 12; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSFT3: {
          b += 24; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSF4: {
          b += 16; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSFT4: {
          b += 28; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSG3: {
          b += 24; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSGT3: {
          b += 36; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSG4: {
          b += 32; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
        case TYPE_LSGT4: {
          b += 42; // --> skip this amount of bytes to bypass this primitive type
          break;
        }
      }
    }
    // Populate object's initial transform values
    object->position = (VECTOR){object->origin.vx, object->origin.vy, object->origin.vz};
    object->scale    = (VECTOR){ONE, ONE, ONE};

    // Initializing the rotation matrix for each object with the identity matrix (I)
    object->rotmat.m[0][0] = ONE;    object->rotmat.m[0][1] =   0;    object->rotmat.m[0][2] =   0;
    object->rotmat.m[1][0] =   0;    object->rotmat.m[1][1] = ONE;    object->rotmat.m[1][2] =   0;
    object->rotmat.m[2][0] =   0;    object->rotmat.m[2][1] =   0;    object->rotmat.m[2][2] = ONE;
    object->rotmat.t[0] = 0;
    object->rotmat.t[1] = 0;
    object->rotmat.t[2] = 0;
  }
  return firstobj;
}

void RenderObject(Object *object, Camera *camera) {
  int i;
  short nclip;
  long otz, p, flg;

  MATRIX worldmat;
  MATRIX viewmat;

  worldmat = object->rotmat; // Start the world matrix with the rotation matrix that comes from the object
  TransMatrix(&worldmat, &object->position);
  ScaleMatrix(&worldmat, &object->scale);

  CompMatrixLV(&camera->lookat, &worldmat, &viewmat); // combine word and lookat transform

  SetRotMatrix(&viewmat);
  SetTransMatrix(&viewmat);

  // Loop all object primitives
  for (i = 0; i < object->numprimitives; i++) {
    switch (object->primitives[i].type) {
      case TYPE_F3: {
        POLY_F3* poly;
        F3* prm;
        prm = (F3*) object->primitives[i].primitive;
        poly = (POLY_F3*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
        gte_avsz3();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyF3(poly);
          setRGB0(poly, prm->color.r, prm->color.g, prm->color.b);
          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_F3));
        }
        break;
      }
      case TYPE_G3: {
        POLY_G3* poly;
        G3* prm;
        prm = (G3*) object->primitives[i].primitive;
        poly = (POLY_G3*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
        gte_avsz3();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyG3(poly);
          setRGB0(poly, prm->color[0].r, prm->color[0].g, prm->color[0].b);
          setRGB1(poly, prm->color[1].r, prm->color[1].g, prm->color[1].b);
          setRGB2(poly, prm->color[2].r, prm->color[2].g, prm->color[2].b);
          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_G3));
        }
        break;
      }
      case TYPE_FT3: {
        POLY_FT3* poly;
        FT3* prm;
        prm = (FT3*) object->primitives[i].primitive;
        poly = (POLY_FT3*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
        gte_avsz3();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyFT3(poly);
          setRGB0(poly, prm->color.r, prm->color.g, prm->color.b);
          poly->tpage = prm->tpage;
          poly->clut  = prm->clut;
          setUV3(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2);
          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_FT3));
        }
        break;
      }
      case TYPE_GT3: {
        POLY_GT3* poly;
        GT3* prm;
        prm = (GT3*) object->primitives[i].primitive;
        poly = (POLY_GT3*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy3(&poly->x0, &poly->x1, &poly->x2);
        gte_avsz3();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyGT3(poly);
          setRGB0(poly, prm->color[0].r, prm->color[0].g, prm->color[0].b);
          setRGB1(poly, prm->color[1].r, prm->color[1].g, prm->color[1].b);
          setRGB2(poly, prm->color[2].r, prm->color[2].g, prm->color[2].b);
          poly->tpage = prm->tpage;
          poly->clut  = prm->clut;
          setUV3(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2);
          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_GT3));
        }
        break;
      }
      case TYPE_F4: {
        POLY_F4* poly;
        F4* prm;
        prm = (F4*) object->primitives[i].primitive;
        poly = (POLY_F4*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy0(&poly->x0);
        gte_ldv0(&object->vertices[prm->coords[3]]);
        gte_rtps();
        gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
        gte_avsz4();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyF4(poly);
          poly->r0 = prm->color.r;
          poly->g0 = prm->color.g;
          poly->b0 = prm->color.b;
          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_F4));
        }
        break;
      }
      case TYPE_G4: {
        POLY_G4* poly;
        G4* prm;
        prm = (G4*) object->primitives[i].primitive;
        poly = (POLY_G4*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy0(&poly->x0);
        gte_ldv0(&object->vertices[prm->coords[3]]);
        gte_rtps();
        gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
        gte_avsz4();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyG4(poly);
          setRGB0(poly, prm->color[0].r, prm->color[0].g, prm->color[0].b);
          setRGB1(poly, prm->color[1].r, prm->color[1].g, prm->color[1].b);
          setRGB2(poly, prm->color[2].r, prm->color[2].g, prm->color[2].b);
          setRGB3(poly, prm->color[3].r, prm->color[3].g, prm->color[3].b);

          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_G4));
        }
        break;
      }
      case TYPE_FT4: {
        POLY_FT4* poly;
        FT4* prm;
        prm = (FT4*) object->primitives[i].primitive;
        poly = (POLY_FT4*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy0(&poly->x0);
        gte_ldv0(&object->vertices[prm->coords[3]]);
        gte_rtps();
        gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
        gte_avsz4();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyFT4(poly);
          setRGB0(poly, prm->color.r, prm->color.g, prm->color.b);
          poly->tpage = prm->tpage;
          poly->clut  = prm->clut;
          setUV4(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2, prm->u3, prm->v3);

          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_FT4));
        }
        break;
      }
      case TYPE_GT4: {
        POLY_GT4* poly;
        GT4* prm;
        prm = (GT4*) object->primitives[i].primitive;
        poly = (POLY_GT4*) GetNextPrim();
        gte_ldv0(&object->vertices[prm->coords[0]]);
        gte_ldv1(&object->vertices[prm->coords[1]]);
        gte_ldv2(&object->vertices[prm->coords[2]]);
        gte_rtpt();
        gte_nclip();
        gte_stopz(&nclip);
        if (nclip < 0) {
          continue;
        }
        gte_stsxy0(&poly->x0);
        gte_ldv0(&object->vertices[prm->coords[3]]);
        gte_rtps();
        gte_stsxy3(&poly->x1, &poly->x2, &poly->x3);
        gte_avsz4();
        gte_stotz(&otz);
        if (otz > 0 && otz < OT_LEN) {
          SetPolyGT4(poly);
          setRGB0(poly, prm->color[0].r, prm->color[0].g, prm->color[0].b);
          setRGB1(poly, prm->color[1].r, prm->color[1].g, prm->color[1].b);
          setRGB2(poly, prm->color[2].r, prm->color[2].g, prm->color[2].b);
          setRGB3(poly, prm->color[3].r, prm->color[3].g, prm->color[3].b);
          poly->tpage = prm->tpage;
          poly->clut  = prm->clut;
          setUV4(poly, prm->u0, prm->v0, prm->u1, prm->v1, prm->u2, prm->v2, prm->u3, prm->v3);

          addPrim(GetOTAt(GetCurrBuff(), otz), poly);
          IncrementNextPrim(sizeof(POLY_GT4));
        }
        break;
      }
    }
  }
}

Object *GetObjectByIndex(Object *list, u_short index) {
  Object *currobj;
  u_short i;

  i = 0;
  currobj = list;
  while (currobj != NULL) {
    if (i >= index) {
      break;
    }
    currobj = currobj->next;
    i++;
  }
  return currobj;
}

void RenderSceneObjects(Object *list, Camera *camera) {
  Object *currobj;
  VECTOR d;
  u_long distmagsq;

  currobj = list;
  while (currobj != NULL) {
    d.vx = currobj->position.vx - camera->position.vx;
    d.vy = currobj->position.vy - camera->position.vy;
    d.vz = currobj->position.vz - camera->position.vz;

    distmagsq = (d.vx * d.vx) + (d.vy * d.vy) + (d.vz * d.vz);

    if (distmagsq < 700000000) {
      RenderObject(currobj, camera);
    }

    currobj = currobj->next;
  }
}
