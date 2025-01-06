#ifndef TEXTURE_H
#define TEXTURE_H

#include <sys/types.h>  // Inclusione delle definizioni di tipi standard.

// Definizioni dei formati di colore (CLUT - Color Lookup Table).
#define	CLUT4			  0x00  // Formato CLUT a 4 bit.
#define	CLUT8			  0x01  // Formato CLUT a 8 bit.
#define	TRUE15		  0x10  // Formato True Color a 15 bit.

// Alias per i formati CLUT per una maggiore chiarezza.
#define  CLUT_4BIT    0  // Formato CLUT 4 bit.
#define  CLUT_8BIT    1  // Formato CLUT 8 bit.
#define  CLUT_15BIT   2  // Formato True Color a 15 bit.

// Definizione della modalità traslucida per le texture.
#define  TRANSLUCENT  1  // Abilita la traslucenza.

// Macro per calcolare gli indirizzi CLUT e TPAGE.
// CLUT(x, y): calcola l'indirizzo CLUT a partire dalle coordinate (x, y).
#define	 CLUT(x,y)       ((y << 6) | x)

// TPAGE(c, a, x, y): calcola l'indirizzo della pagina texture.
#define	 TPAGE(c,a,x,y)  ((c << 7) | (a << 5) | ((x >> 6) + ((y & 0x100) >> 4)))

// Macro per ottenere gli offset orizzontali e verticali di una texture.
#define TextureHOffset(x) (x & 0xffc0)  // Offset orizzontale.
#define TextureVOffset(y) (y & 0x100)   // Offset verticale.

// Macro per determinare il tipo di CLUT di una texture.
#define ClutType(t) (t->flags & 7)  // Restituisce i primi 3 bit dei flag.

// Costante per il numero massimo di texture gestibili.
#define MAX_TEXTURES 800  // Numero massimo di texture.

// Dimensione di ogni tile (mattonella) in byte.
#define BYTES_PER_TILE 42

// Struttura per rappresentare una texture.
typedef struct Texture {
	short			type;       // Tipo della texture (CLUT4, CLUT8, ecc.).
	short			tpage;      // Indice della pagina texture.
	short			clut;       // Indice del CLUT (Color Lookup Table).
	short			clutX;      // Posizione X del CLUT.
	short			clutY;      // Posizione Y del CLUT.
	short			clutW;      // Larghezza del CLUT.
	short			clutH;      // Altezza del CLUT.
	short			textureX;   // Posizione X della texture.
	short			textureY;   // Posizione Y della texture.
	short			textureW;   // Larghezza della texture.
	short			textureH;   // Altezza della texture.
	short			u0;         // Coordinate UV del primo vertice.
	short			v0;         // Coordinate UV del primo vertice.
	short			u1;         // Coordinate UV del secondo vertice.
	short			v1;         // Coordinate UV del secondo vertice.
	short			u2;         // Coordinate UV del terzo vertice.
	short			v2;         // Coordinate UV del terzo vertice.
	short			u3;         // Coordinate UV del quarto vertice.
	short			v3;         // Coordinate UV del quarto vertice.
} Texture;

// Struttura per gestire le texture TIM con CLUT a 8 bit.
typedef struct TimClut8 {
  long     id;             // ID della texture.
  long     flags;          // Flag associati alla texture.
  long     clutBytes;      // Dimensione in byte del CLUT.
  short    clutX;          // Posizione X del CLUT.
  short    clutY;          // Posizione Y del CLUT.
  short    clutW;          // Larghezza del CLUT.
  short    clutH;          // Altezza del CLUT.
  short    clut[256];      // Dati del CLUT (256 colori per CLUT a 8 bit).
  long     textureBytes;   // Dimensione in byte della texture.
  short    textureX;       // Posizione X della texture.
  short    textureY;       // Posizione Y della texture.
  short    textureW;       // Larghezza della texture.
  short    textureH;       // Altezza della texture.
} TimClut8;

// Struttura per gestire le texture TIM con CLUT a 4 bit.
typedef struct TimClut4 {
  long     id;             // ID della texture.
  long     flags;          // Flag associati alla texture.
  long     clutBytes;      // Dimensione in byte del CLUT.
  short    clutX;          // Posizione X del CLUT.
  short    clutY;          // Posizione Y del CLUT.
  short    clutW;          // Larghezza del CLUT.
  short    clutH;          // Altezza del CLUT.
  short    clut[16];       // Dati del CLUT (16 colori per CLUT a 4 bit).
  long     textureBytes;   // Dimensione in byte della texture.
  short    textureX;       // Posizione X della texture.
  short    textureY;       // Posizione Y della texture.
  short    textureW;       // Larghezza della texture.
  short    textureH;       // Altezza della texture.
} TimClut4;

// Struttura base per rappresentare un TIM (Texture Image).
typedef struct Tim {
  long id;    // ID del TIM.
  long flags; // Flag associati al TIM.
} Tim;

// Struttura per rappresentare un tile (mattonella).
typedef struct Tile {
  u_short tileindex; // Indice del tile.
} Tile;

// Funzione per caricare texture compresse CMP.
// Parametri:
// - filenamecmp: nome del file CMP contenente le texture compresse.
// - filenamettf: file TTF per informazioni aggiuntive (opzionale).
void LoadTextureCMP(char *filenamecmp, char *filenamettf);

// Funzione per caricare una texture nella VRAM.
// Parametri:
// - timptr: puntatore alla texture TIM da caricare.
// Restituisce un puntatore alla texture caricata.
Texture *UploadTextureToVRAM(long timptr);

// Funzione per ottenere una texture dallo store.
// Parametri:
// - i: indice della texture.
// Restituisce un puntatore alla texture corrispondente.
Texture *GetFromTextureStore(u_int i);

// Funzione per ottenere il numero totale di texture caricate.
// Restituisce il conteggio delle texture attualmente presenti nello store.
u_short GetTextureCount(void);

#endif
