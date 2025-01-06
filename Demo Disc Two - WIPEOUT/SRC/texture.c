#include "texture.h"
#include "globals.h"
#include "lzss.h"
#include <stdlib.h>

static Texture *texturestore[MAX_TEXTURES]; // Array statico che contiene i puntatori alle texture caricate.
static u_short texturecount = 0;           // Contatore statico che tiene traccia del numero di texture caricate.

static u_short textx = 320;                // Coordinata X di default per la posizione della texture.
static u_short texty = 0;                  // Coordinata Y di default per la posizione della texture.
static u_short clutx = 320;                // Coordinata X di default per la posizione della CLUT (Color Look-Up Table).
static u_short cluty = 256;                // Coordinata Y di default per la posizione della CLUT (Color Look-Up Table).

// Funzione per ottenere una texture dallo store delle texture.
// Parametro: indice della texture che si desidera ottenere.
// Ritorna: puntatore alla texture corrispondente.
Texture *GetFromTextureStore(u_int i) {
  return texturestore[i];
}

// Funzione per ottenere il numero totale di texture caricate.
// Ritorna: numero di texture attualmente nello store.
u_short GetTextureCount(void) {
  return texturecount;
}


void LoadTextureCMP(char *filenamecmp, char *filenamettf) {
  u_long b, i;                  // Variabili per iterazioni e offset
  u_long length;                // Lunghezza del file
  u_char *bytes;                // Buffer per il file caricato
  u_short numtextures;          // Numero di texture nel file CMP
  u_long totaltimsize;          // Dimensione totale delle texture TIM decompressa
  Texture *texture;             // Puntatore per gestire ogni texture

  static void *timsbaseaddr;    // Indirizzo base per le TIM decompressa
  static long timoffsets[400];  // Offset per ciascuna texture TIM

  // Carica il file CMP dalla memoria
  bytes = (u_char*) FileRead(filenamecmp, &length);

  // Controlla se il file è stato caricato correttamente
  if (bytes == NULL) {
    printf("Error reading %s from the CD.\n", filenamecmp);
    return;
  }

  b = 0;

  // Ottieni il numero totale di texture dal file
  numtextures = GetLongLE(bytes, &b);
  printf("Number of textures: %d.\n", numtextures);

  // Legge e accumula la dimensione di ciascuna texture TIM
  totaltimsize = 0;
  for (i = 0; i < numtextures; i++) {
    u_long timsize;
    timoffsets[i] = totaltimsize; // Salva l'offset corrente per la texture TIM
    timsize = GetLongLE(bytes, &b);
    totaltimsize += timsize;
  }
  printf("Total size required for all TIMs is = %d\n", totaltimsize);

  // Alloca memoria sufficiente per contenere tutte le TIM decompressa
  timsbaseaddr = (char*) malloc(totaltimsize);

  // Calcola gli offset assoluti aggiungendo l'indirizzo base
  for (i = 0; i < numtextures; i++) {
    timoffsets[i] += (long)timsbaseaddr;
  }

  // Decomprimi l'intero file CMP usando l'algoritmo LZSS
  ExpandLZSSData(&bytes[b], timsbaseaddr);

  // Libera il buffer del file compresso
  free(bytes);

  // Carica tutte le texture in VRAM se non si tratta di texture delle tile di pista
  if (filenamettf == NULL) {
    for (i = 0; i < numtextures; i++) {
      texture = UploadTextureToVRAM(timoffsets[i]); // Carica la texture in VRAM
      if (texture != NULL) {
        texturestore[texturecount++] = texture;    // Salva la texture nello store globale
      }
      if (texturecount > MAX_TEXTURES) {
        printf("Texture global store is full!!!\n");
      }
    }
  }

  // Se si stanno processando texture di tile di pista, gestisce il file TTF
  if (filenamettf != NULL) {
    Tile *tiles;          // Array di tile
    u_short numtiles;     // Numero totale di tile
    bytes = (u_char*) FileRead(filenamettf, &length);
    if (bytes == NULL) {
      printf("Error reading %s from the CD.\n", filenamettf);
      return;
    }
    numtiles = length / BYTES_PER_TILE;         // Calcola il numero totale di tile
    tiles = (Tile*) malloc(numtiles * sizeof(Tile)); // Alloca memoria per i tile

    b = 0;

    // Carica gli indici di tile a bassa risoluzione dal file TTF
    for (i = 0; i < numtiles; i++) {
      b += 16 * 2;                                // Salta gli indici dei tile ad alta risoluzione
      b += 4 * 2;                                 // Salta gli indici dei tile a media risoluzione
      tiles[i].tileindex = GetShortBE(bytes, &b); // Legge l'indice dei tile a bassa risoluzione

      texture = UploadTextureToVRAM(timoffsets[tiles[i].tileindex]); // Carica la texture in VRAM
      if (texture != NULL) {
        texturestore[texturecount++] = texture;   // Salva la texture nello store globale
      }
      if (texturecount > MAX_TEXTURES) {
        printf("Texture global store is full!!!\n");
      }
    }
    free(tiles); // Libera la memoria allocata per i tile
    free(bytes); // Libera il buffer del file TTF
  }

  // Poiché tutte le texture sono state caricate in VRAM, dealloca la memoria temporanea
  free(timsbaseaddr);
}


