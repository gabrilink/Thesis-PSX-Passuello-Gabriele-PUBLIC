#include "globals.h"

// Stato dell'applicazione
int appState = STATE_MENU;
int selectedProgram = -1; // Nessun programma selezionato

// Variabili per il menu
MenuOption currentMenu = MENU_ITEM_1;

// Definizione degli elementi del menu
MenuItem menuItems[MENU_COUNT] = {
    // Colonna sinistra
    {10, 25, 140, 40, 90, 90, 90},  // Opzione 1
    {10, 75, 140, 40, 90, 90, 90},  // Opzione 2
    {10, 125, 140, 40, 90, 90, 90}, // Opzione 3
    {10, 175, 140, 40, 90, 90, 90}, // Opzione 4

    // Colonna destra
    {170, 25, 140, 40, 90, 90, 90}, // Opzione 5
    {170, 75, 140, 40, 90, 90, 90}, // Opzione 6
    {170, 125, 140, 40, 90, 90, 90},// Opzione 7
    {170, 175, 140, 40, 90, 90, 90} // Opzione 8
};

MenuItem menuItemsInner[MENU_COUNT] = {
    // Colonna sinistra
    {20, 30, 120, 30, 60, 60, 60},   // Opzione 1
    {20, 80, 120, 30, 60, 60, 60},   // Opzione 2
    {20, 130, 120, 30, 60, 60, 60},  // Opzione 3
    {20, 180, 120, 30, 60, 60, 60},  // Opzione 4

    // Colonna destra
    {180, 30, 120, 30, 60, 60, 60},  // Opzione 5
    {180, 80, 120, 30, 60, 60, 60},  // Opzione 6
    {180, 130, 120, 30, 60, 60, 60}, // Opzione 7
    {180, 180, 120, 30, 60, 60, 60}  // Opzione 8
};

// Definizione della variabile globale cube
Cube cube = {
    .rotation = {0, 0, 0},
    .position = {0, -400, 1800},
    .scale = {ONE, ONE, ONE},
    .vel = {0, 0, 0},
    .acc = {0, 0, 0},
    .vertices = {
        { -128, -128, -128 }, // 0
        {  128, -128, -128 }, // 1
        {  128, -128,  128 }, // 2
        { -128, -128,  128 }, // 3
        { -128,  128, -128 }, // 4
        {  128,  128, -128 }, // 5
        {  128,  128,  128 }, // 6
        { -128,  128,  128 }  // 7
    },
    .faces = {
        3, 2, 0, 1,  // Faccia superiore
        0, 1, 4, 5,  // Faccia anteriore
        4, 5, 7, 6,  // Faccia inferiore
        1, 2, 5, 6,  // Faccia destra
        2, 3, 6, 7,  // Faccia posteriore
        3, 0, 7, 4   // Faccia sinistra
    }
};

int isCubeInitialized = 0;

// Definizione delle variabili per i buffer e le primitive
u_long ot[2][OT_LEN];
char primbuff[2][PRIMBUFF_LEN];
char *nextprim;
u_short currbuff = 0;

// Funzioni per la gestione delle primitive e dell'Ordering Table
void EmptyOT(u_short currbuff) {
    ClearOTagR(ot[currbuff], OT_LEN);
}

u_long *GetOTAt(u_short currbuff, u_int i) {
    return &ot[currbuff][i];
}

void IncrementNextPrim(u_int size) {
    nextprim += size;
}

void ResetNextPrim(u_short currbuff) {
    nextprim = primbuff[currbuff];
}

char *GetNextPrim(void) {
    return nextprim;
}

// Definizione dei vertici del cubo
// Definizione aggiornata dei vertici del cubo
SVECTOR cubeVertices[8] = {
    { -64, -64, -64 }, // 0 - Vertice in basso a sinistra (retro)
    {  64, -64, -64 }, // 1 - Vertice in basso a destra (retro)
    {  64, -64,  64 }, // 2 - Vertice in basso a destra (fronte)
    { -64, -64,  64 }, // 3 - Vertice in basso a sinistra (fronte)
    { -64,  64, -64 }, // 4 - Vertice in alto a sinistra (retro)
    {  64,  64, -64 }, // 5 - Vertice in alto a destra (retro)
    {  64,  64,  64 }, // 6 - Vertice in alto a destra (fronte)
    { -64,  64,  64 }  // 7 - Vertice in alto a sinistra (fronte)
};


