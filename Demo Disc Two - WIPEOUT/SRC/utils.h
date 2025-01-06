#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h> // Include tipi di dato standard come u_char, u_long, ecc.

// Funzione per leggere un file e restituire un puntatore al contenuto.
// Parametri:
// - filename: Nome del file da leggere.
// - length: Puntatore a una variabile dove verrà salvata la lunghezza del file.
// Restituisce un puntatore ai dati letti o NULL in caso di errore.
char *FileRead(char *filename, u_long *length);

// Funzione per ottenere un carattere (byte singolo) da un array di byte.
// Parametri:
// - bytes: Puntatore all'array di byte.
// - b: Puntatore all'indice corrente dell'array (aggiornato dopo la lettura).
// Restituisce il carattere letto.
char GetChar(u_char *bytes, u_long *b);

// Funzione per ottenere un valore short (2 byte) in formato Big-Endian.
// Parametri:
// - bytes: Puntatore all'array di byte.
// - b: Puntatore all'indice corrente dell'array (aggiornato dopo la lettura).
// Restituisce il valore short letto.
short GetShortBE(u_char *bytes, u_long *b);

// Funzione per ottenere un valore short (2 byte) in formato Little-Endian.
// Parametri:
// - bytes: Puntatore all'array di byte.
// - b: Puntatore all'indice corrente dell'array (aggiornato dopo la lettura).
// Restituisce il valore short letto.
short GetShortLE(u_char *bytes, u_long *b);

// Funzione per ottenere un valore long (4 byte) in formato Big-Endian.
// Parametri:
// - bytes: Puntatore all'array di byte.
// - b: Puntatore all'indice corrente dell'array (aggiornato dopo la lettura).
// Restituisce il valore long letto.
long GetLongBE(u_char *bytes, u_long *b);

// Funzione per ottenere un valore long (4 byte) in formato Little-Endian.
// Parametri:
// - bytes: Puntatore all'array di byte.
// - b: Puntatore all'indice corrente dell'array (aggiornato dopo la lettura).
// Restituisce il valore long letto.
long GetLongLE(u_char *bytes, u_long *b);

// Funzione per limitare un valore intero a 16 bit (tra -32768 e 32767).
// Parametri:
// - value: Valore da limitare.
// Restituisce il valore limitato ai 16 bit.
long Clamp16Bits(long value);

#endif