Texture *UploadTextureToVRAM(long timptr) {
  Tim *tim;  // Puntatore al file TIM
  Texture *texture; // Struttura per salvare i dati della texture
  u_short x, y; // Coordinate temporanee
  RECT rect;    // Rettangolo per definire aree di memoria VRAM
  tim = (Tim*) timptr;     // Converte il puntatore al TIM nella struttura corretta

  // Verifica il tipo di CLUT (Color Look-Up Table)
  switch (ClutType(tim)) {
    case CLUT_4BIT: {   // Texture con CLUT a 4-bit (16 colori)
      TimClut4 *tc4;
      tc4 = (TimClut4*) tim;
      texture = (Texture*) malloc(sizeof(Texture));
      texture->type = CLUT4;

    // Se la texture non ha coordinate definite, le imposta automaticamente
      if (!tc4->textureX && !tc4->textureY) {
        tc4->textureX = textx;
        tc4->textureY = texty;
        tc4->clutX = clutx;
        tc4->clutY = cluty;
        tc4->clutW = 16; // Larghezza CLUT per 4-bit è sempre 16 pixel
        tc4->clutH = 1;  //Altezza CLUT per 4-bit è sempre 1 pixel

        clutx += 16;    // Incrementa la posizione X della CLUT
        if (clutx >= 384) { // Se supera 384 in X, va a capo e scende in Y
          clutx = 320;
          cluty += 1;
        }

        texty += 32;  // Incrementa la posizione Y della texture
        if (texty >= 256) {  // Se supera 256 in Y, va a capo e si sposta a destra in X
          textx += 8;
          texty = 0;
        }
      }
      // Copia le informazioni di dimensioni e coordinate della texture
      texture->textureX = tc4->textureX;
      texture->textureY = tc4->textureY;
      texture->textureW = tc4->textureW;
      texture->textureH = tc4->textureH;
      texture->clutX    = tc4->clutX;
      texture->clutY    = tc4->clutY;
      texture->clutW    = tc4->clutW;
      texture->clutH    = tc4->clutH;
      // Calcola le coordinate di UV per la texture
      x = tc4->textureX - TextureHOffset(tc4->textureX);
      y = tc4->textureY - TextureVOffset(tc4->textureY);

      texture->u0 = (x << 2);
      texture->v0 = (y);
      texture->u1 = ((x + tc4->textureW) << 2) - 1;
      texture->v1 = (y);
      texture->u2 = (x << 2);
      texture->v2 = (y + tc4->textureH) - 1;
      texture->u3 = ((x + tc4->textureW) << 2) - 1;
      texture->v3 = (y + tc4->textureH) - 1;
            // Imposta il TPAGE e il CLUT per la texture

      texture->tpage = TPAGE(CLUT_4BIT, TRANSLUCENT, texture->textureX, texture->textureY);
      texture->clut  = CLUT(texture->clutX >> 4, texture->clutY);

            // Carica la CLUT nella VRAM
      rect.x = tc4->clutX;
      rect.y = tc4->clutY;
      rect.w = tc4->clutW;
      rect.h = tc4->clutH;
      LoadImage(&rect, (u_long*)(&tc4->clut));
      DrawSync(0);

            // Carica la texture nella VRAM
      rect.x = tc4->textureX;
      rect.y = tc4->textureY;
      rect.w = tc4->textureW;
      rect.h = tc4->textureH;
      LoadImage(&rect, (u_long*)(tc4 + 1));
      DrawSync(0);

      break;
    }
    case CLUT_8BIT: { // Texture con CLUT a 8-bit (256 colori)
      TimClut8 *tc8;
      tc8 = (TimClut8*) tim;
      texture = (Texture*) malloc(sizeof(Texture));
      texture->type = CLUT8;
      texture->textureX = tc8->textureX;
      texture->textureY = tc8->textureY;
      texture->textureW = tc8->textureW;
      texture->textureH = tc8->textureH;
      texture->clutX    = tc8->clutX;
      texture->clutY    = tc8->clutY;
      texture->clutW    = tc8->clutW;
      texture->clutH    = tc8->clutH;
            // Calcola le coordinate di UV per la texture

      x = tc8->textureX - TextureHOffset(tc8->textureX);
      y = tc8->textureY - TextureVOffset(tc8->textureY);

      texture->u0 = (x << 1);
      texture->v0 = (y);
      texture->u1 = ((x + tc8->textureW) << 1) - 1;
      texture->v1 = (y);
      texture->u2 = (x << 1);
      texture->v2 = (y + tc8->textureH) - 1;
      texture->u3 = ((x + tc8->textureW) << 1) - 1;
      texture->v3 = (y + tc8->textureH) - 1;

      texture->tpage = TPAGE(CLUT_8BIT, TRANSLUCENT, texture->textureX, texture->textureY);
      texture->clut  = CLUT(texture->clutX >> 4, texture->clutY);

            // Carica la CLUT nella VRAM
      rect.x = tc8->clutX;
      rect.y = tc8->clutY;
      rect.w = tc8->clutW;
      rect.h = tc8->clutH;
      LoadImage(&rect, (u_long*)(&tc8->clut));
      DrawSync(0);

            // Carica la texture nella VRAM
      rect.x = tc8->textureX;
      rect.y = tc8->textureY;
      rect.w = tc8->textureW;
      rect.h = tc8->textureH;
      LoadImage(&rect, (u_long*)(tc8 + 1));
      DrawSync(0);

      break;
    }
  }
  return texture;// Ritorna il puntatore alla texture caricata
}
