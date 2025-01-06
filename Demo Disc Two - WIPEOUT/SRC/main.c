#include <stdlib.h>
#include <sys/types.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include "inline_n.h"
#include "globals.h"
#include "display.h"
#include "joypad.h"
#include "camera.h"
#include "object.h"
#include "track.h"
#include "ship.h"
#include "texture.h"
#include "sound.h"
#include "utils.h"

// Variabili globali
Object *ships;        // Lista degli oggetti delle navi.
Object *sceneobjs;    // Lista degli oggetti della scena.
Ship ship;            // Oggetto rappresentante la nave del giocatore.
Track track;          // Oggetto che rappresenta il tracciato.
Camera camera;        // Oggetto per la gestione della telecamera.

char *sfx3, *sfx2, *sfx1, *sfxgo;  // Puntatori agli effetti sonori del conto alla rovescia.
u_long sfx3length, sfx2length, sfx1length, sfxgolength; // Lunghezze degli effetti sonori.

u_long ticks;  // Conta i frame trascorsi.
int isracing;  // Stato della gara: 0 se non iniziata, 1 se in corso.

// Funzione di configurazione iniziale.
void Setup(void) {
  VECTOR startpos;            // Posizione iniziale della nave.
  u_short shipstarttexture;   // Texture iniziali per le navi.
  u_short scenestarttexture;  // Texture iniziali per gli oggetti della scena.
  u_short trackstarttexture;  // Texture iniziali per il tracciato.

  ticks = 0;      // Inizializza il contatore dei frame.
  isracing = 0;   // La gara non è ancora iniziata.

  // Inizializzazione di componenti principali.
  SoundInit();     // Inizializza il sistema audio.
  ScreenInit();    // Configura lo schermo e i buffer.
  CdInit();        // Inizializza il CD-ROM.
  JoyPadInit();    // Inizializza il joypad.

  // Resetta il buffer dei primitivi.
  ResetNextPrim(GetCurrBuff());

  // Carica le texture per le navi.
  shipstarttexture = GetTextureCount();
  LoadTextureCMP("\\ALLSH.CMP;1", NULL);

  // Carica le texture per gli oggetti della scena.
  scenestarttexture = GetTextureCount();
  LoadTextureCMP("\\TRACK02\\SCENE.CMP;1", NULL);

  // Carica i modelli delle navi e degli oggetti della scena.
  ships = LoadObjectPRM("\\ALLSH.PRM;1", shipstarttexture);
  sceneobjs = LoadObjectPRM("\\TRACK02\\SCENE.PRM;1", scenestarttexture);

  // Carica texture e informazioni del tracciato.
  trackstarttexture = GetTextureCount();
  LoadTextureCMP("\\TRACK02\\LIBRARY.CMP;1", "\\TRACK02\\LIBRARY.TTF;1");

  // Carica i vertici, facce e sezioni del tracciato.
  LoadTrackVertices(&track, "\\TRACK02\\TRACK.TRV;1");
  LoadTrackFaces(&track, "\\TRACK02\\TRACK.TRF;1", trackstarttexture);
  LoadTrackSections(&track, "\\TRACK02\\TRACK.TRS;1");

  // Configura la nave del giocatore.
  ship.object = GetObjectByIndex(ships, 3);
  setVector(&startpos, 32599, -347, -45310); // Posizione iniziale della nave.
  ShipInit(&ship, &track, &startpos);

  // Configura la telecamera.
  setVector(&camera.position, ship.object->position.vx, ship.object->position.vy - 700, ship.object->position.vz - 1400);
  camera.lookat = (MATRIX){0};
  camera.rotmat = (MATRIX){0};

  // Carica gli effetti sonori del conto alla rovescia.
  sfx3 = LoadVAGSound("\\COUNT3.VAG;1", &sfx3length);
  sfx2 = LoadVAGSound("\\COUNT2.VAG;1", &sfx2length);
  sfx1 = LoadVAGSound("\\COUNT1.VAG;1", &sfx1length);
  sfxgo = LoadVAGSound("\\COUNTGO.VAG;1", &sfxgolength);
  TransferVAGToSpu(sfx3, sfx3length, SPU_0CH);
  TransferVAGToSpu(sfx2, sfx2length, SPU_1CH);
  TransferVAGToSpu(sfx1, sfx1length, SPU_2CH);
  TransferVAGToSpu(sfxgo, sfxgolength, SPU_3CH);
}

// Funzione di aggiornamento per gestire input e logica di gioco.
void Update(void) {
  u_short i;
  VECTOR up = (VECTOR){0, -ONE, 0}; // Vettore "up" per la telecamera.

  EmptyOT(GetCurrBuff()); // Svuota la ordering table corrente.
  JoyPadUpdate();         // Aggiorna lo stato del joypad.

  // Gestione del conto alla rovescia prima della gara.
  if (!isracing) {
    switch (ticks) {
      case  0: AudioPlay(SPU_0CH); break;
      case 30: AudioPlay(SPU_1CH); break;
      case 60: AudioPlay(SPU_2CH); break;
      case 90:
        AudioPlay(SPU_3CH);
        isracing = 1;
        PlayAudioTrack(2);
        break;
    }
  }

  ticks++; // Incrementa il contatore dei frame.

  // Controlli del joypad per la nave.
  // Yaw (rotazione orizzontale).
  if (JoyPadCheck(PAD1_LEFT)) {
    ship.velyaw = (ship.velyaw <= 0) ? ship.velyaw - 128 : ship.velyaw - 256;
  } else if (JoyPadCheck(PAD1_RIGHT)) {
    ship.velyaw = (ship.velyaw >= 0) ? ship.velyaw + 128 : ship.velyaw + 256;
  } else {
    ship.velyaw += (ship.velyaw > 0) ? -128 : 128;
  }
  ship.velyaw = (ship.velyaw < -2048) ? -2048 : (ship.velyaw > 2048) ? 2048 : ship.velyaw;

  // Pitch (rotazione verticale).
  if (JoyPadCheck(PAD1_UP)) ship.pitch -= 10;
  if (JoyPadCheck(PAD1_DOWN)) ship.pitch += 10;

  // Thrust (spinta).
  if (JoyPadCheck(PAD1_CROSS)) {
    ship.thrustmag += 3000;
  } else if (ship.thrustmag > 0) {
    ship.thrustmag -= 15000;
  }
  ship.thrustmag = (ship.thrustmag > ship.thrustmax) ? ship.thrustmax : ship.thrustmag;

  // Aggiorna la nave e la telecamera.
  ShipUpdate(&ship);
  if (!isracing) ship.vel = (VECTOR){0, 0, 0};

  camera.position.vx = ship.object->position.vx - (ship.forward.vx >> 2) + (up.vx >> 3);
  camera.position.vy = ship.object->position.vy - (ship.forward.vy >> 2) + (up.vy >> 3);
  camera.position.vz = ship.object->position.vz - (ship.forward.vz >> 2) + (up.vz >> 3);
  LookAt(&camera, &camera.position, &ship.object->position, &up);

  // Rendering della scena.
  RenderSceneObjects(sceneobjs, &camera);
  RenderTrackAhead(&track, ship.section, &camera);
  RenderObject(ship.object, &camera);
}

// Funzione per il rendering del frame.
void Render(void) {
  DisplayFrame();
}

// Funzione principale.
int main(void) {
  Setup(); // Configurazione iniziale.
  while (1) {
    Update(); // Aggiorna lo stato di gioco.
    Render(); // Disegna il frame.
  }
  return 0;
}
