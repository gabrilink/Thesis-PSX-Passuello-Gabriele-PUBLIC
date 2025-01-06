#ifndef DISPLAY_H
#define DISPLAY_H

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>

// Definizione delle costanti per il display.

// Modalità video (ad esempio, PAL o NTSC). 0 potrebbe rappresentare NTSC.
#define VIDEO_MODE 0

// Risoluzione dello schermo.
#define SCREEN_RES_X 320          // Larghezza dello schermo in pixel.
#define SCREEN_RES_Y 240          // Altezza dello schermo in pixel.

// Coordinate del centro dello schermo.
#define SCREEN_CENTER_X (SCREEN_RES_X >> 1) // Calcolo rapido della metà della larghezza.
#define SCREEN_CENTER_Y (SCREEN_RES_Y >> 1) // Calcolo rapido della metà dell'altezza.

// Profondità Z dello schermo.
#define SCREEN_Z 160 // Distanza virtuale per la proiezione prospettica.

// Struttura per la gestione del doppio buffering:
// - Contiene due ambienti di disegno (draw) e due ambienti di display (disp).
typedef struct {
  DRAWENV draw[2]; // Array di ambienti di disegno per il doppio buffering.
  DISPENV disp[2]; // Array di ambienti di display per il doppio buffering.
} DoubleBuff;

// Dichiarazione delle funzioni pubbliche.

// Restituisce il buffer corrente (0 o 1).
u_short GetCurrBuff(void);

// Inizializza lo schermo, configurando i buffer e le impostazioni di rendering.
void ScreenInit(void);

// Mostra il frame corrente sul display e gestisce lo scambio dei buffer.
void DisplayFrame(void);

#endif
