#include "joypad.h"
#include <sys/types.h>

// Stato corrente del joypad, rappresentato come una variabile a 32 bit (u_long).
static u_long padstate;

// Funzione per verificare se un determinato pulsante è premuto.
// - p: maschera dei pulsanti da controllare (es. un valore corrispondente a un pulsante specifico).
// Restituisce un valore diverso da 0 se il pulsante è premuto, 0 altrimenti.
int JoyPadCheck(int p) {
  return padstate & p; // Confronta lo stato corrente del pad con la maschera fornita.
}

// Funzione per inizializzare il joypad.
// Configura il sistema per l'uso del joypad.
void JoyPadInit(void) {
  PadInit(0); // Inizializza il joypad con la porta 0.
}

// Funzione per resettare lo stato del joypad.
// Imposta lo stato del pad a 0 (nessun pulsante premuto).
void JoyPadReset(void) {
  padstate = 0; // Resetta la variabile padstate.
}

// Funzione per aggiornare lo stato corrente del joypad.
// Legge lo stato del joypad dalla porta 0 e lo salva nella variabile padstate.
void JoyPadUpdate(void) {
  u_long pad = PadRead(0); // Legge lo stato corrente del joypad dalla porta 0.
  padstate = pad;          // Aggiorna lo stato del joypad con i dati letti.
}
