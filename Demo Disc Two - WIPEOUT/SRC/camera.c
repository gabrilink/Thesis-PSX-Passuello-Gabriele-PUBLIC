#include "camera.h"

// Calcola il prodotto vettoriale tra due vettori e salva il risultato in 'out'.
void VectorCross(VECTOR *a, VECTOR *b, VECTOR *out) {
  OuterProduct12(a, b, out);
}

// Imposta la matrice "lookat" della camera in base a un punto di vista (eye), un target e un vettore "up".
void LookAt(Camera *camera, VECTOR *eye, VECTOR *target, VECTOR *up) {
  VECTOR xright;   // Vettore che rappresenta la direzione destra della camera.
  VECTOR yup;      // Vettore che rappresenta la direzione "up" corretta.
  VECTOR zforward; // Vettore che rappresenta la direzione in avanti.
  VECTOR x, y, z;  // Vettori normalizzati per le direzioni principali.
  VECTOR pos;      // Posizione inversa del punto di vista (eye).
  VECTOR t;        // Vettore temporaneo per calcoli intermedi.

  // Calcola il vettore "forward" (zforward) sottraendo il target dall'occhio (eye).
  zforward.vx = target->vx - eye->vx;
  zforward.vy = target->vy - eye->vy;
  zforward.vz = target->vz - eye->vz;
  VectorNormal(&zforward, &z); // Normalizza il vettore zforward.

  // Calcola il vettore "right" (xright) come il prodotto vettoriale tra z e "up".
  VectorCross(&z, up, &xright);
  VectorNormal(&xright, &x); // Normalizza il vettore xright.

  // Calcola il vettore "up" corretto (yup) come il prodotto vettoriale tra z e x.
  VectorCross(&z, &x, &yup);
  VectorNormal(&yup, &y); // Normalizza il vettore yup.

  // Popola la matrice "lookat" della camera con i vettori x, y e z.
  camera->lookat.m[0][0] = x.vx; camera->lookat.m[0][1] = x.vy; camera->lookat.m[0][2] = x.vz;
  camera->lookat.m[1][0] = y.vx; camera->lookat.m[1][1] = y.vy; camera->lookat.m[1][2] = y.vz;
  camera->lookat.m[2][0] = z.vx; camera->lookat.m[2][1] = z.vy; camera->lookat.m[2][2] = z.vz;

  // Calcola la posizione inversa del punto di vista (eye).
  pos.vx = -eye->vx;
  pos.vy = -eye->vy;
  pos.vz = -eye->vz;

  // Salva la parte di rotazione della matrice nella matrice di rotazione (rotmat) della camera.
  camera->rotmat.m[0][0] = x.vx; camera->rotmat.m[0][1] = x.vy; camera->rotmat.m[0][2] = x.vz;
  camera->rotmat.m[1][0] = y.vx; camera->rotmat.m[1][1] = y.vy; camera->rotmat.m[1][2] = y.vz;
  camera->rotmat.m[2][0] = z.vx; camera->rotmat.m[2][1] = z.vy; camera->rotmat.m[2][2] = z.vz;

  // Combina la rotazione e la traslazione nella matrice "lookat" della camera.
  ApplyMatrixLV(&camera->lookat, &pos, &t); // Applica la matrice al vettore posizione.
  TransMatrix(&camera->lookat, &t);        // Trasforma la matrice finale.
}
