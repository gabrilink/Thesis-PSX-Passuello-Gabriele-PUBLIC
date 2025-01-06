#include "globals.h"

// Strutture dati statiche per la gestione dei primitivi e dell'ordering table.

// Ordering table: contiene puntatori ai primitivi ordinati per la loro priorità.
static u_long ot[2][OT_LEN];

// Buffer dei primitivi: memorizza i dati effettivi per ciascun primitivo.
static char primbuff[2][PRIMBUFF_LEN];

// Puntatore al prossimo primitivo da allocare nel buffer dei primitivi.
static char *nextprim;

// Funzione per svuotare l'ordering table del buffer corrente.
// - currbuff: indice del buffer corrente (0 o 1).
void EmptyOT(u_short currbuff) {
  ClearOTagR(ot[currbuff], OT_LEN); // Resetta l'ordering table del buffer specificato.
}

// Funzione per impostare un valore in una posizione specifica dell'ordering table.
// - currbuff: indice del buffer corrente.
// - i: indice nella ordering table.
// - value: valore da assegnare.
void SetOTAt(u_short currbuff, u_int i, u_long value) {
  ot[currbuff][i] = value; // Assegna il valore nella posizione specificata.
}

// Funzione per ottenere un puntatore a una posizione specifica dell'ordering table.
// - currbuff: indice del buffer corrente.
// - i: indice nella ordering table.
// Restituisce un puntatore al valore richiesto.
u_long *GetOTAt(u_short currbuff, u_int i) {
  return &ot[currbuff][i];
}

// Funzione per incrementare il puntatore al prossimo primitivo.
// - size: dimensione del primitivo da aggiungere.
void IncrementNextPrim(u_int size) {
  nextprim += size; // Avanza il puntatore di una quantità pari alla dimensione del primitivo.
}

// Funzione per resettare il puntatore al prossimo primitivo al buffer corrente.
// - currbuff: indice del buffer corrente (0 o 1).
void ResetNextPrim(u_short currbuff) {
  nextprim = primbuff[currbuff]; // Reimposta il puntatore all'inizio del buffer dei primitivi.
}

// Funzione per impostare il puntatore al prossimo primitivo su un valore specifico.
// - value: puntatore al nuovo valore.
void SetNextPrim(char *value) {
  nextprim = value;
}

// Funzione per ottenere il puntatore al prossimo primitivo.
// Restituisce il puntatore `nextprim`.
char *GetNextPrim(void) {
  return nextprim;
}
