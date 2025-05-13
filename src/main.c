#include <ti/screen.h>
#include <ti/getcsc.h>
#include "graphx.h"
#include "screen.h"
#include "ledEncryption.h"
#include <stdlib.h>
#include "gfx/gfx.h"
#include <sys/rtc.h>
#include "debug.h"
#include "time.h"
#include <fontlibc.h>
#include "keyUtils.h"
#include "ledEncryption.h"
#include "skewedSlots.h"
#include "bombStuff.h"
#include "graphicsUtils.h"
#include "mathUtils.h"

#define NUM_SCREENS 3

#define MENU_START 0
#define MENU_MARGIN 8
#define MENU_HEIGHT 16

#define SPRITE_WIDTH 16

void makeScreen(Screen* screen, const char* name, gfx_sprite_t* image1, gfx_sprite_t* image2, Init init, Proc proc) {
    screen->name = name;
    screen->image1 = image1;
    screen->image2 = image2;
    screen->init = init;
    screen->proc = proc;
}

void* data;
Screen* current = NULL;
int num = 0;

void setCurrent(Screen *newCurrent) {
    free(data);
    current = newCurrent;
    current->init(&data);
}

void defaultDraw(Screen* screens) {
    gfx_FillScreen(1);

    if (getKeyDown(kb_KeyEnter)) {
        setCurrent(&screens[num]);
        num = 0;
        return;
    } else if (getKeyDown(kb_KeyUp)) {
        num = wrap(num - 1, NUM_SCREENS);
    } else if (getKeyDown(kb_KeyDown)) {
        num = wrap(num + 1, NUM_SCREENS);
    }

    clock_t time = clock() / 5000;
    for (int i = 0, y = MENU_START; i < NUM_SCREENS; i++, y += MENU_HEIGHT + MENU_MARGIN) {
        if (i == num) selected();

        gfx_FillRectangle(0, y, GFX_LCD_WIDTH, MENU_HEIGHT);
        gfx_SetTextXY(1, y + 4);
        gfx_PrintString(screens[i].name);
        gfx_Sprite(time % 2 ? screens[i].image1 : screens[i].image2, GFX_LCD_WIDTH - SPRITE_WIDTH, y);
        if (i == num) global();
    }
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
}

/* Main function, called first */
int main(void) {
    gfx_Begin();
    gfx_SetDrawBuffer();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTransparentColor(0);
    kb_DisableOnLatch();
    global();
    kb_Scan();
    copyLast();
    initBombStuff();

    Screen screens[NUM_SCREENS];
    makeScreen(&screens[0], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    makeScreen(&screens[1], "SkewedSlots", LEDEncryptionImage1, LEDEncryptionImage2, initSkewedSlots, procSkewedSlots);
    makeScreen(&screens[2], "BombStuff", LEDEncryptionImage1, LEDEncryptionImage2, initBombStuffSetter, procBombStuff);
    setCurrent(&screens[2]);

    while (!kb_On) {
        if (getKey(kb_Data, kb_KeyDel)) {
            current = NULL;
        }

        if (current == NULL) defaultDraw(screens);
        else current->proc(data);

        gfx_SwapDraw();
        copyLast();
        kb_Scan();
        checkAlpha();
    }

    gfx_End();
    return 0;
}