#include "track.h"
#include "inline_n.h"
#include "texture.h"
#include <stdlib.h>

void LoadTrackVertices(Track *track, char *filename) {
  u_long i, b, length;    // Variabili per iterazione, posizione corrente e lunghezza dei dati.
  u_char *bytes;          // Puntatore ai dati letti dal file.

  // Legge il file dal CD e memorizza la lunghezza dei dati.
  bytes = (u_char*) FileRead(filename, &length);

  // Verifica se il file è stato letto correttamente.
  if (bytes == NULL) {
    printf("Error reading %s from the CD.\n", filename); // Stampa un errore in caso di lettura fallita.
    return;
  }

  b = 0; // Inizializza il contatore dei byte letti.

  // Calcola il numero di vertici basandosi sulla dimensione dei dati.
  track->numvertices = length / BYTES_PER_VERTEX;

  // Alloca memoria per memorizzare i vertici del tracciato.
  track->vertices = (VECTOR*) malloc(track->numvertices * sizeof(VECTOR));

  // Ciclo per leggere i dati di ciascun vertice.
  for (i = 0; i < track->numvertices; i++) {
    track->vertices[i].vx  = GetLongBE(bytes, &b); // Legge la componente X del vertice.
    track->vertices[i].vy  = GetLongBE(bytes, &b); // Legge la componente Y del vertice.
    track->vertices[i].vz  = GetLongBE(bytes, &b); // Legge la componente Z del vertice.
    track->vertices[i].pad = GetLongBE(bytes, &b); // Legge il padding associato al vertice.
  }

  // Libera la memoria utilizzata per i dati letti dal file.
  free(bytes);
}

void LoadTrackFaces(Track *track, char *filename, u_short texturestart) {
  u_long i, b, length;   // Variabili per iterazione, posizione corrente e lunghezza dei dati.
  u_char *bytes;         // Puntatore ai dati letti dal file.
  Texture *texture;      // Puntatore per gestire le texture associate ai volti.

  // Legge il file dal CD e memorizza la lunghezza dei dati.
  bytes = (u_char*) FileRead(filename, &length);

  // Verifica se il file è stato letto correttamente.
  if (bytes == NULL) {
    printf("Error reading %s from the CD.\n", filename); // Segnala errore in caso di lettura fallita.
    return;
  }

  b = 0; // Inizializza il contatore dei byte letti.

  // Calcola il numero di facce basandosi sulla dimensione dei dati.
  track->numfaces = length / BYTES_PER_FACE;

  // Alloca memoria per memorizzare le facce del tracciato.
  track->faces = (Face*) malloc(track->numfaces * sizeof(Face));

  // Ciclo per leggere i dati di ciascuna faccia.
  for (i = 0; i < track->numfaces; i++) {
    Face *face = &track->faces[i];  // Ottiene un puntatore alla faccia corrente.

    // Legge gli indici dei vertici della faccia.
    face->indices[0] = GetShortBE(bytes, &b);
    face->indices[1] = GetShortBE(bytes, &b);
    face->indices[2] = GetShortBE(bytes, &b);
    face->indices[3] = GetShortBE(bytes, &b);

    // Legge il vettore normale della faccia.
    face->normal.vx = GetShortBE(bytes, &b);
    face->normal.vy = GetShortBE(bytes, &b);
    face->normal.vz = GetShortBE(bytes, &b);

    // Legge il valore della texture associata.
    face->texture = GetChar(bytes, &b);

    // Legge i flag che determinano proprietà speciali della faccia.
    face->flags = GetChar(bytes, &b);

    // Legge i valori del colore della faccia (rosso, verde, blu e codifica colore).
    face->color.r  = GetChar(bytes, &b);
    face->color.g  = GetChar(bytes, &b);
    face->color.b  = GetChar(bytes, &b);
    face->color.cd = GetChar(bytes, &b);

    // Incrementa l'indice della texture con l'offset iniziale.
    face->texture += texturestart;

    // Ottiene la texture associata dalla memoria.
    texture = GetFromTextureStore(face->texture);

    // Imposta i valori di `tpage` e `clut` (coordinate della texture).
    face->tpage = texture->tpage;
    face->clut  = texture->clut;

    // Verifica se la texture deve essere "flippata" (invertita).
    if (face->flags & FACE_FLIP_TEXTURE) {
      // Se il flag è attivo, inverte le coordinate UV della texture.
      face->u0 = texture->u1;
      face->v0 = texture->v1;
      face->u1 = texture->u0;
      face->v1 = texture->v0;
      face->u2 = texture->u3;
      face->v2 = texture->v3;
      face->u3 = texture->u2;
      face->v3 = texture->v2;
    } else {
      // Altrimenti, mantiene le coordinate UV originali.
      face->u0 = texture->u0;
      face->v0 = texture->v0;
      face->u1 = texture->u1;
      face->v1 = texture->v1;
      face->u2 = texture->u2;
      face->v2 = texture->v2;
      face->u3 = texture->u3;
      face->v3 = texture->v3;
    }
  }

  // Libera la memoria utilizzata per i dati letti dal file.
  free(bytes);
}


