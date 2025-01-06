#include "camera.h"
#include "display.h"
#include "globals.h"
#include "inline_n.h"
#include "joypad.h"
#include "object.h"
#include "utils.h"
#include <libcd.h>
#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>
#include <libpress.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgs.h>
#include <libapi.h>

#include "MARITEX.h"
#include "MARYLIN.h"

#include <sys/types.h>

// Include the STR player library
#include "strplay.h"
// Per la funzione phong
#include "PHONG.h"

#include "Multiplayer.h"

// Variabili globali necessarie
POLY_FT4 *polyft4;

MATRIX worldmat = {0};
MATRIX viewmat = {0};

Obj obj;

u_long timmode; /* Pixel mode of the TIM */
RECT timprect;  /* Stores the X,Y location of the texture */
RECT timcrect;  /* Stores the X,Y location of the CLUT */

size_t totalAllocatedMemory = 0;
#define REQUIRED_FREE_MEMORY 196608 // 192 KB
#define TOTAL_MEMORY 2097152       // 2 MB
#define STATIC_MEMORY_USED 2048 // Es. 2 KB
#define STACK_MEMORY 8192 // 8 KB stimati per la pila



// Handle per l'unica finestra di font
int fontHandle;

void RunHelloWorld(void);
void RunProgram1(void);
void RunProgram2(void);
void RunProgram3(void);
void RunProgram4(void);
void RunProgram5(void);
void RunProgram6(void);
void RunProgram7(void);
void RunProgram8(void);
void RunSelectedProgram(void);


// Funzione per inizializzare una finestra di testo
void InitializeFont(void) {
  FntLoad(960, 256); // Carica il font dal BIOS
  fontHandle = FntOpen(0, 0, 320, 240, 0, 512); // Una finestra di testo sull'intero schermo
}

// Funzione per disegnare il menu
void DrawMenu(void) {
    int i;

    // Disegna i riquadri esterni
    for (i = 0; i < MENU_COUNT; i++) {
        POLY_F4 *poly = (POLY_F4 *)GetNextPrim();
        setPolyF4(poly);
        setXYWH(poly, menuItems[i].x, menuItems[i].y, menuItems[i].width, menuItems[i].height);
        setRGB0(poly, menuItems[i].r, menuItems[i].g, menuItems[i].b);
        addPrim(GetOTAt(GetCurrBuff(), OT_LEN - 1), poly);
        IncrementNextPrim(sizeof(POLY_F4));
    }

    // Disegna i riquadri interni
    for (i = 0; i < MENU_COUNT; i++) {
        POLY_F4 *poly = (POLY_F4 *)GetNextPrim();
        setPolyF4(poly);
        setXYWH(poly, menuItemsInner[i].x, menuItemsInner[i].y, menuItemsInner[i].width, menuItemsInner[i].height);
        setRGB0(poly, menuItemsInner[i].r, menuItemsInner[i].g, menuItemsInner[i].b);
        addPrim(GetOTAt(GetCurrBuff(), OT_LEN - 1), poly);
        IncrementNextPrim(sizeof(POLY_F4));
    }

    InitializeFont(); // Inizializza il font

    // Stampa il testo del menu con la nuova opzione "HELLO WORLD"
    SetDumpFnt(fontHandle);
    FntPrint("\n\n\n\n       Cube               Texture");
    FntPrint("\n\n  Transformations         Mapping");
    FntPrint("\n\n\n\n\n  Bouncing Cubes       Movie / M-DEC");
    FntPrint("\n\n\n\n\n\n    Multiplayer             FOG");
    FntPrint("\n\n\n\n\n\n\n       Phong            3D ANIMATION");
    FntPrint("\n\n\n\n                          START: Credits"); // Nuova opzione
    FntFlush(-1); // Renderizza il testo
}


void Setup(void) {
  ScreenInit();                 // Initialize the display
  CdInit();                     // Initialize the CD subsystem
  JoyPadInit();                 // Initialize the joypad
  ResetNextPrim(GetCurrBuff()); // Reset primitives
  InitializeFont();             // Setup the font
  CustomSetGeomScreen(CustomGetGeomScreen());
}


void ShowLoadingScreen(int seconds) {
    int i;
    char buffer[32];
    int fontHandle;

    // Configura l'ambiente grafico
    DISPENV disp;
    DRAWENV draw;

    SetDefDispEnv(&disp, 0, 0, 320, 240); // Ambiente di visualizzazione
    SetDefDrawEnv(&draw, 0, 0, 320, 240); // Ambiente di disegno
    PutDispEnv(&disp);
    PutDrawEnv(&draw);

    // Carica e configura il font
    FntLoad(960, 256);
    fontHandle = FntOpen(80, 100, 160, 50, 0, 512);

    // Loop per il conto alla rovescia
    for (i = seconds; i > 0; i--) {
        //ClearOTagR(GetCurrBuff(), OT_LEN); // Pulisce l'Order Table
        EmptyOT(GetCurrBuff());

        // Mostra il conto alla rovescia
        sprintf(buffer, "Loading... %d", i);
        SetDumpFnt(fontHandle);
        FntPrint("\n\n%s", buffer);
        FntFlush(-1);

        // Attendi un secondo (1 VSync per ogni frame su PAL, 50Hz)
        VSync(50);

        // Sincronizza i buffer
        DrawSync(0);
        DisplayFrame();
    }

    //FntClose(fontHandle); // Chiude la finestra del font
}

