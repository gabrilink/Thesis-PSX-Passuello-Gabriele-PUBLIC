#include "ship.h"
#include <stdlib.h>

void ShipInit(Ship *ship, Track *track, VECTOR *startpos) {
    Section *curr;         // Puntatore alla sezione corrente del tracciato
    VECTOR d;              // Vettore di distanza tra la nave e la sezione
    long distmag;          // Magnitudine della distanza
    long mindist;          // Distanza minima trovata

    // Imposta la posizione iniziale della nave
    ship->object->position.vx = startpos->vx;
    ship->object->position.vy = startpos->vy;
    ship->object->position.vz = startpos->vz;

    // Inizializza velocità e accelerazione della nave
    ship->vel = (VECTOR) {0, 0, 0};
    ship->acc = (VECTOR) {0, 0, 0};

    // Inizializza vettori di spinta e attrito
    ship->thrust = (VECTOR) {0, 0, 0};
    ship->drag   = (VECTOR) {0, 0, 0};

    // Inizializza angoli di rotazione
    ship->yaw   = 0;
    ship->pitch = 0;
    ship->roll  = 0;

    // Inizializza velocità angolari
    ship->velyaw   = 0;
    ship->velpitch = 0;
    ship->velroll  = 0;

    // Inizializza accelerazioni angolari
    ship->accyaw   = 0;
    ship->accpitch = 0;
    ship->accroll  = 0;

    // Inizializza altri parametri della nave
    ship->speed = 0;          // Velocità attuale
    ship->thrustmag = 0;      // Magnitudine della spinta attuale
    ship->thrustmax = 15000;  // Spinta massima disponibile
    ship->mass = 150;         // Massa della nave

    // Trova la sezione del tracciato più vicina alla nave
    curr = track->sections;  // Inizia dalla prima sezione del tracciato
    mindist = 99999999;      // Imposta una distanza minima iniziale molto alta
    do {
        // Calcola il vettore distanza tra il centro della sezione e la posizione iniziale
        d.vx = (curr->center.vx - startpos->vx);
        d.vy = (curr->center.vy - startpos->vy);
        d.vz = (curr->center.vz - startpos->vz);
        // Calcola la magnitudine della distanza
        distmag = SquareRoot12(d.vx * d.vx + d.vy * d.vy + d.vz * d.vz);
        // Aggiorna la distanza minima e assegna la sezione più vicina
        if (distmag < mindist) {
            mindist = distmag;
            ship->section = curr;
        }
        curr = curr->next; // Passa alla prossima sezione
    } while (curr != track->sections); // Continua finché non ritorna alla prima sezione
}


void UpdateShipNearestSection(Ship *ship) {
    u_short i;         // Contatore per il ciclo
    Section *curr;     // Puntatore alla sezione corrente
    VECTOR d;          // Vettore per calcolare la distanza
    long distmag;      // Magnitudine della distanza
    long mindist;      // Distanza minima trovata

    // Imposta il punto di partenza della ricerca alla sezione attualmente associata alla nave
    curr = ship->section;

    // Inizializza la distanza minima a un valore molto grande
    mindist = 99999999;

    // Controlla le quattro sezioni più vicine alla nave
    for (i = 0; i < 4; i++) {
        // Calcola il vettore distanza tra la nave e il centro della sezione corrente
        d.vx = (curr->center.vx - ship->object->position.vx);
        d.vy = (curr->center.vy - ship->object->position.vy);
        d.vz = (curr->center.vz - ship->object->position.vz);

        // Calcola la magnitudine della distanza utilizzando SquareRoot12
        distmag = SquareRoot12(d.vx * d.vx + d.vy * d.vy + d.vz * d.vz);

        // Se la distanza corrente è minore della minima trovata finora, aggiorna i dati
        if (distmag < mindist) {
            mindist = distmag; // Aggiorna la distanza minima
            ship->section = curr; // Aggiorna la sezione associata alla nave
        }

        // Passa alla sezione successiva
        curr = curr->next;
    }
}


