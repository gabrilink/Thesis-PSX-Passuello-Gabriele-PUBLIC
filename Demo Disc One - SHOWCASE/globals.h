#ifndef GLOBALS_H
#define GLOBALS_H

#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libetc.h>

// Definizione della struttura Cube
typedef struct Cube {
    SVECTOR rotation;
    VECTOR position;
    VECTOR scale;
    VECTOR vel;
    VECTOR acc;
    SVECTOR vertices[8];
    short faces[24];
} Cube;

// Definizione della struttura MenuItem
typedef struct {
    int x, y, width, height;
    u_char r, g, b;
} MenuItem;


typedef struct {
        VECTOR position;
        VECTOR scale;
        SVECTOR rotation;
        SVECTOR vertices[4];
        short faces[6];
        CVECTOR color; // Assicuriamoci che questo membro sia definito
    } Plane;

typedef struct {
    int type;          // 0: Plane, 1: Cube
    int index;         // Index for planes
    long depth;        // Depth (calculated as average Z)
} RenderItem;

// Definizione delle costanti
#define OT_LEN 8192
#define PRIMBUFF_LEN 131072
#define STATE_MENU 0
#define STATE_PROGRAM 1

// Definizione dell'enumerazione per le opzioni del menu
typedef enum {
    MENU_ITEM_1,
    MENU_ITEM_2,
    MENU_ITEM_3,
    MENU_ITEM_4,
    MENU_ITEM_5,
    MENU_ITEM_6,
    MENU_ITEM_7,
    MENU_ITEM_8,
    MENU_COUNT
} MenuOption;

// Variabili globali
extern int appState;           // Stato corrente dell'applicazione
extern int selectedProgram;    // Programma selezionato
extern MenuOption currentMenu;
extern MenuItem menuItems[MENU_COUNT];
extern MenuItem menuItemsInner[MENU_COUNT];

// Variabili per la gestione dei buffer e delle primitive
extern u_long ot[2][OT_LEN];
extern char primbuff[2][PRIMBUFF_LEN];
extern char *nextprim;
extern u_short currbuff;

// Funzioni per la gestione delle primitive e dell'Ordering Table
void EmptyOT(u_short currbuff);
u_long *GetOTAt(u_short currbuff, u_int i);
void IncrementNextPrim(u_int size);
void ResetNextPrim(u_short currbuff);
char *GetNextPrim(void);

long Distance(const VECTOR *v1, const VECTOR *v2);


// Variabili globali per il cubo
extern Cube cube;
extern int isCubeInitialized;
extern SVECTOR cubeVertices[8];
extern short cubeFaces[24];
extern short cubeTriangles[36];
extern CVECTOR colors[6];
extern Plane planes[5];


extern int current_scr_z;
void CustomSetGeomScreen(int scr_z);
int CustomGetGeomScreen(void);

#endif // GLOBALS_H
