#include "utils.h"     // Include le dichiarazioni delle funzioni di utilità.
#include "globals.h"   // Include variabili e costanti globali.
#include <sys/types.h> // Include tipi di dato standard come u_char, u_long, ecc.
#include <stdio.h>     // Include funzioni di input/output standard.
#include <stdlib.h>    // Include funzioni per la gestione della memoria dinamica.
#include <stddef.h>    // Include definizioni di tipi standard come NULL.
#include <libcd.h>     // Include funzioni per la gestione del CD-ROM.

// Funzione per leggere un file dal CD e restituirne il contenuto in memoria.
// Parametri:
// - filename: nome del file da leggere.
// - length: puntatore a una variabile dove verrà salvata la dimensione del file.
// Restituisce un puntatore al buffer contenente i dati del file, o NULL in caso di errore.
char *FileRead(char *filename, u_long *length) {
    CdlFILE filepos;          // Struttura per memorizzare informazioni sul file nel CD.
    int numsectors;           // Numero di settori da leggere dal file.
    char *buffer;             // Buffer per contenere i dati del file.

    buffer = NULL;            // Inizializza il buffer a NULL.

    // Cerca il file nel CD.
    if (CdSearchFile(&filepos, filename) == NULL) {
        printf("%s file not found in the CD.\n", filename); // Messaggio di errore se il file non viene trovato.
    } else {
        printf("Found %s in the CD.\n", filename);         // Messaggio se il file viene trovato.
        numsectors = (filepos.size + 2047) / 2048;         // Calcola il numero di settori necessari (arrotondando).
        buffer = (char *)malloc(2048 * numsectors);        // Alloca memoria per i dati del file.
        if (!buffer) {
            printf("Error allocating %d sectors!\n", numsectors); // Messaggio di errore se l'allocazione fallisce.
        }
        CdControl(CdlSetloc, (u_char *)&filepos.pos, 0);   // Imposta la posizione di lettura nel CD.
        CdRead(numsectors, (u_long *)buffer, CdlModeSpeed); // Legge i settori dal CD.
        CdReadSync(0, 0);                                  // Attende il completamento della lettura.
    }

    *length = filepos.size; // Salva la dimensione del file nella variabile fornita.

    return buffer; // Restituisce il buffer con i dati del file o NULL in caso di errore.
}

// Funzione per ottenere un carattere (byte singolo) da un array di byte.
// Parametri:
// - bytes: puntatore all'array di byte.
// - b: puntatore all'indice corrente nell'array (incrementato dopo la lettura).
// Restituisce il carattere letto.
char GetChar(u_char *bytes, u_long *b) {
    return bytes[(*b)++]; // Restituisce il byte corrente e incrementa l'indice.
}

// Funzione per ottenere un valore "short" (2 byte) in formato Little-Endian.
// Parametri:
// - bytes: puntatore all'array di byte.
// - b: puntatore all'indice corrente nell'array (incrementato dopo la lettura).
// Restituisce il valore "short" letto.
short GetShortLE(u_char *bytes, u_long *b) {
    u_short value = 0;
    value |= bytes[(*b)++] << 0; // Legge il byte meno significativo.
    value |= bytes[(*b)++] << 8; // Legge il byte più significativo.
    return (short)value;         // Restituisce il valore "short".
}

// Funzione per ottenere un valore "short" (2 byte) in formato Big-Endian.
// Parametri:
// - bytes: puntatore all'array di byte.
// - b: puntatore all'indice corrente nell'array (incrementato dopo la lettura).
// Restituisce il valore "short" letto.
short GetShortBE(u_char *bytes, u_long *b) {
    u_short value = 0;
    value |= bytes[(*b)++] << 8; // Legge il byte più significativo.
    value |= bytes[(*b)++] << 0; // Legge il byte meno significativo.
    return (short)value;         // Restituisce il valore "short".
}

// Funzione per ottenere un valore "long" (4 byte) in formato Little-Endian.
// Parametri:
// - bytes: puntatore all'array di byte.
// - b: puntatore all'indice corrente nell'array (incrementato dopo la lettura).
// Restituisce il valore "long" letto.
long GetLongLE(u_char *bytes, u_long *b) {
    u_long value = 0;
    value |= bytes[(*b)++] << 0;  // Legge il byte meno significativo.
    value |= bytes[(*b)++] << 8;  // Legge il secondo byte.
    value |= bytes[(*b)++] << 16; // Legge il terzo byte.
    value |= bytes[(*b)++] << 24; // Legge il byte più significativo.
    return (long)value;           // Restituisce il valore "long".
}

// Funzione per ottenere un valore "long" (4 byte) in formato Big-Endian.
// Parametri:
// - bytes: puntatore all'array di byte.
// - b: puntatore all'indice corrente nell'array (incrementato dopo la lettura).
// Restituisce il valore "long" letto.
long GetLongBE(u_char *bytes, u_long *b) {
    u_long value = 0;
    value |= bytes[(*b)++] << 24; // Legge il byte più significativo.
    value |= bytes[(*b)++] << 16; // Legge il terzo byte.
    value |= bytes[(*b)++] << 8;  // Legge il secondo byte.
    value |= bytes[(*b)++] << 0;  // Legge il byte meno significativo.
    return (long)value;           // Restituisce il valore "long".
}

// Funzione per limitare un valore a 16 bit (intervallo: -32767 a +32767).
// Parametri:
// - value: valore da limitare.
// Restituisce il valore limitato.
long Clamp16Bits(long value) {
    if (value > +32767) return +32767; // Limita al massimo positivo.
    if (value < -32767) return -32767; // Limita al massimo negativo.
    return value;                      // Restituisce il valore originale se entro i limiti.
}
