#ifndef SOUND_H
#define SOUND_H

#include "globals.h"  // Inclusione del file di intestazione globale per le definizioni comuni.
#include <libspu.h>   // Libreria per la gestione dell'SPU (Sound Processing Unit).

// Funzione per inizializzare il sistema audio.
// Configura l'SPU per l'elaborazione audio.
void SoundInit(void);

// Funzione per caricare un file audio in formato VAG.
// Parametri:
// - filename: nome del file VAG da caricare.
// - length: puntatore a una variabile dove verrà salvata la lunghezza del file in byte.
// Restituisce un puntatore ai dati audio caricati in memoria.
char *LoadVAGSound(char *filename, u_long *length);

// Funzione per trasferire i dati audio in formato VAG alla SPU.
// Parametri:
// - data: puntatore ai dati audio da trasferire.
// - length: lunghezza dei dati in byte.
// - voicechannel: numero del canale vocale SPU su cui trasferire i dati.
void TransferVAGToSpu(char *data, u_long length, int voicechannel);

// Funzione per avviare la riproduzione di una traccia audio del CD.
// Parametri:
// - tracknum: numero della traccia audio da riprodurre.
void PlayAudioTrack(u_short tracknum);

#endif