void LoadTrackSections(Track *track, char *filename) {
  u_long i, j, b, length; // Variabili per iterazione, posizione corrente e lunghezza dei dati.
  u_char *bytes;          // Puntatore ai dati letti dal file.

  // Legge il file dal CD e memorizza la lunghezza dei dati.
  bytes = (u_char*) FileRead(filename, &length);

  // Verifica se il file è stato letto correttamente.
  if (bytes == NULL) {
    printf("Error reading %s from the CD.\n", filename); // Messaggio di errore in caso di fallimento della lettura.
    return;
  }

  b = 0; // Inizializza il contatore dei byte letti.

  // Calcola il numero di sezioni basandosi sulla dimensione dei dati.
  track->numsections = length / BYTES_PER_SECTION;

  // Alloca memoria per memorizzare le sezioni del tracciato.
  track->sections = (Section*) malloc(track->numsections * sizeof(Section));

  // Ciclo per leggere i dati di ciascuna sezione.
  for (i = 0; i < track->numsections; i++) {
    b += 4; // Salta i primi 4 byte (padding o dati inutilizzati).

    // Imposta i puntatori alle sezioni precedente e successiva.
    track->sections[i].prev = track->sections + GetLongBE(bytes, &b);
    track->sections[i].next = track->sections + GetLongBE(bytes, &b);

    // Legge il centro della sezione (coordinate vx, vy, vz).
    track->sections[i].center.vx = GetLongBE(bytes, &b);
    track->sections[i].center.vy = GetLongBE(bytes, &b);
    track->sections[i].center.vz = GetLongBE(bytes, &b);

    b += 118; // Salta dati inutilizzati (esempio: padding o informazioni non usate).

    // Legge l'indice della faccia di partenza e il numero di facce nella sezione.
    track->sections[i].facestart = GetShortBE(bytes, &b);
    track->sections[i].numfaces  = GetShortBE(bytes, &b);

    b += 4; // Salta ulteriori byte inutilizzati.

    // Legge i flag e l'ID della sezione.
    track->sections[i].flags = GetShortBE(bytes, &b);
    track->sections[i].id = GetShortBE(bytes, &b);
    track->sections[i].id = i; // Imposta l'indice come ID della sezione.

    b += 2; // Salta eventuali dati inutilizzati.

    // Calcola il vettore normale della sezione, basandosi sulle facce associate.
    for (j = 0; j < track->sections[i].numfaces; j++) {
      Face *face = track->faces + track->sections[i].facestart + j;
      if (face->flags & FACE_TRACK_BASE) {
        // Se la faccia è "base", utilizza il normale della faccia come normale della sezione.
        track->sections[i].normal = face->normal;

        // Imposta le coordinate del vertice base della faccia.
        track->sections[i].basevertex.vx = track->vertices[face->indices[0]].vx;
        track->sections[i].basevertex.vy = track->vertices[face->indices[0]].vy;
        track->sections[i].basevertex.vz = track->vertices[face->indices[0]].vz;
        continue; // Termina il ciclo per questa faccia.
      }
    }
  }

  // Libera la memoria utilizzata per i dati grezzi letti dal file.
  free(bytes);
}

