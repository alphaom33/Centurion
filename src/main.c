#include <ti/screen.h>
#include <ti/getcsc.h>
#include "gfx/LEDEncryptionImage1.h"
#include "gfx/LEDEncryptionImage2.h"
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
#include "safetySafe.h"
#include "morsematics.h"
#include "calculator.h"

#define NUM_SCREENS 7

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
    if (current != NULL) current->init(&data);
}

void defaultDraw(Screen* screens) {
    gfx_FillScreen(1);

    if (getNumber() != UINT8_MAX && getNumber() < NUM_SCREENS) {
        setCurrent(&screens[getNumber()]);
        num = 0;
        return;
    }

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
        
        char buf[20];
        sprintf(buf, "%i: %s", i, screens[i].name);
        gfx_PrintStringXY(buf, 1, y + 4);

        gfx_Sprite(time % 2 ? screens[i].image1 : screens[i].image2, GFX_LCD_WIDTH - SPRITE_WIDTH, y);
        if (i == num) global();
    }
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
}

void initLetterDisp(void** info) {
    (void)info;
}

void procLetterDisp(void* info) {
    (void)info;
    
    gfx_FillScreen(1);
    for (int i = 0; i < 26; i++) {
        char buf[4];
        sprintf(buf, "%c %d", i + 'A', i);
        gfx_PrintStringXY(buf, 0, i * TEXT_HEIGHT);
    }
}

void doProc(Screen* screens) {
    if (current == NULL) defaultDraw(screens);
    else current->proc(data);

    gfx_SwapDraw();
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
    makeScreen(&screens[3], "LetterDisp", LEDEncryptionImage1, LEDEncryptionImage2, initLetterDisp, procLetterDisp);
    makeScreen(&screens[4], "SafetySafe", LEDEncryptionImage1, LEDEncryptionImage2, initSafetySafe, procSafetySafe);
    makeScreen(&screens[5], "Morsematics", LEDEncryptionImage1, LEDEncryptionImage2, initMorsematics, procMorsematics);
    makeScreen(&screens[6], "Calculator", LEDEncryptionImage1, LEDEncryptionImage2, initCalculator, procCalculator);

    // such a hack but it works okay
    setCurrent(&screens[0]);
    doProc(screens);
    setCurrent(NULL);
    doProc(screens);
    setCurrent(&screens[2]);
    doProc(screens);

    while (!kb_On) {
        if (getKeyDown(kb_KeyDel)) {
            setCurrent(NULL);
        }

        doProc(screens);

        copyLast();
        kb_Scan();
        checkAlpha();
    }

    gfx_End();
    return 0;
}