// Funzione per eseguire il programma selezionato
void RunSelectedProgram(void) {
  switch (selectedProgram) {
  case MENU_ITEM_1:
    RunProgram1();
    break;
  case MENU_ITEM_2:
    RunProgram2();
    break;
  case MENU_ITEM_3:
    RunProgram3();
    break;
  case MENU_ITEM_4:
    RunProgram4();
    break;
  case MENU_ITEM_5:
    RunProgram5();
    break;
  case MENU_ITEM_6:
    RunProgram6();
    break;
  case MENU_ITEM_7:
    RunProgram7();
    break;
  case MENU_ITEM_8:
    RunProgram8();
    break;
  default:
    printf("Invalid selectedProgram: %d\n", selectedProgram); // Debugging
    break;
  }
}

// Funzione per aggiornare lo stato
void Update(void) {
    EmptyOT(GetCurrBuff());
    JoyPadUpdate();

    if (appState == STATE_MENU) {
        DrawMenu();

        // Controlla se START è premuto per eseguire la nuova funzione
        if (GetPadState() & PAD1_START) {
            appState = STATE_PROGRAM; // Cambia stato dell'applicazione
            RunHelloWorld();          // Esegui la funzione
        }
    } else if (appState == STATE_PROGRAM) {
        if (selectedProgram != -1) {
            printf("Running selectedProgram: %d\n", selectedProgram); // Debugging
            RunSelectedProgram();
        }
    }

    DisplayFrame();
}

void RunHelloWorld(void) {
    u_long padState;

    while (1) {
        JoyPadUpdate();
        padState = GetPadState();


        // Controlla se SELECT è premuto per tornare al menu
        if (padState & PAD1_SELECT) {
            appState = STATE_MENU; // Torna al menu
            selectedProgram = -1;  // Resetta il programma selezionato
            return;
        }

        // Resetta la schermata con il colore nero
        setRGB0(&screen.draw[currbuff], 0, 0, 0); // Sfondo nero

        EmptyOT(GetCurrBuff());
        //ClearOTagR(GetCurrBuff(), OT_LEN);

        // Disegna il testo "HELLO WORLD" al centro dello schermo
        SetDumpFnt(fontHandle);
        FntPrint("\n\n             Demo Disc One              "); // Testo centrato

        FntPrint("\n\n\n STUDENT : GABRIELE PASSUELLO"); // Testo centrato
        FntPrint("\n\n Thesis Supervisor : DEMIS BALLIS"); // Testo centrato

        FntPrint("\n\n\n           UNIVERSITY OF UDINE            "); // Testo centrato
        FntPrint("\n\n Multimedia Science and Technology - \n\n bachelor degree 2025"); // Testo centrato

        FntPrint("\n\n\n\n Thesis Title :\n\n Retrocomputing su PlayStation 1:\n progettazione e sviluppo di due \n Tech Demo in occasione del trentesimo \n anniversario della console"); // Testo centrato

        FntFlush(-1);

        // Aggiorna il frame
        DisplayFrame();
        VSync(0);
    }
}


// Funzione principale
int main(void) {
  Setup();
  while (1) {
    Update();
  }
  return 0;
}

