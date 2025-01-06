#include "display.h"
#include "globals.h"

// Variabili statiche per la gestione del doppio buffering.
static DoubleBuff screen;           // Struttura che contiene i buffer di display e di disegno.
static u_short currbuff;            // Numero del buffer corrente (0 o 1).

// Funzione per ottenere il buffer corrente.
u_short GetCurrBuff(void) {
  return currbuff; // Restituisce il numero del buffer attualmente in uso.
}

// Funzione per inizializzare lo schermo.
void ScreenInit(void) {
  // Resetta la GPU per prepararla all'uso.
  ResetGraph(0);

  // Configura l'area di display del primo buffer.
  SetDefDispEnv(&screen.disp[0], 0,   0, SCREEN_RES_X, SCREEN_RES_Y);
  SetDefDrawEnv(&screen.draw[0], 0, 240, SCREEN_RES_X, SCREEN_RES_Y);

  // Configura l'area di display del secondo buffer.
  SetDefDispEnv(&screen.disp[1], 0, 240, SCREEN_RES_X, SCREEN_RES_Y);
  SetDefDrawEnv(&screen.draw[1], 0,   0, SCREEN_RES_X, SCREEN_RES_Y);

  // Imposta i buffer di disegno come sfondi.
  screen.draw[0].isbg = 1; // Indica che il primo buffer sarà utilizzato come sfondo.
  screen.draw[1].isbg = 1; // Indica che il secondo buffer sarà utilizzato come sfondo.

  // Imposta il colore di sfondo per entrambi i buffer (viola scuro).
  setRGB0(&screen.draw[0], 63, 0, 127);
  setRGB0(&screen.draw[1], 63, 0, 127);

  // Imposta il buffer iniziale come corrente.
  currbuff = 0;
  PutDispEnv(&screen.disp[currbuff]); // Applica l'ambiente di display per il buffer corrente.
  PutDrawEnv(&screen.draw[currbuff]); // Applica l'ambiente di disegno per il buffer corrente.

  // Inizializza e configura gli offset della geometria per la GTE (Geometry Transformation Engine).
  InitGeom();
  SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y); // Configura l'offset al centro dello schermo.
  SetGeomScreen(SCREEN_Z);                        // Imposta la profondità dello schermo.

  // Abilita il display.
  SetDispMask(1);
}

///////////////////////////////////////////////////////////////////////////////
// Disegna i primitivi del frame corrente nell'ordering table.
///////////////////////////////////////////////////////////////////////////////
void DisplayFrame(void) {
  // Sincronizza e attende il blanking verticale.
  DrawSync(0);
  VSync(0);

  // Imposta i buffer di display e disegno correnti.
  PutDispEnv(&screen.disp[currbuff]);
  PutDrawEnv(&screen.draw[currbuff]);

  // Disegna l'ordering table per il buffer corrente.
  DrawOTag(GetOTAt(currbuff, OT_LEN - 1));

  // Scambia il buffer corrente (swap tra 0 e 1).
  currbuff = !currbuff;

  // Reimposta il puntatore al prossimo primitivo all'inizio del buffer di primitivi.
  ResetNextPrim(currbuff);
}