void RenderQuadRecursive(Face* face, SVECTOR *v0, SVECTOR *v1, SVECTOR *v2, SVECTOR *v3, u_short tu0, u_short tv0, u_short tu1, u_short tv1, u_short tu2, u_short tv2, u_short tu3, u_short tv3, u_short level, u_short depth) {
  // Se il livello corrente ha raggiunto la profondità desiderata, si procede al rendering del quadrilatero.
  if (level >= depth) {
    short nclip; // Variabile per il clipping.
    long otz;    // Profondità del poligono nella Z.
    LINE_F2 *line0, *line1, *line2, *line3;
    POLY_FT4 *poly; // Struttura per definire un poligono.

    poly = (POLY_FT4*) GetNextPrim(); // Ottiene il prossimo spazio per un primitivo.
    gte_ldv0(v0);
    gte_ldv1(v1);
    gte_ldv2(v2);
    gte_rtpt(); // Trasforma i vertici nello spazio dello schermo.
    gte_nclip(); // Controlla se il poligono è fuori dallo schermo.
    gte_stopz(&nclip);
    if (nclip < 0) {
      return; // Esce se il poligono è fuori dallo schermo.
    }
    gte_stsxy0(&poly->x0); // Memorizza le coordinate X e Y del primo vertice.
    gte_ldv0(v3);
    gte_rtps();
    gte_stsxy3(&poly->x1, &poly->x2, &poly->x3); // Memorizza le coordinate degli altri vertici.
    gte_avsz4();
    gte_stotz(&otz);
    if (otz > 0 && otz < OT_LEN) { // Controlla se il poligono è visibile.
      setPolyFT4(poly); // Configura il poligono come FT4 (testurizzato).
      setRGB0(poly, face->color.r, face->color.g, face->color.b); // Imposta il colore.
      poly->tpage = face->tpage; // Imposta la pagina della texture.
      poly->clut  = face->clut;  // Imposta il clut (color lookup table).
      setUV4(poly, tu0, tv0, tu1, tv1, tu2, tv2, tu3, tv3); // Imposta le coordinate UV.
      addPrim(GetOTAt(GetCurrBuff(), otz), poly); // Aggiunge il poligono alla lista di rendering.
      IncrementNextPrim(sizeof(POLY_FT4)); // Incrementa il puntatore al prossimo primitivo.

      // Sezione di debug per disegnare linee attorno ai bordi del poligono (commentata).
#if 0
      line0 = (LINE_F2*) GetNextPrim();
      setLineF2(line0);
      setXY2(line0, poly->x0, poly->y0, poly->x1, poly->y1);
      setRGB0(line0, 255, 255, 0);
      addPrim(GetOTAt(GetCurrBuff(), 0), line0);
      IncrementNextPrim(sizeof(LINE_F2));

      line1 = (LINE_F2*) GetNextPrim();
      setLineF2(line1);
      setXY2(line1, poly->x1, poly->y1, poly->x3, poly->y3);
      setRGB0(line1, 255, 255, 0);
      addPrim(GetOTAt(GetCurrBuff(), 0), line1);
      IncrementNextPrim(sizeof(LINE_F2));

      line2 = (LINE_F2*) GetNextPrim();
      setLineF2(line2);
      setXY2(line2, poly->x3, poly->y3, poly->x2, poly->y2);
      setRGB0(line2, 255, 255, 0);
      addPrim(GetOTAt(GetCurrBuff(), 0), line2);
      IncrementNextPrim(sizeof(LINE_F2));

      line3 = (LINE_F2*) GetNextPrim();
      setLineF2(line3);
      setXY2(line3, poly->x2, poly->y2, poly->x0, poly->y0);
      setRGB0(line3, 255, 255, 0);
      addPrim(GetOTAt(GetCurrBuff(), 0), line3);
      IncrementNextPrim(sizeof(LINE_F2));
#endif
    }
  } else {
    // Se il livello corrente è inferiore alla profondità desiderata, suddivide il quadrilatero in sub-quadrilateri.
    SVECTOR vm01, vm02, vm03, vm12, vm32, vm13;
    u_short tum01, tvm01, tum02, tvm02, tum03, tvm03, tum12, tvm12, tum13, tvm13, tum32, tvm32;

    // Calcola i punti medi tra i vertici per creare i sub-quadrilateri.
    vm01 = (SVECTOR){(v0->vx + v1->vx) >> 1, (v0->vy + v1->vy) >> 1, (v0->vz + v1->vz) >> 1};
    vm02 = (SVECTOR){(v0->vx + v2->vx) >> 1, (v0->vy + v2->vy) >> 1, (v0->vz + v2->vz) >> 1};
    vm03 = (SVECTOR){(v0->vx + v3->vx) >> 1, (v0->vy + v3->vy) >> 1, (v0->vz + v3->vz) >> 1};
    vm12 = (SVECTOR){(v1->vx + v2->vx) >> 1, (v1->vy + v2->vy) >> 1, (v1->vz + v2->vz) >> 1};
    vm13 = (SVECTOR){(v1->vx + v3->vx) >> 1, (v1->vy + v3->vy) >> 1, (v1->vz + v3->vz) >> 1};
    vm32 = (SVECTOR){(v3->vx + v2->vx) >> 1, (v3->vy + v2->vy) >> 1, (v3->vz + v2->vz) >> 1};

    // Calcola i punti medi delle coordinate UV.
    tum01 = (tu0 + tu1) >> 1; tvm01 = (tv0 + tv1) >> 1;
    tum02 = (tu0 + tu2) >> 1; tvm02 = (tv0 + tv2) >> 1;
    tum03 = (tu0 + tu3) >> 1; tvm03 = (tv0 + tv3) >> 1;
    tum12 = (tu1 + tu2) >> 1; tvm12 = (tv1 + tv2) >> 1;
    tum13 = (tu1 + tu3) >> 1; tvm13 = (tv1 + tv3) >> 1;
    tum32 = (tu3 + tu2) >> 1; tvm32 = (tv3 + tv2) >> 1;

    // Ricorsivamente suddivide il quadrilatero in quattro sub-quadrilateri e li renderizza.
    RenderQuadRecursive(face,    v0, &vm01, &vm02, &vm03,   tu0,   tv0, tum01, tvm01, tum02, tvm02, tum03, tvm03, level + 1, depth);  // Sub-quadrilatero in alto a sinistra.
    RenderQuadRecursive(face, &vm01,    v1, &vm03, &vm13, tum01, tvm01,   tu1,   tv1, tum03, tvm03, tum13, tvm13, level + 1, depth);  // Sub-quadrilatero in alto a destra.
    RenderQuadRecursive(face, &vm02, &vm03,    v2, &vm32, tum02, tvm02, tum03, tvm03,   tu2,   tv2, tum32, tvm32, level + 1, depth);  // Sub-quadrilatero in basso a sinistra.
    RenderQuadRecursive(face, &vm03, &vm13, &vm32,    v3, tum03, tvm03, tum13, tvm13, tum32, tvm32,   tu3,   tv3, level + 1, depth);  // Sub-quadrilatero in basso a destra.
  }
}


