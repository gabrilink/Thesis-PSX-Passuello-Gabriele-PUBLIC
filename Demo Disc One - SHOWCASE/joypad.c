#include <sys/types.h>
#include <libetc.h>
#include "joypad.h"
#include "globals.h"


#include "display.h"
#include <libgpu.h>
#include <libgte.h>
#include "inline_n.h"
static u_long padstate;
static u_long oldPadstate;


int JoyPadCheck(int p) {
    return padstate & p;
}

void JoyPadInit(void) {
    PadInit(0); // Initialize the PlayStation pad
    padstate = 0;
    oldPadstate = 0;

    // Highlight the first option
    menuItems[MENU_ITEM_1].r = 0;
    menuItems[MENU_ITEM_1].g = 122;
    menuItems[MENU_ITEM_1].b = 204;
}

u_long GetPadState(void) {
    return padstate;
}


void JoyPadUpdate(void) {
    oldPadstate = padstate;
    padstate = PadRead(0);

    // Resetta il colore dell'opzione attualmente selezionata
    menuItems[currentMenu].r = 90;
    menuItems[currentMenu].g = 90;
    menuItems[currentMenu].b = 90;

    if (appState == STATE_MENU) {


        if ((padstate & PAD1_UP) && !(oldPadstate & PAD1_UP)) {
            currentMenu = (currentMenu % 4 == 0) ? currentMenu + 3 : currentMenu - 1;
        }
        if ((padstate & PAD1_DOWN) && !(oldPadstate & PAD1_DOWN)) {
            currentMenu = (currentMenu % 4 == 3) ? currentMenu - 3 : currentMenu + 1;
        }
        if ((padstate & PAD1_LEFT) && !(oldPadstate & PAD1_LEFT)) {
            currentMenu = (currentMenu < 4) ? currentMenu + 4 : currentMenu - 4;
        }
        if ((padstate & PAD1_RIGHT) && !(oldPadstate & PAD1_RIGHT)) {
            currentMenu = (currentMenu < 4) ? currentMenu + 4 : currentMenu - 4;
        }
        if ((padstate & PAD1_CROSS) && !(oldPadstate & PAD1_CROSS)) {
            selectedProgram = currentMenu;
            appState = STATE_PROGRAM;
        }
    } else if (appState == STATE_PROGRAM) {
        if ((padstate & PAD1_SELECT) && !(oldPadstate & PAD1_SELECT)) {
    appState = STATE_MENU;
    selectedProgram = -1;

    // Ripristina il colore dello sfondo
    setRGB0(&screen.draw[0], 30, 30, 30); // Colore originale
    setRGB0(&screen.draw[1], 30, 30, 30); // Colore originale

    // Segna che il cubo deve essere reinizializzato
    isCubeInitialized = 0;
}
    }


    menuItems[currentMenu].r = 0;
    menuItems[currentMenu].g = 122;
    menuItems[currentMenu].b = 204;
}
