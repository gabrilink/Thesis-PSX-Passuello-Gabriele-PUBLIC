#include "sound.h"
#include <libcd.h>     // Libreria per la gestione del CD-ROM.
#include <stdlib.h>    // Per funzioni standard come malloc.

#define SOUND_MALLOC_MAX 10  // Numero massimo di allocazioni di memoria per l'SPU.

static SpuCommonAttr spucommonattr;   // Struttura per configurare gli attributi comuni dell'SPU.
static SpuVoiceAttr spuvoiceattr;     // Struttura per configurare gli attributi di un canale vocale SPU.

static u_long vagspuaddr;             // Indirizzo in memoria SPU per il file audio VAG.
static char spumallocrec[SPU_MALLOC_RECSIZ * (SOUND_MALLOC_MAX + 1)]; // Buffer per tracciare le allocazioni SPU.

static CdlLOC loc[100];               // Array per memorizzare la TOC (tabella dei contenuti) del CD.
static int numtoc;                    // Numero di tracce nella TOC del CD.

// Funzione per inizializzare il sistema audio.
void SoundInit(void) {
  SpuInit();  // Inizializza l'SPU.

  SpuInitMalloc(SOUND_MALLOC_MAX, spumallocrec);  // Configura la gestione della memoria SPU.

  // Configura gli attributi comuni dell'SPU.
  spucommonattr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR | SPU_COMMON_CDVOLL | SPU_COMMON_CDVOLR | SPU_COMMON_CDMIX);

  spucommonattr.mvol.left  = 0x3FFF; // Volume master sinistro (massimo).
  spucommonattr.mvol.right = 0x3FFF; // Volume master destro (massimo).

  spucommonattr.cd.volume.left  = 0x7FFF; // Volume CD sinistro (massimo).
  spucommonattr.cd.volume.right = 0x7FFF; // Volume CD destro (massimo).

  spucommonattr.cd.mix = SPU_ON;    // Abilita l'ingresso audio del CD.

  SpuSetCommonAttr(&spucommonattr); // Applica le impostazioni comuni all'SPU.
}

// Funzione per caricare un file audio in formato VAG dal CD.
char *LoadVAGSound(char *filename, u_long *length) {
  u_char *bytes;

  bytes = (u_char*) FileRead(filename, length);  // Legge il file e restituisce un buffer con i dati.

  if (bytes == NULL) {
    printf("Error reading %s from the CD.\n", filename); // Messaggio di errore se il file non è stato trovato.
    return NULL;
  }

  return bytes; // Restituisce il buffer dei dati audio.
}

// Funzione per trasferire i dati VAG alla memoria SPU.
void TransferVAGToSpu(char *data, u_long length, int voicechannel) {
  SpuSetTransferMode(SpuTransByDMA);         // Imposta il trasferimento tramite DMA.
  vagspuaddr = SpuMalloc(length);            // Alloca memoria nella SPU per il file audio.
  SpuSetTransferStartAddr(vagspuaddr);       // Imposta l'indirizzo di partenza del trasferimento.
  SpuWrite(data, length);                    // Esegue il trasferimento dei dati audio.
  SpuIsTransferCompleted(SPU_TRANSFER_WAIT); // Attende che il trasferimento sia completato.

  // Configura gli attributi del canale vocale.
  spuvoiceattr.mask = (
    SPU_VOICE_VOLR |
    SPU_VOICE_PITCH |
    SPU_VOICE_WDSA |
    SPU_VOICE_ADSR_AMODE |
    SPU_VOICE_ADSR_SMODE |
    SPU_VOICE_ADSR_RMODE |
    SPU_VOICE_ADSR_AR |
    SPU_VOICE_ADSR_DR |
    SPU_VOICE_ADSR_SR |
    SPU_VOICE_ADSR_RR |
    SPU_VOICE_ADSR_SL
  );
  spuvoiceattr.voice = voicechannel;          // Imposta il canale vocale.
  spuvoiceattr.volume.left  = 0x1FFF;         // Volume sinistro.
  spuvoiceattr.volume.right = 0x1FFF;         // Volume destro.
  spuvoiceattr.pitch = 0x800;                 // Altezza del suono.
  spuvoiceattr.addr = vagspuaddr;             // Indirizzo di inizio dei dati audio.
  spuvoiceattr.a_mode = SPU_VOICE_LINEARIncN; // Curva di attacco lineare.
  spuvoiceattr.s_mode = SPU_VOICE_LINEARIncN; // Curva di sostegno lineare.
  spuvoiceattr.r_mode = SPU_VOICE_LINEARIncN; // Curva di rilascio lineare.
  spuvoiceattr.ar = 0x00;                     // Velocità di attacco.
  spuvoiceattr.dr = 0x00;                     // Velocità di decadimento.
  spuvoiceattr.sr = 0x00;                     // Velocità di sostegno.
  spuvoiceattr.rr = 0x00;                     // Velocità di rilascio.
  spuvoiceattr.sl = 0x00;                     // Livello di sostegno.

  SpuSetVoiceAttr(&spuvoiceattr);             // Applica gli attributi al canale vocale.
}

// Funzione per avviare la riproduzione di un canale vocale.
void AudioPlay(int voicechannel) {
  SpuSetKey(SpuOn, voicechannel); // Avvia il canale vocale specificato.
}

// Funzione per liberare memoria nella SPU.
void AudioFree(unsigned long address) {
  SpuFree(address); // Libera la memoria all'indirizzo specificato.
}

// Funzione per riprodurre una traccia audio da un CD.
void PlayAudioTrack(u_short tracknum) {
  u_int i;
  u_char param[4];
  u_char result[8];

  SpuSetTransferMode(SpuTransByDMA); // Imposta il trasferimento tramite DMA.

  // Ottiene la TOC (tabella dei contenuti) del CD.
  while ((numtoc = CdGetToc(loc)) == 0) {
    printf("No TOC found: Please use CD-DA disc...\n"); // Messaggio di errore se non si trova la TOC.
  }

  printf("NUMTOC = %d\n", numtoc); // Stampa il numero di tracce trovate.

  // Regola le posizioni delle tracce per evitare errori di lettura.
  for (i = 1; i < numtoc; i++) {
    CdIntToPos(CdPosToInt(&loc[i]) - 74, &loc[i]); // Calcola la posizione esatta della traccia.
    printf("TOC[%d] ---> Track=%d --> Minute=%d --> Second=%d\n", i, loc[i].track, loc[i].minute, loc[i].second);
  }

  // Imposta i parametri del CD per la riproduzione.
  param[0] = CdlModeRept | CdlModeDA; // Modalità di riproduzione continua (loop) e CD-DA.

  CdControlB(CdlSetmode, param, 0); // Applica la modalità di riproduzione.

  VSync(3); // Attende tre VSync per sincronizzarsi.

  CdControlB(CdlPlay, (u_char*) &loc[tracknum], 0); // Avvia la riproduzione della traccia specificata.
}