// Definizione delle facce del cubo
short cubeFaces[24] = {
    0, 1, 2, 3, // Faccia inferiore
    4, 5, 6, 7, // Faccia superiore
    0, 3, 7, 4, // Faccia sinistra
    1, 5, 6, 2, // Faccia destra
    3, 2, 6, 7, // Faccia frontale
    0, 4, 5, 1  // Faccia posteriore
};

// Definizione dei triangoli del cubo (12 triangoli, 36 indici)
// Triangoli aggiornati per le facce
short cubeTriangles[36] = {
    // Faccia inferiore
    0, 1, 3,
    1, 2, 3,
    // Faccia superiore
    4, 5, 7,
    5, 6, 7,
    // Faccia sinistra
    0, 3, 4,
    3, 7, 4,
    // Faccia destra
    1, 5, 2,
    5, 6, 2,
    // Faccia frontale
    3, 2, 7,
    2, 6, 7,
    // Faccia posteriore
    0, 4, 1,
    4, 5, 1
};

// Definizione dei colori per ogni faccia
CVECTOR colors[6] = {
    { 255,   0,   0 }, // Rosso (Faccia 1)
    {   0, 255,   0 }, // Verde (Faccia 2)
    {   0,   0, 255 }, // Blu (Faccia 3)
    { 255, 255,   0 }, // Giallo (Faccia 4)
    { 255,   0, 255 }, // Magenta (Faccia 5)
    {   0, 255, 255 }  // Ciano (Faccia 6)
};

// Funzione di calcolo distanza
    long Distance(const VECTOR *v1, const VECTOR *v2) {
        long dx = v1->vx - v2->vx;
        long dy = v1->vy - v2->vy;
        long dz = v1->vz - v2->vz;
        return SquareRoot0((dx * dx) + (dy * dy) + (dz * dz));
    }


    Plane planes[5] = {
        { // soffitto
            {0, 450, 1800},  // Posizione
            {ONE, ONE, ONE}, // Scala
            {0, 0, 0},       // Rotazione
            {                // Vertici
                {-900, 0, -900},
                {900, 0, -900},
                {-900, 0, 900},
                {900, 0, 900}
            },
            {0, 1, 2, 1, 3, 2} // Facce
        },
        { // Parete Sinistra
            {0, 450, 1800}, // Posizione
            {ONE, ONE, ONE},   // Scala
            {0, 0, 0},         // Rotazione
            {                  // Vertici
                {-900, 0, -900},
                {-900, 900, -900},
                {-900, 0, 900},
                {-900, 900, 900}
            },
            {1, 0, 3, 0, 2, 3} // Facce invertite
        },
        { // Parete Destra
            {0, 450, 1800},  // Posizione
            {ONE, ONE, ONE},   // Scala
            {0, 0, 0},         // Rotazione
            {                  // Vertici
                {900, 0, -900},
                {900, 900, -900},
                {900, 0, 900},
                {900, 900, 900}
            },
            {0, 1, 2, 1, 3, 2} // Facce
        },
        { // Parete Posteriore
            {0, 450, 1800},     // Posizione
            {ONE, ONE, ONE},   // Scala
            {0, 0, 0},         // Rotazione
            {                  // Vertici
                {-900, 0, 900},
                {900, 0, 900},
                {-900, 900, 900},
                {900, 900, 900}
            },
            {0, 1, 2, 1, 3, 2} // Facce
        },
        { // pavimento
            {0, 450, 1800},   // Posizione
            {ONE, ONE, ONE},   // Scala
            {0, 0, 0},         // Rotazione
            {                  // Vertici
                {-900, 900, -900},
                {-900, 900, 900},
                {900, 900, -900},
                {900, 900, 900}
            },
            {0, 1, 2, 1, 3, 2} // Facce
        }
    };


int current_scr_z = 500;



// Wrapper for SetGeomScreen to track the value
void CustomSetGeomScreen(int scr_z) {
    current_scr_z = scr_z; // Save the screen distance value
    SetGeomScreen(scr_z);  // Call the actual SetGeomScreen function
}

// Getter to retrieve the saved SCR_Z value
int CustomGetGeomScreen(void) {
    return current_scr_z; // Return the last set SCR_Z value
}