void DrawSectionNormal(Section *section, Camera *camera) {
  // Definizione di variabili per i vettori, profondità Z, poligoni e linea.
  SVECTOR v0, v1, v2;
  int i;
  long otz; // Valore della profondità Z.
  POLY_F3 *polya, *polyb, *polyc; // Poligoni per il rendering.
  LINE_F2 line; // Linea per rappresentare la normale della sezione.

  // Creazione e configurazione del primo poligono per il centro della sezione.
  polya = (POLY_F3*) GetNextPrim();
  SetPolyF3(polya);

  // Calcolo del primo vertice relativo alla posizione della telecamera.
  v0.vx = (short) (section->center.vx - camera->position.vx);
  v0.vy = (short) (section->center.vy - camera->position.vy);
  v0.vz = (short) (section->center.vz - camera->position.vz);
  v1 = v2 = v0; // Gli altri vertici coincidono con il primo.

  // Trasformazione e proiezione del poligono nello spazio dello schermo.
  otz  = RotTransPers(&v0, (long*)&polya->x0, NULL, NULL);
  otz += RotTransPers(&v1, (long*)&polya->x1, NULL, NULL);
  otz += RotTransPers(&v2, (long*)&polya->x2, NULL, NULL);
  otz /= 3; // Media dei valori di profondità.

  setRGB0(polya, 0, 0, 0); // Colore nero per il primo poligono.
  addPrim(GetOTAt(GetCurrBuff(), otz), polya); // Aggiunta del poligono alla lista di rendering.
  IncrementNextPrim(sizeof(POLY_FT3)); // Incremento del puntatore al prossimo primitivo.

  // Creazione e configurazione del secondo poligono per il punto terminale della normale.
  polyb = (POLY_F3*) GetNextPrim();
  SetPolyF3(polyb);

  // Calcolo del secondo vertice utilizzando il vettore normale della sezione.
  v0.vx = (short) Clamp16Bits((section->center.vx - camera->position.vx)) + (section->normal.vx >> 3);
  v0.vy = (short) Clamp16Bits((section->center.vy - camera->position.vy)) + (section->normal.vy >> 3);
  v0.vz = (short) Clamp16Bits((section->center.vz - camera->position.vz)) + (section->normal.vz >> 3);
  v1 = v2 = v0; // Gli altri vertici coincidono con il primo.

  // Trasformazione e proiezione del poligono nello spazio dello schermo.
  otz  = RotTransPers(&v0, (long*)&polyb->x0, NULL, NULL);
  otz += RotTransPers(&v1, (long*)&polyb->x1, NULL, NULL);
  otz += RotTransPers(&v2, (long*)&polyb->x2, NULL, NULL);
  otz /= 3; // Media dei valori di profondità.

  setRGB0(polyb, 0, 0, 0); // Colore nero per il secondo poligono.
  addPrim(GetOTAt(GetCurrBuff(), otz), polyb); // Aggiunta del poligono alla lista di rendering.
  IncrementNextPrim(sizeof(POLY_FT3)); // Incremento del puntatore al prossimo primitivo.

  // Disegna una linea magenta per rappresentare la normale della sezione.
  SetLineF2(&line); // Configura la linea.
  setXY2(&line, polya->x0, polya->y0, polyb->x0, polyb->y0); // Imposta i vertici della linea.
  setRGB0(&line, 255, 0, 255); // Colore magenta.
  DrawPrim(&line); // Disegna la linea.
}


