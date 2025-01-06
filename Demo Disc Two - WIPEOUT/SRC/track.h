#ifndef TRACK_H
#define TRACK_H

#include "globals.h"  // Include variabili e costanti globali.
#include "camera.h"   // Include definizioni e funzioni relative alla telecamera.

// Definizioni delle costanti che rappresentano vari tipi di facce della pista.
// Questi flag possono essere combinati per definire attributi specifici di una faccia.
#define FACE_TRACK_BASE         1    // Flag per indicare una faccia della pista base.
#define FACE_WEAPON_GRID_LEFT   2    // Flag per indicare una griglia arma a sinistra.
#define FACE_FLIP_TEXTURE       4    // Flag per indicare una faccia con texture ribaltata.
#define FACE_WEAPON_GRID_RIGHT  8    // Flag per indicare una griglia arma a destra.
#define FACE_START_GRID        16    // Flag per indicare la griglia di partenza.
#define FACE_SPEED_UP          32    // Flag per indicare una faccia che accelera il veicolo.

// Costanti per il numero di byte utilizzati nelle strutture della pista.
#define BYTES_PER_VERTEX    16  // Numero di byte per ciascun vertice.
#define BYTES_PER_FACE      20  // Numero di byte per ciascuna faccia.
#define BYTES_PER_SECTION  156  // Numero di byte per ciascuna sezione.

// Definizione della struttura Face per rappresentare una faccia della pista.
typedef struct Face {
  short indices[4];   // Indici dei 4 vertici che compongono la faccia.
  char    flags;      // Flag che definiscono le proprietà della faccia.
  SVECTOR normal;     // Normale della faccia (direzione perpendicolare alla superficie).
  CVECTOR color;      // Colore della faccia.
  char    texture;    // Indice della texture associata alla faccia.
  short   clut;       // Indice CLUT (Color Lookup Table) per la texture.
  short   tpage;      // Indice della pagina texture.
  short   u0, v0;     // Coordinate UV del primo vertice.
  short   u1, v1;     // Coordinate UV del secondo vertice.
  short   u2, v2;     // Coordinate UV del terzo vertice.
  short   u3, v3;     // Coordinate UV del quarto vertice.
} Face;

// Definizione della struttura Section per rappresentare una sezione della pista.
typedef struct Section {
  short id;               // ID univoco della sezione.
  short flags;            // Flag che definiscono le proprietà della sezione.

  struct Section *prev;   // Puntatore alla sezione precedente.
  struct Section *next;   // Puntatore alla sezione successiva.

  VECTOR  center;         // Centro geometrico della sezione.
  SVECTOR normal;         // Normale della sezione.
  VECTOR basevertex;      // Vertice base per questa sezione.

  short numfaces;         // Numero di facce nella sezione.
  short facestart;        // Indice della faccia iniziale nella lista delle facce.
} Section;

// Definizione della struttura Track per rappresentare l'intera pista.
typedef struct Track {
  long numvertices;       // Numero totale di vertici nella pista.
  VECTOR *vertices;       // Puntatore all'array dei vertici.

  long numfaces;          // Numero totale di facce nella pista.
  Face *faces;            // Puntatore all'array delle facce.

  long numsections;       // Numero totale di sezioni nella pista.
  Section *sections;      // Puntatore all'array delle sezioni.
} Track;

// Funzione per caricare i vertici della pista da un file.
// Parametri:
// - track: puntatore alla struttura Track da popolare.
// - filename: nome del file contenente i vertici.
void LoadTrackVertices(Track *track, char *filename);

// Funzione per caricare le sezioni della pista da un file.
// Parametri:
// - track: puntatore alla struttura Track da popolare.
// - filename: nome del file contenente le sezioni.
void LoadTrackSections(Track *track, char *filename);

// Funzione per caricare le facce della pista da un file.
// Parametri:
// - track: puntatore alla struttura Track da popolare.
// - filename: nome del file contenente le facce.
// - texturestart: indice iniziale delle texture per la pista.
void LoadTrackFaces(Track *track, char *filename, u_short texturestart);

// Funzione per renderizzare la pista a partire da una sezione specifica.
// Parametri:
// - track: puntatore alla struttura Track contenente i dati della pista.
// - startsection: puntatore alla sezione di partenza per il rendering.
// - camera: puntatore alla telecamera utilizzata per la visualizzazione.
void RenderTrackAhead(Track *track, Section *startsection, Camera *camera);

#endif