void RunProgram1(void) {
  static SVECTOR cubeRotation = {0, 0, 0};   // Cube rotation
  static VECTOR cubePosition = {0, 0, 0};    // Cube position
  static VECTOR cubeScale = {ONE, ONE, ONE}; // Cube scale
  const int MIN_SCALE = ONE / 10;            // Minimum scale limit
  const int MAX_SCALE = ONE * 5;             // Maximum scale limit
  static int isCubeInitialized = 0;          // Initialization flag
  MATRIX transformMatrix;                    // Transformation matrix
  u_long currentPadState;                    // Controller state
  int i;                                     // Loop variable

  // Transform and project the vertices using RotAverageNclip4
  int nclip;
  long otz, p, flg;

  // Initialize the cube on first run
  if (!isCubeInitialized) {
    cubeRotation.vx = 0;
    cubeRotation.vy = 0;
    cubeRotation.vz = 0;
    cubePosition.vx = 0;
    cubePosition.vy = 0;
    cubePosition.vz = 2000;
    cubeScale.vx = ONE;
    cubeScale.vy = ONE;
    cubeScale.vz = ONE; // Reset scale
    isCubeInitialized = 1;
  }

  // Get controller state
  currentPadState = GetPadState();

  // Rotate the cube based on controller input
  if (currentPadState & PAD1_SQUARE) {
    cubeRotation.vx += 16; // Rotate around X-axis
  }
  if (currentPadState & PAD1_CIRCLE) {
    cubeRotation.vz += 16; // Rotate around Z-axis
  }
  if (currentPadState & PAD1_TRIANGLE) {
    cubeRotation.vy += 16; // Rotate around Y-axis
  }

  // Reset cube rotation and position
  if (currentPadState & PAD1_CROSS) {
    cubeRotation.vx = 0;
    cubeRotation.vy = 0;
    cubeRotation.vz = 0;
    cubePosition.vx = 0;
    cubePosition.vy = 0;
    cubePosition.vz = 2000;
    cubeScale.vx = ONE;
    cubeScale.vy = ONE;
    cubeScale.vz = ONE; // Reset scale
  }

  // Modify the cube's scale based on button presses
  if (currentPadState & PAD1_R1) {
    // Increase scale if below maximum limit
    if (cubeScale.vx < MAX_SCALE)
      cubeScale.vx += ONE / 10;
    if (cubeScale.vy < MAX_SCALE)
      cubeScale.vy += ONE / 10;
    if (cubeScale.vz < MAX_SCALE)
      cubeScale.vz += ONE / 10;
  }
  if (currentPadState & PAD1_L2) {
    // Decrease scale if above minimum limit
    if (cubeScale.vx > MIN_SCALE)
      cubeScale.vx -= ONE / 10;
    if (cubeScale.vy > MIN_SCALE)
      cubeScale.vy -= ONE / 10;
    if (cubeScale.vz > MIN_SCALE)
      cubeScale.vz -= ONE / 10;
  }
  if (currentPadState & PAD1_UP) {
    // Increase scale on Y-axis if below maximum limit
    if (cubeScale.vy < MAX_SCALE)
      cubeScale.vy += ONE / 10;
  }
  if (currentPadState & PAD1_LEFT) {
    // Increase scale on X-axis if below maximum limit
    if (cubeScale.vx < MAX_SCALE)
      cubeScale.vx += ONE / 10;
  }
  if (currentPadState & PAD1_RIGHT) {
    // Increase scale on Z-axis if below maximum limit
    if (cubeScale.vz < MAX_SCALE)
      cubeScale.vz += ONE / 10;
  }
  if (currentPadState & PAD1_DOWN) {
    // Reset scale to original
    cubeScale.vx = ONE;
    cubeScale.vy = ONE;
    cubeScale.vz = ONE;
  }

  // Prepare transformation matrix
  RotMatrix(&cubeRotation, &transformMatrix);
  TransMatrix(&transformMatrix, &cubePosition); // Apply position
  ScaleMatrix(&transformMatrix, &cubeScale);    // Apply scale
  SetRotMatrix(&transformMatrix);
  SetTransMatrix(&transformMatrix);

  // Change background color (e.g., light blue)
  setRGB0(&screen.draw[currbuff], 30, 60, 120);

  // Render the cube using RotAverageNclip4
  for (i = 0; i < 24; i += 4) {
    POLY_G4 *poly = (POLY_G4 *)GetNextPrim();
    setPolyG4(poly);

    // Colors for vertices (Gouraud shading)
    setRGB0(poly, 255, 0, 0);   // Red
    setRGB1(poly, 0, 255, 0);   // Green
    setRGB2(poly, 0, 0, 255);   // Blue
    setRGB3(poly, 255, 255, 0); // Yellow

    nclip = RotAverageNclip4(
        &cube.vertices[cube.faces[i + 0]], &cube.vertices[cube.faces[i + 1]],
        &cube.vertices[cube.faces[i + 2]], &cube.vertices[cube.faces[i + 3]],
        (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
        (long *)&poly->x3, &p, &otz, &flg);

    // Skip face if it's outside the view frustum
    if (nclip <= 0) {
      continue;
    }

    // Add primitive to ordering table if within valid depth range
    if ((otz > 0) && (otz < OT_LEN)) {
      addPrim(GetOTAt(GetCurrBuff(), otz), poly);
      IncrementNextPrim(sizeof(POLY_G4));
    }
  }

  // Stampa il testo del menu allineato manualmente
  SetDumpFnt(fontHandle);
  FntPrint("\n\n Rotation XYZ: (%d)(%d)(%d)", cubeRotation.vx, cubeRotation.vy,
           cubeRotation.vz);
  FntPrint("\n\n Scale XYZ: (%d)(%d)(%d)", cubeScale.vx, cubeScale.vy,
           cubeScale.vz);

  // Renderizza il testo
  FntFlush(-1);
}

void RunProgram5(void) {
  int i, q, nclip;
  long otz;
  u_long padState;

  // Reset del puntatore dei primitivi
  ResetNextPrim(GetCurrBuff());

  // Configurazione della camera
  setVector(&camera.position, 500, -1000, -1200);
  camera.lookat = (MATRIX){0};

  // Configurazione dell'oggetto
  setVector(&obj.position, 0, 0, 0);
  setVector(&obj.rotation, 0, 0, 0);
  setVector(&obj.scale, ONE, ONE, ONE);

  // Caricamento del modello e della texture
  LoadModel("\\MODEL.BIN;1");
  LoadTexture_v2("\\UNIUD.TIM;1");

  // Loop principale
  while (1) {
    JoyPadUpdate();
    padState = GetPadState();

    // Ritorna al menu
    if (JoyPadCheck(PAD1_SELECT)) {
      appState = STATE_MENU;
      return;
    }

    // Svuota la Ordering Table
    EmptyOT(GetCurrBuff());

    // Controlli della camera
    if (JoyPadCheck(PAD1_LEFT)) {
      camera.position.vx -= 50;
    }
    if (JoyPadCheck(PAD1_RIGHT)) {
      camera.position.vx += 50;
    }
    if (JoyPadCheck(PAD1_UP)) {
      camera.position.vy -= 50;
    }
    if (JoyPadCheck(PAD1_DOWN)) {
      camera.position.vy += 50;
    }
    if (JoyPadCheck(PAD1_CROSS)) {
      camera.position.vz += 50;
    }
    if (JoyPadCheck(PAD1_CIRCLE)) {
      camera.position.vz -= 50;
    }

    if (JoyPadCheck(PAD1_TRIANGLE)) {
      LoadTexture_v2("\\UNIUD.TIM;1");
    }

    if (JoyPadCheck(PAD1_SQUARE)) {
      LoadTexture_v2("\\BRICKS.TIM;1");
    }

    // Calcolo della matrice LookAt della camera
    LookAt(&camera, &camera.position, &obj.position, &(VECTOR){0, -ONE, 0});

    // Trasformazioni dell'oggetto
    RotMatrix(&obj.rotation, &worldmat);
    TransMatrix(&worldmat, &obj.position);
    ScaleMatrix(&worldmat, &obj.scale);

    // Composizione della View Matrix
    CompMatrixLV(&camera.lookat, &worldmat, &viewmat);

    SetRotMatrix(&viewmat);
    SetTransMatrix(&viewmat);

    // Rendering dell'oggetto
    for (i = 0, q = 0; i < obj.numfaces; i += 4, q++) {
      polyft4 = (POLY_FT4 *)GetNextPrim();
      setPolyFT4(polyft4);

      polyft4->r0 = 128;
      polyft4->g0 = 128;
      polyft4->b0 = 128;

      polyft4->u0 = 0;
      polyft4->v0 = 0;
      polyft4->u1 = 63;
      polyft4->v1 = 0;
      polyft4->u2 = 0;
      polyft4->v2 = 63;
      polyft4->u3 = 63;
      polyft4->v3 = 63;

      polyft4->tpage = getTPage(timmode & 0x3, 0, timprect.x, timprect.y);
      polyft4->clut = getClut(timcrect.x, timcrect.y);

      // Caricamento dei primi 3 vertici
      gte_ldv0(&obj.vertices[obj.faces[i + 0]]);
      gte_ldv1(&obj.vertices[obj.faces[i + 1]]);
      gte_ldv2(&obj.vertices[obj.faces[i + 2]]);

      gte_rtpt();

      gte_nclip();
      gte_stopz(&nclip);

      // Culling delle facce non visibili
      if (nclip < 0) {
        continue;
      }

      // Salvataggio delle coordinate trasformate
      gte_stsxy0(&polyft4->x0);

      // Caricamento del quarto vertice
      gte_ldv0(&obj.vertices[obj.faces[i + 3]]);

      // Trasformazione e proiezione
      gte_rtps();

      // Salvataggio delle coordinate
      gte_stsxy3(&polyft4->x1, &polyft4->x2, &polyft4->x3);

      gte_avsz4();
      gte_stotz(&otz);

      if ((otz > 0) && (otz < OT_LEN)) {
        addPrim(GetOTAt(GetCurrBuff(), otz), polyft4);
        IncrementNextPrim(sizeof(POLY_FT4));
      }
    }

    // Aggiornamento della rotazione dell'oggetto
    obj.rotation.vy += 20;

    // Rendering del frame
    DisplayFrame();
  }
}

void RunProgram2(void) {
#define MAX_CUBES 600
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

  typedef struct {
    VECTOR position;   // Posizione del cubo
    VECTOR velocity;   // Velocit� del cubo
    CVECTOR colors[4]; // Colori per ogni vertice di una faccia
  } CUBE;

  CUBE cubes[MAX_CUBES];
  int cubeCount = 1; // Inizia con un solo cubo
  int paused = 0;
  int i, j, nclip;
  MATRIX transformMatrix;
  POLY_G4 *poly;
  u_long padState;

  int VELOCITY = 20;

  // Variabile per la scala dei cubi
  int cubeScale = 8;        // Dimensione iniziale del cubo
  const int MIN_SCALE = 8;  // Scala minima
  const int MAX_SCALE = 32; // Scala massima

  // Inizializza i cubi
  for (i = 0; i < MAX_CUBES; i++) {
    cubes[i].position.vx = rand() % SCREEN_WIDTH;
    cubes[i].position.vy = rand() % SCREEN_HEIGHT;
    cubes[i].position.vz = 2000; // Distanza predefinita

    cubes[i].velocity.vx = (rand() % (2 * VELOCITY)) - VELOCITY;
    cubes[i].velocity.vy = (rand() % (2 * VELOCITY)) - VELOCITY;
    cubes[i].velocity.vz = 0;

    // Colori casuali distintivi per ogni faccia del cubo
    for (j = 0; j < 4; j++) {
      cubes[i].colors[j].r = rand() % 255;
      cubes[i].colors[j].g = rand() % 255;
      cubes[i].colors[j].b = rand() % 255;
    }
  }

  while (1) {
    JoyPadUpdate();
    padState = GetPadState();

    // Ritorna al menu
    if (JoyPadCheck(PAD1_SELECT)) {
      appState = STATE_MENU;
      selectedProgram = -1;

      // Ripristina il colore dello sfondo
      setRGB0(&screen.draw[0], 30, 30, 30); // Colore originale
      setRGB0(&screen.draw[1], 30, 30, 30); // Colore originale
      break;
    }

    // Modifica il numero di cubi
    if (padState & PAD1_UP && cubeCount < MAX_CUBES) {
      cubeCount++;
    }
    if (padState & PAD1_DOWN && cubeCount > 1) {
      cubeCount--;
    }

    // Modifica la scala dei cubi
    if (padState & PAD1_RIGHT && cubeScale < MAX_SCALE) {
      cubeScale += 4; // Incrementa la scala
    }
    if (padState & PAD1_LEFT && cubeScale > MIN_SCALE) {
      cubeScale -= 4; // Decrementa la scala
    }

    // Aggiorna le posizioni dei cubi
    for (i = 0; i < cubeCount; i++) {
      cubes[i].position.vx += cubes[i].velocity.vx;
      cubes[i].position.vy += cubes[i].velocity.vy;

      // Controlla i bordi dello schermo e gestisce il rimbalzo
      if (cubes[i].position.vx <= 0 ||
          cubes[i].position.vx >= SCREEN_WIDTH - cubeScale) {
        cubes[i].velocity.vx = -cubes[i].velocity.vx;
      }

      if (cubes[i].position.vy <= 0 ||
          cubes[i].position.vy >= SCREEN_HEIGHT - cubeScale) {
        cubes[i].velocity.vy = -cubes[i].velocity.vy;
      }
    }

    // Rendering
    EmptyOT(GetCurrBuff());
    for (i = 0; i < cubeCount; i++) {
      // Crea la matrice di trasformazione per la posizione
      TransMatrix(&transformMatrix, &cubes[i].position);
      SetTransMatrix(&transformMatrix);

      // Disegna il cubo scalato
      for (j = 0; j < 6; j++) { // Per ogni faccia
        poly = (POLY_G4 *)GetNextPrim();
        setPolyG4(poly);

        // Imposta i colori per i vertici della faccia
        setRGB0(poly, cubes[i].colors[0].r, cubes[i].colors[0].g,
                cubes[i].colors[0].b);
        setRGB1(poly, cubes[i].colors[1].r, cubes[i].colors[1].g,
                cubes[i].colors[1].b);
        setRGB2(poly, cubes[i].colors[2].r, cubes[i].colors[2].g,
                cubes[i].colors[2].b);
        setRGB3(poly, cubes[i].colors[3].r, cubes[i].colors[3].g,
                cubes[i].colors[3].b);

        // Aggiungi scala ai vertici
        poly->x0 = cubes[i].position.vx - cubeScale / 2;
        poly->y0 = cubes[i].position.vy - cubeScale / 2;

        poly->x1 = cubes[i].position.vx + cubeScale / 2;
        poly->y1 = cubes[i].position.vy - cubeScale / 2;

        poly->x2 = cubes[i].position.vx - cubeScale / 2;
        poly->y2 = cubes[i].position.vy + cubeScale / 2;

        poly->x3 = cubes[i].position.vx + cubeScale / 2;
        poly->y3 = cubes[i].position.vy + cubeScale / 2;

        addPrim(GetOTAt(GetCurrBuff(), OT_LEN - 1), poly);
        IncrementNextPrim(sizeof(POLY_G4));
      }
    }

    // Mostra la scala corrente
    SetDumpFnt(fontHandle);
    FntPrint("\n\nCube Scale: %d", cubeScale);
    FntPrint("\nCube Count: %d", cubeCount);

    FntFlush(-1);

    DrawSync(0);
    VSync(0);
    DisplayFrame();
  }
}

void RunProgram6(void) {
  u_long padState;

  STRFILE StrFile[] = {"\\SPYRO.STR;1", 320, 240, 454};

  //ResetCallback();
  CdInit();
  PadInit(0);
  ResetGraph(0);
  SetGraphDebug(0);

  while (1) {
    padState = GetPadState(); // Ensure pad state is updated

    PlayStr(320, 240, 0, 0, &StrFile[0]);
    printf("Video playback completed\n"); // Debugging
    appState = STATE_MENU;
    selectedProgram = -1;                               // Ensure reset
    printf("Exiting RunProgram6: Returning to menu\n"); // Debugging
    break;
  }
}

void RunProgram4(void) {

  DB db[2]; /* packet double buffer */
  DB *cdb;  /* current db */
  int i, j, k;
  long ret;

  // Backup SCR_Z value
  int backup_scr_z = CustomGetGeomScreen();

  SVECTOR QCOL[8][2]; /*3D coordinates*/
  int center[3];
  int hight;
  int width;
  int depth;
  int cut;            /*corner cut*/
  short SV_top[8][2]; /*screen coordinate of top*/
  short SV_bot[8][2]; /*screen coordinate of bottom*/
  int OTZ_top[8];     /*OTZ of top vertex*/
  int OTZ_bot[8];     /*OTZ of bottom vertex*/
  SVECTOR NML_TB[2];  /*normal vector of top&bottom face*/
  SVECTOR NML_SD[4];  /*normal vector of side face*/
  CVECTOR COL_TB[2];  /*color of top/bottom face*/
  CVECTOR COL_SD[4];  /*color of side face*/

  //ResetCallback();
  //PadInit(0);       /* reset graphic environment */
  //ResetGraph(1);    /* reset graphic subsystem (0:cold,1:warm) */
  //SetGraphDebug(0); /* set debug mode (0:off, 1:monitor, 2:dump) */

  // InitGeom();			/* initialize geometry subsystem */
  SetGeomOffset(OFX, OFY);     /* set geometry origin as (160, 120) */
  SetGeomScreen(SCR_Z);        /* distance to viewing-screen */
  SetBackColor(RBK, GBK, BBK); /* set background(ambient) color*/
  SetFarColor(RFC, GFC, BFC);  /* set far color */
  SetFogNear(FOGNEAR, SCR_Z);  /* set fog parameter*/

  /*interlace mode*/
  SetDefDrawEnv(&db[0].draw, 0, 0, PIH, PIV);
  SetDefDrawEnv(&db[1].draw, 0, PIV, PIH, PIV);
  SetDefDispEnv(&db[0].disp, 0, PIV, PIH, PIV);
  SetDefDispEnv(&db[1].disp, 0, 0, PIH, PIV);

  //SetDispMask(1); /* enable to display (0:inhibit, 1:enable) */

  init_prim(&db[0]);
  init_prim(&db[1]);

  center[0] = 500;
  center[1] = 0;
  center[2] = 0;
  hight = 1000;
  width = 1000;
  depth = 1000;
  cut = 100;
  object_col.r = 255;
  object_col.g = 0;
  object_col.b = 0;

  /*set lighting constants*/
  set_light_color(&lmat, &cmat);
  SetColorMatrix(&cmat);

  /*set vertex coordinates&normals*/
  set_column_vertex(QCOL, center, hight, width, depth, cut);
  set_column_normal_4096(NML_TB, NML_SD);

  ret = 0;

  while (pad_read() == 0) {

    JoyPadUpdate(); // Aggiunge l'aggiornamento del joypad

    // Ritorna al menu principale quando viene premuto SELECT
    if (JoyPadCheck(PAD1_SELECT)) {

      appState = STATE_MENU; // Cambia lo stato dell'applicazione
      selectedProgram = -1;  // Resetta il programma selezionato


      CustomSetGeomScreen(backup_scr_z);
      InitGeom();
      SetGeomOffset(SCREEN_CENTER_X, SCREEN_CENTER_Y);
      SetGeomScreen(SCREEN_Z);
      return;                // Esci da RunProgram4
    }

    cdb = (cdb == db) ? db + 1 : db; /* swap double buffer ID */
    ClearOTagR(cdb->ot, OTSIZE);
    DrawSync(0); /* wait for end of BG drawing */

    /*LEFT(phong)*/
    /*set LEFT object coordinate*/
    SetRotMatrix(&WL1mat);
    SetTransMatrix(&WL1mat);

    /*rotate&transfer&perspective of LEFT object*/
    rot_column(QCOL, SV_top, SV_bot, OTZ_top, OTZ_bot);

    /*lighting of LEFT object*/
    col_column(NML_TB, NML_SD, COL_TB, COL_SD);

    /*make GPU packet for LEFT object*/
    set_primP(SV_top, SV_bot, COL_TB, COL_SD, cdb, OTZ_top, OTZ_bot);

    /*add prim for LEFT object*/
    add_primP(cdb);

    /*phong shade & draw in texture area for LEFT object*/
    phong_column_edge(SV_top, SV_bot, NML_TB, NML_SD);

    DrawSync(0); /* wait for end of BG drawing */

    VSync(0); /* wait for the next V-BLNK */

    PutDrawEnv(&cdb->draw); /* update drawing environment */
    PutDispEnv(&cdb->disp); /* update display environment */

    DrawOTag(cdb->ot + OTSIZE - 1); /* draw BG*/
  }

  // Failsafe: Restore SCR_Z
  CustomSetGeomScreen(backup_scr_z);
  // PadStop();
  ResetGraph(3);
  // StopCallback();
}

void RunProgram3(void) {
  #define ever                                                                   \
    ;                                                                            \
    ;

  // Backup SCR_Z value and other graphics parameters
  int backup_scr_z = CustomGetGeomScreen();
  int backup_offset_x = SCREEN_CENTER_X;
  int backup_offset_y = SCREEN_CENTER_Y;

  // Per i colori dei quadrati
  static CVECTOR colors[4] = {
      {255, 0, 255, 0}, {255, 255, 255, 0}, {0, 0, 0, 0}, {255, 255, 0, 0}};

  int i;
  System *system = System_get_instance();
  POLY_F4 testpoly[4];

  // Inizializza il sistema grafico
  System_init(system);
  System_init_graphics(system);

  // Inizializza i primitivi
  for (i = 0; i < 4; ++i) {
    SetPolyF4(&testpoly[i]);
    setXYWH(&testpoly[i], 10, 30, 40, 40);
    setRGB0(&testpoly[i], colors[i].r, colors[i].g, colors[i].b);
  }

  for (ever) {
    JoyPadUpdate(); // Aggiunge l'aggiornamento del joypad

    // Ritorna al menu principale quando viene premuto SELECT
    if (JoyPadCheck(PAD1_SELECT)) {
      appState = STATE_MENU; // Cambia lo stato dell'applicazione
      selectedProgram = -1;  // Resetta il programma selezionato

      FntFlush(-1);
      DrawSync(0);

      // Restore geometry and rendering parameters
      CustomSetGeomScreen(backup_scr_z);
      InitGeom();
      SetGeomOffset(backup_offset_x, backup_offset_y);
      SetGeomScreen(SCREEN_Z);
      RunProgram4();//SOLO PER RISOLVERE ERRORE GRAFICO. BUG FIX
      return; // Esci da RunProgram3
    }

    // Imposta l'ambiente di visualizzazione
    System_set_dispenv(system);

    // Disegna ogni viewport
    for (i = 0; i < NUM_VIEWPORTS; ++i) {
      System_start_frame(system, i);

      FntPrint("Schermo %d\n", i);

      // Aggiungi il primitivo alla order table
      System_add_prim(system, &testpoly[i], 1, i);

      System_end_frame(system, i);

      // Sincronizzazione per ogni viewport
      FntFlush(-1);
      DrawSync(0);
    }

    // Sincronizzazione verticale
    VSync(0);
  }

  ResetGraph(3); // Reset the graphics subsystem
  System_deinit(system); // Deinizializza il sistema

}

void RunProgram7(void) {
  static SVECTOR cubeRotations[3] = {
      {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; // Rotazione cubi
  static VECTOR cubePositions[3] = {
      {0, 0, 1000}, {-200, 0, 1000}, {200, 0, 1000}}; // Posizioni cubi più vicine
  static VECTOR cubeScales[3] = {
      {ONE / 2, ONE / 2, ONE / 2}, {ONE / 2, ONE / 2, ONE / 2}, {ONE / 2, ONE / 2, ONE / 2}}; // Scale cubi
  const int cubeInitialZ = 1000; // Posizione iniziale
  const CVECTOR fogColor = {200, 200, 255}; // Colore della nebbia
  const CVECTOR backgroundColor = {30, 60, 120}; // Colore background
  const CVECTOR cubeColors[3][4] = {
    {{255, 0, 0}, {200, 0, 0}, {150, 0, 0}, {50, 0, 0}},     // Cubo sinistro (sfumature di rosso)
    {{0, 255, 0}, {0, 200, 0}, {0, 150, 0}, {0, 100, 0}},     // Cubo centrale (sfumature di verde)
    {{0, 0, 255}, {0, 0, 200}, {0, 0, 150}, {0, 0, 100}}      // Cubo destro (sfumature di blu)
    };

  const int fogNear = 500, fogFar = 1000;
  static int isInitialized = 0;
  u_long currentPadState;
  MATRIX transformMatrix;
  int i, j, fogFactor, bgR, bgG, bgB;
  long otz, nclip, p, flg; // Dichiarazione di p e flg

  if (!isInitialized) {
    isInitialized = 1;
  }

  // Stato del controller
  currentPadState = GetPadState();

  // Controllo cubo centrale (Sinistra e Destra)
  if (currentPadState & PAD1_LEFT) {
    cubePositions[0].vz += 50;
  }
  if (currentPadState & PAD1_RIGHT) {
    if (cubePositions[0].vz > cubeInitialZ) {
      cubePositions[0].vz -= 50;
    }
  }

  // Controllo cubo sinistro (Su e Giù)
  if (currentPadState & PAD1_UP) {
    cubePositions[1].vz += 50;
  }
  if (currentPadState & PAD1_DOWN) {
    if (cubePositions[1].vz > cubeInitialZ) {
      cubePositions[1].vz -= 50;
    }
  }

  // Controllo cubo destro (Triangolo e Quadrato)
  if (currentPadState & PAD1_TRIANGLE) {
    cubePositions[2].vz += 50;
  }
  if (currentPadState & PAD1_CIRCLE) {
    if (cubePositions[2].vz > cubeInitialZ) {
      cubePositions[2].vz -= 50;
    }
  }

  // Controllo reset posizione cubi (tasto X)
  if (currentPadState & PAD1_CROSS) {
    for (i = 0; i < 3; i++) {
      cubePositions[i].vz = cubeInitialZ; // Ripristina la posizione Z originale
    }
  }

  // Interpolazione colori background
  fogFactor = 0;
  if (cubePositions[0].vz > fogNear && cubePositions[0].vz < fogFar) {
    fogFactor = (cubePositions[0].vz - fogNear) * ONE / (fogFar - fogNear);
  } else if (cubePositions[0].vz >= fogFar) {
    fogFactor = ONE;
  }

  bgR = (backgroundColor.r * (ONE - fogFactor) + fogColor.r * fogFactor) / ONE;
  bgG = (backgroundColor.g * (ONE - fogFactor) + fogColor.g * fogFactor) / ONE;
  bgB = (backgroundColor.b * (ONE - fogFactor) + fogColor.b * fogFactor) / ONE;

  setRGB0(&screen.draw[currbuff], bgR, bgG, bgB);

  // Disegna ogni cubo
  for (i = 0; i < 3; i++) {
    RotMatrix(&cubeRotations[i], &transformMatrix);
    TransMatrix(&transformMatrix, &cubePositions[i]);
    ScaleMatrix(&transformMatrix, &cubeScales[i]);
    SetRotMatrix(&transformMatrix);
    SetTransMatrix(&transformMatrix);

    for (j = 0; j < 24; j += 4) {
      POLY_G4 *poly = (POLY_G4 *)GetNextPrim();
      setPolyG4(poly);

      // Colori Gouraud per i cubi
      setRGB0(poly, cubeColors[i][0].r, cubeColors[i][0].g, cubeColors[i][0].b);
      setRGB1(poly, cubeColors[i][1].r, cubeColors[i][1].g, cubeColors[i][1].b);
      setRGB2(poly, cubeColors[i][2].r, cubeColors[i][2].g, cubeColors[i][2].b);
      setRGB3(poly, cubeColors[i][3].r, cubeColors[i][3].g, cubeColors[i][3].b);

      nclip = RotAverageNclip4(
          &cube.vertices[cube.faces[j + 0]], &cube.vertices[cube.faces[j + 1]],
          &cube.vertices[cube.faces[j + 2]], &cube.vertices[cube.faces[j + 3]],
          (long *)&poly->x0, (long *)&poly->x1, (long *)&poly->x2,
          (long *)&poly->x3, &p, &otz, &flg);

      if (nclip <= 0) {
        continue;
      }

      // Effetto nebbia
      if (otz > fogNear && otz < fogFar) {
        fogFactor = (otz - fogNear) * ONE / (fogFar - fogNear);
        poly->r0 = (poly->r0 * (ONE - fogFactor) + fogColor.r * fogFactor) / ONE;
        poly->g0 = (poly->g0 * (ONE - fogFactor) + fogColor.g * fogFactor) / ONE;
        poly->b0 = (poly->b0 * (ONE - fogFactor) + fogColor.b * fogFactor) / ONE;
        poly->r1 = (poly->r1 * (ONE - fogFactor) + fogColor.r * fogFactor) / ONE;
        poly->g1 = (poly->g1 * (ONE - fogFactor) + fogColor.g * fogFactor) / ONE;
        poly->b1 = (poly->b1 * (ONE - fogFactor) + fogColor.b * fogFactor) / ONE;
        poly->r2 = (poly->r2 * (ONE - fogFactor) + fogColor.r * fogFactor) / ONE;
        poly->g2 = (poly->g2 * (ONE - fogFactor) + fogColor.g * fogFactor) / ONE;
        poly->b2 = (poly->b2 * (ONE - fogFactor) + fogColor.b * fogFactor) / ONE;
        poly->r3 = (poly->r3 * (ONE - fogFactor) + fogColor.r * fogFactor) / ONE;
        poly->g3 = (poly->g3 * (ONE - fogFactor) + fogColor.g * fogFactor) / ONE;
        poly->b3 = (poly->b3 * (ONE - fogFactor) + fogColor.b * fogFactor) / ONE;
      } else if (otz >= fogFar) {
        poly->r0 = fogColor.r;
        poly->g0 = fogColor.g;
        poly->b0 = fogColor.b;
        poly->r1 = fogColor.r;
        poly->g1 = fogColor.g;
        poly->b1 = fogColor.b;
        poly->r2 = fogColor.r;
        poly->g2 = fogColor.g;
        poly->b2 = fogColor.b;
        poly->r3 = fogColor.r;
        poly->g3 = fogColor.g;
        poly->b3 = fogColor.b;
      }

      if ((otz > 0) && (otz < OT_LEN)) {
        addPrim(GetOTAt(GetCurrBuff(), otz), poly);
        IncrementNextPrim(sizeof(POLY_G4));
      }
    }
  }

  // Mostra informazioni sui cubi
  SetDumpFnt(fontHandle);
  for (i = 0; i < 3; i++) {
    FntPrint("\n\n\t\t\tCubo %d - Z: %d", i + 1, cubePositions[i].vz);
  }
  FntFlush(-1);
}

void RunProgram8(void) {
    int PadStatus;
    int currentFrame = 0;          // Current model index
    const int totalFrames = 5;     // Total number of models
    int frameCounter = 0;          // Counter to control animation speed
    int frameDelay = 24;     // Frames to wait before switching models
    int i;

    MATRIX obj_matrix;

    VECTOR obj_pos = {0};
    SVECTOR obj_rot = {0};

    VECTOR bulb_pos = {0};
    SVECTOR bulb_rot = {0};

    // Player coordinates
    struct {
        int x, xv;
        int y, yv;
        int z, zv;
        int pan, panv;
        int til, tilv;
    } Player = {0};

    // Init everything
    init();

    // Load the texture for our test model
    LoadTexture((u_long *)tim_maritex);

    // Link the TMD models
    ObjectCount += LinkModel((u_long *)tmd_00, &Object[0]);
    ObjectCount += LinkModel((u_long *)tmd_01, &Object[1]);
    ObjectCount += LinkModel((u_long *)tmd_02, &Object[2]);
    ObjectCount += LinkModel((u_long *)tmd_03, &Object[3]);
    ObjectCount += LinkModel((u_long *)tmd_04, &Object[4]);

    ObjectCount += LinkModel((u_long *)tmd_bulb, &Object[5]); // Bulb


    Object[0].attribute |= GsDIV1; // Set 2x2 sub-division for the platform to reduce clipping errors
    Object[1].attribute |= GsDIV1; // Set 2x2 sub-division for the platform to reduce clipping errors
    Object[2].attribute |= GsDIV1; // Set 2x2 sub-division for the platform to reduce clipping errors
    Object[3].attribute |= GsDIV1; // Set 2x2 sub-division for the platform to reduce clipping errors
    Object[4].attribute |= GsDIV1; // Set 2x2 sub-division for the platform to reduce clipping errors

    for (i = 5; i < ObjectCount; i++) {
        Object[i].attribute = 0; // Re-enable lighting for the test model
    }



    // Default camera/player position
    Player.x = ONE * -640;
    Player.y = ONE * 510;
    Player.z = ONE * 800;

    Player.pan = -660;
    Player.til = -245;

    // Object positions
    bulb_pos.vz = -800;
    bulb_pos.vy = -400;
    obj_pos.vy = 400;

    camera.position.vx = -12576;
    camera.position.vy = 8000;
    camera.position.vz = 14780;
    camera.rotation.vy = 467;
    camera.rotation.vx = 363;

    while (1) {
        JoyPadUpdate();

        if (JoyPadCheck(PAD1_SELECT)) {
            appState = STATE_MENU; // Change app state to MENU
            selectedProgram = -1;  // Reset selected program
            FntFlush(-1);
            DrawSync(0);
            InitGeom();
            RunProgram4(); // Fix graphical bug
            return;
        }

       // PadStatus = PadRead(0);

           // Light source/bulb movement
/*
            if (JoyPadCheck(PAD1_UP)){bulb_pos.vy -= 160;}
            if (JoyPadCheck(PAD1_DOWN)){bulb_pos.vy += 160;}
            if (JoyPadCheck(PAD1_RIGHT)){bulb_pos.vz -= 160;}
            if (JoyPadCheck(PAD1_LEFT)){bulb_pos.vz += 160;}
            if (JoyPadCheck(PAD1_TRIANGLE)){bulb_pos.vx -= 160;}
            if (JoyPadCheck(PAD1_CIRCLE)){bulb_pos.vx += 160;}
*/

            if (JoyPadCheck(PAD1_RIGHT)){frameDelay += 1;}
            if (JoyPadCheck(PAD1_LEFT)){frameDelay -= 1;}

            //if (JoyPadCheck(PAD1_DOWN)){frameCounter -= 1;}
            //if (JoyPadCheck(PAD1_UP)){frameCounter += 1;}

        // Increment frame counter for animation control
        frameCounter++;
        if (frameCounter >= frameDelay) {
            frameCounter = 0;                  // Reset the counter
            currentFrame = (currentFrame + 1) % totalFrames; // Switch to next frame
        }

        // Prepare for rendering
        PrepDisplay();

        // Print debug information
        //FntPrint("\n\n\n TMD EXAMPLE BY LAMEGUY64\n 2014 MEIDO-TEK PRODUCTIONS\n -\n");
        FntPrint("\n\n\n TMD FIGHTER ANIMATION EXAMPLE\n -\n");

        FntPrint(" CX:%d CY:%d CZ:%d\n", camera.position.vx, camera.position.vy, camera.position.vz);
        FntPrint(" CP:%d CT:%d CR:%d\n", camera.rotation.vy, camera.rotation.vx, camera.rotation.vz);
        FntPrint(" Current Frame: %d\n", currentFrame);
        FntPrint(" LX:%d LY:%d LZ:%d\n", bulb_pos.vx, bulb_pos.vy, bulb_pos.vz);
        FntPrint(" Frame Delay: %d\n", frameDelay);
        FntPrint(" Frame Counter: %d\n", frameCounter);



        // Calculate camera position and orientation
        CalculateCamera();

        // Imposta la luce
        pslt[0].vx = -(bulb_pos.vx);
        pslt[0].vy = -(bulb_pos.vy);
        pslt[0].vz = -(bulb_pos.vz);

        pslt[0].r = 0x00;
        pslt[0].g = 0x00;
        pslt[0].b = 0x00;
        GsSetFlatLight(0, &pslt[0]);

        // Render light bulb
        PutObject(bulb_pos, bulb_rot, &Object[5]);

        // Rotate the object
        obj_rot.vy += 4;

        // Set the transformation matrix for the object
        GsSetLsMatrix(&obj_matrix);

        // Render the current model
        PutObject(obj_pos, obj_rot, &Object[currentFrame]);

        // Display the frame
        Display();
    }
}