void RenderTrackSection(Track *track, Section *section, Camera *camera, u_short numsubdivs) {
  int i, depth;

  MATRIX worldmat, viewmat; // Matrici per la trasformazione del mondo e della vista.

  VECTOR pos, scale;        // Vettori per posizione e scala.
  SVECTOR rot;              // Vettore per la rotazione.

  SVECTOR v0, v1, v2, v3;   // Vettori per i vertici del quadrilatero.

  // Impostazione iniziale di posizione, rotazione e scala.
  setVector(&pos, 0, 0, 0);
  setVector(&rot, 0, 0, 0);
  setVector(&scale, ONE, ONE, ONE);

  // Inizializzazione della matrice del mondo.
  worldmat = (MATRIX){0};
  RotMatrix(&rot, &worldmat);          // Applicazione della rotazione alla matrice del mondo.
  TransMatrix(&worldmat, &pos);        // Traslazione nella matrice del mondo.
  ScaleMatrix(&worldmat, &scale);      // Scalatura nella matrice del mondo.

  // Combinazione della matrice del mondo con quella della telecamera.
  CompMatrixLV(&camera->rotmat, &worldmat, &viewmat);

  // Imposta la matrice di rotazione e traslazione per il rendering.
  SetRotMatrix(&viewmat);
  SetTransMatrix(&viewmat);

  // Ciclo per ogni faccia della sezione.
  for (i = 0; i < section->numfaces; i++) {
    Face *face = track->faces + section->facestart + i;

    // Calcolo delle coordinate dei vertici relativi alla posizione della telecamera.
    v0.vx = (short) Clamp16Bits(track->vertices[face->indices[1]].vx - camera->position.vx);
    v0.vy = (short) Clamp16Bits(track->vertices[face->indices[1]].vy - camera->position.vy);
    v0.vz = (short) Clamp16Bits(track->vertices[face->indices[1]].vz - camera->position.vz);

    v1.vx = (short) Clamp16Bits(track->vertices[face->indices[0]].vx - camera->position.vx);
    v1.vy = (short) Clamp16Bits(track->vertices[face->indices[0]].vy - camera->position.vy);
    v1.vz = (short) Clamp16Bits(track->vertices[face->indices[0]].vz - camera->position.vz);

    v2.vx = (short) Clamp16Bits(track->vertices[face->indices[2]].vx - camera->position.vx);
    v2.vy = (short) Clamp16Bits(track->vertices[face->indices[2]].vy - camera->position.vy);
    v2.vz = (short) Clamp16Bits(track->vertices[face->indices[2]].vz - camera->position.vz);

    v3.vx = (short) Clamp16Bits(track->vertices[face->indices[3]].vx - camera->position.vx);
    v3.vy = (short) Clamp16Bits(track->vertices[face->indices[3]].vy - camera->position.vy);
    v3.vz = (short) Clamp16Bits(track->vertices[face->indices[3]].vz - camera->position.vz);

    depth = numsubdivs; // Imposta il livello di suddivisione desiderato.

    // Esegui il rendering ricorsivo del quadrilatero.
    RenderQuadRecursive(face, &v0, &v1, &v2, &v3,
      face->u0, face->v0, face->u1, face->v1,
      face->u2, face->v2, face->u3, face->v3,
      0, depth);
  }

#if 0
  // Debug opzionale: Disegna la normale della sezione.
  DrawSectionNormal(section, camera);
#endif
}


