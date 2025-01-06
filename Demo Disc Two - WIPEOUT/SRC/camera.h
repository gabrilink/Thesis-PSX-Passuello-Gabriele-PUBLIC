#ifndef CAMERA_H
#define CAMERA_H

#include "globals.h"

// Definizione della struttura Camera:
// - position: posizione della camera nello spazio 3D.
// - rotation: rotazione della camera (usata per orientare la vista).
// - rotmat: matrice di rotazione della camera.
// - lookat: matrice che combina rotazione e traslazione per puntare a un target specifico.
typedef struct Camera {
  VECTOR position;   // Vettore che rappresenta la posizione nello spazio.
  SVECTOR rotation;  // Vettore che rappresenta la rotazione (probabilmente in gradi o radianti).
  MATRIX rotmat;     // Matrice di rotazione.
  MATRIX lookat;     // Matrice "lookat" che combina rotazione e traslazione.
} Camera;

// Dichiarazione della funzione LookAt:
// - Imposta la matrice "lookat" della camera in modo che punti verso un target specifico.
// - Parametri:
//   - camera: puntatore alla struttura Camera da configurare.
//   - eye: punto di vista (posizione della camera nello spazio).
//   - target: punto nello spazio che la camera deve osservare.
//   - up: vettore che rappresenta la direzione "su" per orientare correttamente la vista.
void LookAt(Camera *camera, VECTOR *eye, VECTOR *target, VECTOR *up);

#endif
