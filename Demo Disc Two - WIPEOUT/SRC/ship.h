#ifndef SHIP_H
#define SHIP_H

#include "globals.h"
#include "object.h"
#include "track.h"

// Struttura per rappresentare una nave.
typedef struct Ship {
  Object *object;  // Puntatore all'oggetto grafico associato alla nave.

  VECTOR right, up, forward;  // Vettori per rappresentare le direzioni locali della nave: destra, alto, avanti.

  VECTOR vel;  // Velocità della nave.
  VECTOR acc;  // Accelerazione della nave.

  VECTOR thrust;  // Vettore della spinta applicata.
  VECTOR drag;    // Vettore della resistenza aerodinamica.

  short yaw, pitch, roll;  // Rotazioni della nave sui tre assi (imbardata, beccheggio, rollio).

  short velyaw, velpitch, velroll;  // Velocità angolari della nave sui tre assi.
  short accyaw, accpitch, accroll; // Accelerazioni angolari sui tre assi.

  short mass;  // Massa della nave.
  long speed;  // Velocità scalare della nave.
  long thrustmag;  // Magnitudo della spinta applicata.
  long thrustmax;  // Valore massimo della spinta applicabile.

  Section *section;  // Puntatore alla sezione del tracciato in cui si trova la nave.
} Ship;

// Funzione per inizializzare la nave.
// Imposta le proprietà iniziali come posizione, sezione di partenza e stato iniziale.
void ShipInit(Ship *ship, Track *track, VECTOR *startpos);

// Funzione per aggiornare lo stato della nave ad ogni frame.
// Calcola nuove velocità, accelerazioni e posizione basandosi sulla fisica.
void ShipUpdate(Ship *ship);

// Funzione per disegnare gli assi X, Y, Z della nave nel mondo 3D.
// Utile per il debug o la visualizzazione della direzione della nave.
void DrawXYZAxis(Ship *ship, Camera *camera);

#endif