void ShipUpdate(Ship *ship) {
    VECTOR force; // Forza totale che agisce sulla nave
    VECTOR nosevel; // Velocità lungo l'asse frontale della nave

    VECTOR base2ship; // Vettore dalla base del tracciato alla nave
    long dot, height; // Prodotto scalare e altezza dalla base del tracciato

    short sinx, cosx; // Seno e coseno per il pitch (rotazione sull'asse X)
    short siny, cosy; // Seno e coseno per lo yaw (rotazione sull'asse Y)
    short sinz, cosz; // Seno e coseno per il roll (rotazione sull'asse Z)

    // Calcolo delle funzioni trigonometriche basate sui valori angolari della nave
    sinx = rsin(ship->pitch);
    cosx = rcos(ship->pitch);
    siny = rsin(ship->yaw);
    cosy = rcos(ship->yaw);
    sinz = rsin(ship->roll);
    cosz = rcos(ship->roll);

    // Calcolo dei vettori destra, sopra e avanti della nave basandosi sulla sua orientazione
    ship->right.vx = ((cosy * cosz) >> 12) + ((((siny * sinx) >> 12) * sinz) >> 12);
    ship->right.vy = (cosx * sinz) >> 12;
    ship->right.vz = ((-siny * cosz) >> 12) + ((((cosy * sinx) >> 12) * sinz) >> 12);

    ship->up.vx = ((-sinz * cosy) >> 12) + ((((siny * sinx) >> 12) * cosz) >> 12);
    ship->up.vy = (cosx * cosz) >> 12;
    ship->up.vz = ((-siny * -sinz) >> 12) + ((((cosy * sinx) >> 12) * cosz) >> 12);

    ship->forward.vx = (siny * cosx) >> 12;
    ship->forward.vy = (-sinx);
    ship->forward.vz = (cosy * cosx) >> 12;

    // Calcolo del vettore di spinta basato sulla magnitudine della spinta
    ship->thrust.vx = (ship->thrustmag * ship->forward.vx) >> 12;
    ship->thrust.vy = (ship->thrustmag * ship->forward.vy) >> 12;
    ship->thrust.vz = (ship->thrustmag * ship->forward.vz) >> 12;

    // Calcolo della velocità totale della nave
    ship->speed = SquareRoot0(ship->vel.vx * ship->vel.vx + ship->vel.vy * ship->vel.vy + ship->vel.vz * ship->vel.vz);

    // Calcolo della velocità lungo l'asse frontale
    nosevel.vx = (ship->speed * ship->forward.vx) >> 12;
    nosevel.vy = (ship->speed * ship->forward.vy) >> 12;
    nosevel.vz = (ship->speed * ship->forward.vz) >> 12;

    // Calcolo della distanza verticale tra la nave e la base del tracciato
    base2ship.vx = ship->object->position.vx - ship->section->basevertex.vx;
    base2ship.vy = ship->object->position.vy - ship->section->basevertex.vy;
    base2ship.vz = ship->object->position.vz - ship->section->basevertex.vz;
    dot = (ship->section->normal.vx * base2ship.vx) >> 12;
    dot += (ship->section->normal.vy * base2ship.vy) >> 12;
    dot += (ship->section->normal.vz * base2ship.vz) >> 12;

    height = dot;
    if (height < 50) {
        height = 50;
    }

    // Calcolo delle forze che agiscono sulla nave
    force = (VECTOR){0, 0, 0};

    // Attrazione verso il tracciato (forza gravitazionale simulata)
    force.vx += -ship->section->normal.vx * TRACK_PULL;
    force.vy += -ship->section->normal.vy * TRACK_PULL;
    force.vz += -ship->section->normal.vz * TRACK_PULL;

    // Repulsione dal tracciato
    force.vx += (ship->section->normal.vx * TRACK_PUSH) / height;
    force.vy += (ship->section->normal.vy * TRACK_PUSH) / height;
    force.vz += (ship->section->normal.vz * TRACK_PUSH) / height;

    // Aggiunta della spinta
    force.vx += ship->thrust.vx;
    force.vy += ship->thrust.vy;
    force.vz += ship->thrust.vz;

    // Aggiornamento dell'accelerazione
    ship->acc.vx = (nosevel.vx - ship->vel.vx);
    ship->acc.vy = (nosevel.vy - ship->vel.vy);
    ship->acc.vz = (nosevel.vz - ship->vel.vz);
    ship->acc.vx += force.vx / ship->mass;
    ship->acc.vy += force.vy / ship->mass;
    ship->acc.vz += force.vz / ship->mass;

    // Aggiornamento della velocità
    ship->vel.vx += ship->acc.vx;
    ship->vel.vy += ship->acc.vy;
    ship->vel.vz += ship->acc.vz;

    // Aggiornamento della posizione
    ship->object->position.vx += ship->vel.vx >> 6;
    ship->object->position.vy += ship->vel.vy >> 6;
    ship->object->position.vz += ship->vel.vz >> 6;

    // Aggiornamento del roll basato sulla velocità angolare di yaw
    ship->velroll = ship->velyaw >> 4;
    ship->velroll -= ship->velroll >> 1;

    // Aggiornamento degli angoli yaw, pitch e roll
    ship->yaw += ship->velyaw >> 6;
    ship->pitch += ship->velpitch;
    ship->roll += ship->velroll >> 1;

    // Ritorno alla posizione standard del roll
    ship->roll -= ship->roll >> 3;

    // Aggiorna la sezione più vicina alla nave
    UpdateShipNearestSection(ship);

    // Imposta la matrice di rotazione della nave
    ship->object->rotmat.m[0][0] = ship->right.vx;
    ship->object->rotmat.m[1][0] = ship->right.vy;
    ship->object->rotmat.m[2][0] = ship->right.vz;

    ship->object->rotmat.m[0][1] = ship->up.vx;
    ship->object->rotmat.m[1][1] = ship->up.vy;
    ship->object->rotmat.m[2][1] = ship->up.vz;

    ship->object->rotmat.m[0][2] = ship->forward.vx;
    ship->object->rotmat.m[1][2] = ship->forward.vy;
    ship->object->rotmat.m[2][2] = ship->forward.vz;
}