void RenderTrackAhead(Track *track, Section *startsection, Camera *camera) {
  u_short i, numsubdivs;       // Contatori e livello di suddivisione per il rendering.
  Section *currsection;        // Puntatore alla sezione attuale.

  // Ciclo per renderizzare le sezioni davanti alla nave.
  currsection = startsection;  // Inizia dalla sezione di partenza.
  for (i = 0; i < 20; i++) {   // Itera fino a 20 sezioni in avanti.
    if (i < 5) numsubdivs = 1; // Per le prime 5 sezioni, usa una suddivisione bassa.
    if (i < 2) numsubdivs = 2; // Per le prime 2 sezioni, aumenta la suddivisione per maggiore dettaglio.
    RenderTrackSection(track, currsection, camera, numsubdivs); // Renderizza la sezione attuale.
    currsection = currsection->next; // Passa alla prossima sezione.
  }

  // Ciclo per renderizzare le sezioni dietro la nave.
  currsection = startsection->prev; // Inizia dalla sezione precedente rispetto a quella di partenza.
  for (i = 0; i < 3; i++) {         // Itera fino a 3 sezioni indietro.
    if (i < 3) numsubdivs = 1;      // Per tutte e 3 le sezioni, usa una suddivisione bassa.
    if (i < 1) numsubdivs = 2;      // Per la prima sezione, aumenta la suddivisione per maggiore dettaglio.
    RenderTrackSection(track, currsection, camera, numsubdivs); // Renderizza la sezione attuale.
    currsection = currsection->prev; // Passa alla sezione precedente.
  }
}
