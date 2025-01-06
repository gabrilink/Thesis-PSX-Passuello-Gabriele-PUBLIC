#ifndef GLOBALS_H
#define GLOBALS_H

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

// Lunghezza dell'ordering table (OT).
// - 8192: consente di ordinare un numero elevato di primitivi.
#define OT_LEN 8192

// Lunghezza del buffer dei primitivi.
// - 131072: dimensione in byte per memorizzare i dati di tutti i primitivi.
#define PRIMBUFF_LEN 131072

// Costanti per il tracciamento (track) di oggetti o effetti.
// - TRACK_PULL: indica una distanza minima per "tirare" oggetti o entità.
#define TRACK_PULL 100

// - TRACK_PUSH: indica una distanza massima per "spingere" oggetti o entità.
#define TRACK_PUSH 30000

// Funzioni per la gestione dell'ordering table (OT).

// Svuota l'ordering table del buffer corrente.
// - currbuff: indice del buffer corrente (0 o 1).
void EmptyOT(u_short currbuff);

// Imposta un valore specifico in una posizione dell'ordering table.
// - currbuff: indice del buffer corrente.
// - i: indice nella ordering table.
// - value: valore da impostare.
void SetOTAt(u_short currbuff, u_int i, u_long value);

// Ottiene un puntatore a una posizione specifica dell'ordering table.
// - currbuff: indice del buffer corrente.
// - i: indice nella ordering table.
// Restituisce un puntatore al valore nella posizione specificata.
u_long *GetOTAt(u_short currbuff, u_int i);

// Funzioni per la gestione del buffer dei primitivi.

// Incrementa il puntatore al prossimo primitivo di una dimensione specificata.
// - size: dimensione del primitivo da aggiungere.
void IncrementNextPrim(u_int size);

// Imposta il puntatore al prossimo primitivo su un valore specificato.
// - value: puntatore al nuovo valore.
void SetNextPrim(char *value);

// Reimposta il puntatore al prossimo primitivo all'inizio del buffer corrente.
// - currbuff: indice del buffer corrente.
void ResetNextPrim(u_short currbuff);

// Restituisce il puntatore al prossimo primitivo da utilizzare.
char *GetNextPrim(void);

#endif