void DrawXYZAxis(Ship *ship, Camera *camera) {
    // Vettori per rappresentare le posizioni dei punti dell'asse
    SVECTOR v0, v1, v2;
    int i; // Variabile di supporto
    long otz; // Profondità Z nel sistema di coordinate dello schermo
    POLY_F3 *polya, *polyb, *polyc, *polyd; // Poligoni utilizzati per i punti dell'asse
    LINE_F2 lineA, lineB, lineC; // Linee per rappresentare gli assi

    VECTOR vec; // Vettore di posizione
    SVECTOR rot; // Rotazioni per la matrice di trasformazione
    MATRIX worldmat; // Matrice del mondo
    MATRIX viewmat;  // Matrice della vista

    // Inizializzazione della rotazione
    rot.vx = 0;
    rot.vy = 0;
    rot.vz = 0;
    RotMatrix(&rot, &worldmat); // Calcola la matrice di rotazione
    TransMatrix(&worldmat, &ship->object->position); // Applica la traslazione
    ScaleMatrix(&worldmat, &ship->object->scale); // Applica la scala
    CompMatrixLV(&camera->lookat, &worldmat, &viewmat); // Combina la matrice di visualizzazione
    SetRotMatrix(&viewmat); // Imposta la matrice di rotazione
    SetTransMatrix(&viewmat); // Imposta la matrice di traslazione

    // Disegno del punto di origine
    polya = (POLY_F3*) GetNextPrim();
    SetPolyF3(polya);
    v0.vx = (short) (0); // Punto di origine (0,0,0)
    v0.vy = (short) (0);
    v0.vz = (short) (0);
    v1 = v2 = v0; // Copia il punto
    otz  = RotTransPers(&v0, (long*)&polya->x0, NULL, NULL); // Trasformazione in prospettiva
    otz += RotTransPers(&v1, (long*)&polya->x1, NULL, NULL);
    otz += RotTransPers(&v2, (long*)&polya->x2, NULL, NULL);
    otz /= 3; // Calcolo media Z
    setRGB0(polya, 255, 255, 255); // Imposta colore bianco
    addPrim(GetOTAt(GetCurrBuff(), otz), polya); // Aggiunge il primitivo
    IncrementNextPrim(sizeof(POLY_FT3));

    // Disegno del vettore forward (asse Z)
    polyb = (POLY_F3*) GetNextPrim();
    SetPolyF3(polyb);
    v0.vx = (short) (ship->forward.vx >> 3);
    v0.vy = (short) (ship->forward.vy >> 3);
    v0.vz = (short) (ship->forward.vz >> 3);
    v1 = v2 = v0;
    otz  = RotTransPers(&v0, (long*)&polyb->x0, NULL, NULL);
    otz += RotTransPers(&v1, (long*)&polyb->x1, NULL, NULL);
    otz += RotTransPers(&v2, (long*)&polyb->x2, NULL, NULL);
    otz /= 3;
    setRGB0(polyb, 0, 0, 0); // Imposta colore nero
    addPrim(GetOTAt(GetCurrBuff(), otz), polyb);
    IncrementNextPrim(sizeof(POLY_FT3));

    // Disegno del vettore right (asse X)
    polyc = (POLY_F3*) GetNextPrim();
    SetPolyF3(polyc);
    v0.vx = (short) (ship->right.vx >> 4);
    v0.vy = (short) (ship->right.vy >> 4);
    v0.vz = (short) (ship->right.vz >> 4);
    v1 = v2 = v0;
    otz  = RotTransPers(&v0, (long*)&polyc->x0, NULL, NULL);
    otz += RotTransPers(&v1, (long*)&polyc->x1, NULL, NULL);
    otz += RotTransPers(&v2, (long*)&polyc->x2, NULL, NULL);
    otz /= 3;
    setRGB0(polyc, 0, 0, 0);
    addPrim(GetOTAt(GetCurrBuff(), otz), polyc);
    IncrementNextPrim(sizeof(POLY_FT3));

    // Disegno del vettore up (asse Y)
    polyd = (POLY_F3*) GetNextPrim();
    SetPolyF3(polyd);
    v0.vx = (short) -(ship->up.vx >> 4);
    v0.vy = (short) -(ship->up.vy >> 4);
    v0.vz = (short) -(ship->up.vz >> 4);
    v1 = v2 = v0;
    otz  = RotTransPers(&v0, (long*)&polyd->x0, NULL, NULL);
    otz += RotTransPers(&v1, (long*)&polyd->x1, NULL, NULL);
    otz += RotTransPers(&v2, (long*)&polyd->x2, NULL, NULL);
    otz /= 3;
    setRGB0(polyd, 0, 0, 0);
    addPrim(GetOTAt(GetCurrBuff(), otz), polyd);
    IncrementNextPrim(sizeof(POLY_FT3));

    // Disegna la linea blu per l'asse Z (forward)
    SetLineF2(&lineA);
    setXY2(&lineA, polya->x0, polya->y0, polyb->x0, polyb->y0);
    setRGB0(&lineA, 55, 150, 255);
    DrawPrim(&lineA);

    // Disegna la linea rossa per l'asse X (right)
    SetLineF2(&lineB);
    setXY2(&lineB, polya->x0, polya->y0, polyc->x0, polyc->y0);
    setRGB0(&lineB, 255, 55, 87);
    DrawPrim(&lineB);

    // Disegna la linea verde per l'asse Y (up)
    SetLineF2(&lineC);
    setXY2(&lineC, polya->x0, polya->y0, polyd->x0, polyd->y0);
    setRGB0(&lineC, 10, 255, 110);
    DrawPrim(&lineC);
}
