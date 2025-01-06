#include "lzss.h"

// Funzione per decomprimere dati compressi con l'algoritmo LZSS.
// - indata: puntatore ai dati compressi in input.
// - outdata: puntatore ai dati decompressi in output.
void ExpandLZSSData(unsigned char *indata, unsigned char *outdata) {
	short i;
	short current_position;       // Posizione corrente nella finestra.
	unsigned char cc;             // Carattere corrente da decomprimere.
	short match_length;           // Lunghezza della sequenza corrispondente.
	short match_position;         // Posizione della sequenza corrispondente nella finestra.
	unsigned long mask;           // Maschera utilizzata per leggere i bit.
	unsigned long return_value;   // Valore temporaneo per costruire dati dai bit.
	unsigned char inBfile_mask;   // Maschera per leggere i bit dai dati compressi.
	short inBfile_rack;           // Buffer temporaneo per leggere i dati compressi.
	short value;                  // Valore utilizzato per determinare il tipo di simbolo (letterale o match).
	unsigned char window[WINDOW_SIZE]; // Finestra circolare utilizzata dall'algoritmo LZSS.

	// Inizializza il buffer di lettura e la maschera.
	inBfile_rack = 0;
	inBfile_mask = 0x80;

	// Inizializza la posizione corrente nella finestra.
	current_position = 1;

	// Ciclo principale di decompressione.
	for (;;) {
		// Se la maschera è a 0x80, leggi un nuovo byte dai dati compressi.
		if (inBfile_mask == 0x80)
	 		inBfile_rack = (short)*indata++;

		// Determina se il valore corrente è un letterale o una sequenza match.
		value = inBfile_rack & inBfile_mask;
		inBfile_mask >>= 1; // Sposta la maschera a destra.
		if (inBfile_mask == 0)
			inBfile_mask = 0x80;

		// Se è un letterale.
		if (value) {
			// Leggi un byte letterale.
			mask = 1L << (8 - 1);
			return_value = 0;
			while (mask != 0) {
				if (inBfile_mask == 0x80)
		 			inBfile_rack = (short)*indata++;

				if (inBfile_rack & inBfile_mask)
					return_value |= mask;
				mask >>= 1;
				inBfile_mask >>= 1;

				if (inBfile_mask == 0)
					inBfile_mask = 0x80;
			}
			cc = (unsigned char) return_value;
			*outdata++ = cc; // Scrive il carattere nei dati decompressi.
			window[current_position] = cc; // Salva nella finestra.
			current_position = MOD_WINDOW(current_position + 1); // Aggiorna la posizione nella finestra.
		}
		// Se è una sequenza match.
		else {
			// Leggi la posizione del match.
			mask = 1L << (INDEX_BIT_COUNT - 1);
			return_value = 0;
			while (mask != 0) {
				if (inBfile_mask == 0x80)
		 			inBfile_rack = (short)*indata++;

				if (inBfile_rack & inBfile_mask)
					return_value |= mask;
				mask >>= 1;
				inBfile_mask >>= 1;

				if (inBfile_mask == 0)
					inBfile_mask = 0x80;
			}
			match_position = (short) return_value;

			// Se si incontra la fine dello stream, interrompi il ciclo.
			if (match_position == END_OF_STREAM)
					break;

			// Leggi la lunghezza del match.
			mask = 1L << (LENGTH_BIT_COUNT - 1);
			return_value = 0;
			while (mask != 0) {
				if (inBfile_mask == 0x80)
		 			inBfile_rack =  (short)*indata++;

				if (inBfile_rack & inBfile_mask)
					return_value |= mask;
				mask >>= 1;
				inBfile_mask >>= 1;

				if (inBfile_mask == 0)
					inBfile_mask = 0x80;
			}
			match_length = (short) return_value;

			// Aggiungi un offset alla lunghezza (BREAK_EVEN).
			match_length += BREAK_EVEN;

			// Copia la sequenza match nei dati decompressi.
			for (i = 0; i <= match_length; i++) {
				cc = window[MOD_WINDOW(match_position + i)];
				*outdata++ = cc;
				window[current_position] = cc;
				current_position = MOD_WINDOW(current_position + 1);
			}
		}
	}
}
