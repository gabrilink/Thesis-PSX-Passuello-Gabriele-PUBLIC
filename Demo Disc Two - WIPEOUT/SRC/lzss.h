#ifndef LZSS_H
#define LZSS_H

// Numero di bit utilizzati per rappresentare la posizione di un match nella finestra.
#define INDEX_BIT_COUNT  13

// Numero di bit utilizzati per rappresentare la lunghezza di un match.
#define LENGTH_BIT_COUNT  4

// Dimensione della finestra circolare utilizzata per la decompressione LZSS.
// È una potenza di 2 determinata da INDEX_BIT_COUNT.
#define WINDOW_SIZE  (1 << INDEX_BIT_COUNT)

// Valore "break even": il numero minimo di bit risparmiati per un match rispetto a un simbolo letterale.
// Serve a determinare la soglia di compressione efficace.
#define BREAK_EVEN  ((1 + INDEX_BIT_COUNT + LENGTH_BIT_COUNT) / 9)

// Valore che indica la fine dello stream di dati compressi.
#define END_OF_STREAM  0

// Macro per calcolare la posizione "modulo" nella finestra circolare.
// Assicura che gli indici siano sempre validi all'interno dei limiti della finestra.
#define MOD_WINDOW(a)  ((a) & (WINDOW_SIZE - 1))

// Dichiarazione della funzione per decomprimere i dati compressi con l'algoritmo LZSS.
// - indata: puntatore ai dati compressi in input.
// - outdata: puntatore ai dati decompressi in output.
void ExpandLZSSData(unsigned char *indata, unsigned char *outdata);

#endif
