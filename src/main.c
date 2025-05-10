#include <ti/screen.h>
#include <ti/getcsc.h>
#include "graphx.h"
#include "keypadc.h"
#include <stdlib.h>
#include "gfx/gfx.h"
#include <sys/rtc.h>
#include "debug.h"
#include "time.h"
#include <fontlibc.h>

#define TEXT_WIDTH 8
#define TEXT_HEIGHT 16

#define NUM_SCREENS 7
#define CENTER_X GFX_LCD_WIDTH / 2
#define CENTER_Y GFX_LCD_HEIGHT / 2

#define MENU_START 0
#define MENU_MARGIN 8
#define MENU_HEIGHT 16

#define SPRITE_WIDTH 16

#define LED_ENCRYPTION_MARGIN 10
#define LED_ENCRYPTION_WIDTH (GFX_LCD_WIDTH - 2 * LED_ENCRYPTION_MARGIN)
#define LED_ENCRYPTION_HEIGHT (GFX_LCD_HEIGHT - 2 * LED_ENCRYPTION_MARGIN)
#define LED_ENCRYPTION_CENTER_WIDTH 50
#define LED_ENCRYPTION_CENTER_HEIGHT 50

#define RAINBOW_WIDTH 110
#define RAINBOW_HEIGHT 60
#define RAINBOW_MARGIN_X 8
#define RAINBOW_MARGIN_Y 12
#define RAINBOW_X (CENTER_X - (RAINBOW_WIDTH + RAINBOW_MARGIN_X) / 2)
#define RAINBOW_Y (CENTER_Y + 10)
#define RAINBOW_BAR_WIDTH (RAINBOW_WIDTH - 2 * TEXT_WIDTH)
#define RAINBOW_BAR_HEIGHT (RAINBOW_HEIGHT / 6)
#define RAINBOW_BAR_X (CENTER_X - RAINBOW_BAR_WIDTH / 2 - TEXT_WIDTH)
#define RAINBOW_BAR_Y (RAINBOW_Y + RAINBOW_MARGIN_Y / 2)

#define HALF_TEXT_X 4
#define HALF_TEXT_Y 8

uint8_t kb_LastData[20];

void global() {
    gfx_SetTextBGColor(1);
    gfx_SetTextFGColor(2);
    gfx_SetColor(1);
}

void selected() {
    gfx_SetTextBGColor(2);
    gfx_SetTextFGColor(1);
    gfx_SetColor(2);
}

#define getKey(datum, key) (datum[(key) >> 8] & (key))

bool getKeyDown(kb_lkey_t key) {
    return getKey(kb_Data, key) && !getKey(kb_LastData, key);
}

typedef void (*Init)(void** data);
typedef void (*Proc)(void* data);

typedef struct {
    const char* name;
    gfx_sprite_t* image1;
    gfx_sprite_t* image2;
    Init init;
    Proc proc;
} Screen;

typedef struct {
    uint8_t nums[4];
    uint8_t multiplier;
} LEDEncryption;

void reinitLEDEncryption(LEDEncryption* data) {
    data->multiplier = UINT8_MAX;
    data->nums[0] = UINT8_MAX;
    data->nums[1] = UINT8_MAX;
    data->nums[2] = UINT8_MAX;
    data->nums[3] = UINT8_MAX;
}

void initLEDEncryption(void** data) {
    *data = malloc(sizeof(LEDEncryption));
    reinitLEDEncryption((LEDEncryption*)*data);
    gfx_SetPalette(led_encryption, sizeof_led_encryption, 0);
}

uint8_t getLetter() {
    if (getKeyDown(kb_KeyMath)) return 'A';
    else if (getKeyDown(kb_KeyApps)) return 'B';
    else if (getKeyDown(kb_KeyPrgm)) return 'C';
    else if (getKeyDown(kb_KeyRecip)) return 'D';
    else if (getKeyDown(kb_KeySin)) return 'E';
    else if (getKeyDown(kb_KeyCos)) return 'F';
    else if (getKeyDown(kb_KeyTan)) return 'G';
    else if (getKeyDown(kb_KeySuppr)) return 'H';
    else if (getKeyDown(kb_KeySquare)) return 'I';
    else if (getKeyDown(kb_KeyComma)) return 'J';
    else if (getKeyDown(kb_KeyLParen)) return 'K';
    else if (getKeyDown(kb_KeyRParen)) return 'L';
    else if (getKeyDown(kb_KeyDiv)) return 'M';
    else if (getKeyDown(kb_KeyLog)) return 'N';
    else if (getKeyDown(kb_Key7)) return 'O';
    else if (getKeyDown(kb_Key8)) return 'P';
    else if (getKeyDown(kb_Key9)) return 'Q';
    else if (getKeyDown(kb_KeyMul)) return 'R';
    else if (getKeyDown(kb_KeyLn)) return 'S';
    else if (getKeyDown(kb_Key4)) return 'T';
    else if (getKeyDown(kb_Key5)) return 'U';
    else if (getKeyDown(kb_Key6)) return 'V';
    else if (getKeyDown(kb_KeySub)) return 'W';
    else if (getKeyDown(kb_KeySto)) return 'X';
    else if (getKeyDown(kb_Key1)) return 'Y';
    else if (getKeyDown(kb_Key2)) return 'Z';
    else return UINT8_MAX;
}

uint8_t getNumber() {
    if (getKeyDown(kb_Key0)) return 0;
    else if (getKeyDown(kb_Key1)) return 1;
    else if (getKeyDown(kb_Key2)) return 2;
    else if (getKeyDown(kb_Key3)) return 3;
    else if (getKeyDown(kb_Key4)) return 4;
    else if (getKeyDown(kb_Key5)) return 5;
    else if (getKeyDown(kb_Key6)) return 6;
    else if (getKeyDown(kb_Key7)) return 7;
    else if (getKeyDown(kb_Key8)) return 8;
    else if (getKeyDown(kb_Key9)) return 9;
    else return UINT8_MAX;
}

uint8_t solveLEDEncryption(LEDEncryption* info) {
    for (int i = 0; i < 4; i++) {
        uint8_t val = (info->nums[i] * info->multiplier) % 26;
        if (val == info->nums[3 - i]) {
            return info->nums[i] + 'A';
        }
    }

    return 'x';
}

void drawRainbow() {
    gfx_SetColor(1);
    gfx_FillRectangle(RAINBOW_X, RAINBOW_Y, RAINBOW_WIDTH + RAINBOW_MARGIN_X, RAINBOW_HEIGHT + RAINBOW_MARGIN_Y);
    gfx_SetColor(2);
    gfx_Rectangle(RAINBOW_X, RAINBOW_Y, RAINBOW_WIDTH + RAINBOW_MARGIN_X, RAINBOW_HEIGHT + RAINBOW_MARGIN_Y);
    for (int i = 0; i < 6; i++) {
        gfx_SetTextXY(RAINBOW_BAR_X, RAINBOW_BAR_Y + i * RAINBOW_BAR_HEIGHT);
        char buf[3];
        sprintf(buf, "%d:", i);
        gfx_PrintString(buf);

        gfx_SetColor(i + 3);
        gfx_FillRectangle(RAINBOW_BAR_X + TEXT_WIDTH * 2, RAINBOW_BAR_Y + i * RAINBOW_BAR_HEIGHT, RAINBOW_BAR_WIDTH, RAINBOW_BAR_HEIGHT);
    }
}

void procLEDEncryption(void* data) {
    LEDEncryption *info = (LEDEncryption*)data;
    gfx_FillScreen(1);

    if (getKeyDown(kb_KeyClear)) {
        reinitLEDEncryption(info);
    } else if (getLetter() != UINT8_MAX && info->nums[3] == UINT8_MAX) {
        int i;
        for (i = 0; info->nums[i] != UINT8_MAX; i++);
        info->nums[i] = getLetter() - 'A';
    } else if (getNumber() != UINT8_MAX) {
        info->multiplier = getNumber() + 2;
    }

    if (info->multiplier != UINT8_MAX) {
        gfx_FillScreen(1);
        gfx_SetTextXY(CENTER_X, CENTER_Y);
        gfx_PrintChar(solveLEDEncryption(info));
        if (getKeyDown(kb_KeyEnter)) reinitLEDEncryption(info);
        return;
    }

    const int width = LED_ENCRYPTION_WIDTH / 2;
    const int height = LED_ENCRYPTION_HEIGHT / 2;
    const int centerX[] = {
        LED_ENCRYPTION_MARGIN + width / 2,
        LED_ENCRYPTION_MARGIN + 3 * width / 2,
        LED_ENCRYPTION_MARGIN + width / 2,
        LED_ENCRYPTION_MARGIN + 3 * width / 2,
    };
    const int centerY[] = {
        LED_ENCRYPTION_MARGIN + height / 2,
        LED_ENCRYPTION_MARGIN + height / 2,
        LED_ENCRYPTION_MARGIN + 3 * height / 2,
        LED_ENCRYPTION_MARGIN + 3 * height / 2,
    };

    gfx_SetColor(2);
    gfx_Rectangle(LED_ENCRYPTION_MARGIN, LED_ENCRYPTION_MARGIN, width, height);
    gfx_Rectangle(LED_ENCRYPTION_MARGIN + width, LED_ENCRYPTION_MARGIN, width, height);
    gfx_Rectangle(LED_ENCRYPTION_MARGIN, LED_ENCRYPTION_MARGIN + height, width, height);
    gfx_Rectangle(LED_ENCRYPTION_MARGIN + width, LED_ENCRYPTION_MARGIN + height, width, height);

    gfx_SetColor(1);
    gfx_FillRectangle(CENTER_X - LED_ENCRYPTION_CENTER_WIDTH / 2, CENTER_Y - LED_ENCRYPTION_CENTER_HEIGHT / 2, LED_ENCRYPTION_CENTER_WIDTH, LED_ENCRYPTION_CENTER_HEIGHT);
    gfx_SetColor(2);
    gfx_Rectangle(CENTER_X - LED_ENCRYPTION_CENTER_WIDTH / 2, CENTER_Y - LED_ENCRYPTION_CENTER_HEIGHT / 2, LED_ENCRYPTION_CENTER_WIDTH, LED_ENCRYPTION_CENTER_HEIGHT);

    int i;
    for (i = 0; i < 4 && info->nums[i] != UINT8_MAX; i++) {
        gfx_SetTextXY(centerX[i] - HALF_TEXT_X, centerY[i] - HALF_TEXT_Y);
        gfx_PrintChar(info->nums[i] + 'A');
    }

    if (info->multiplier != UINT8_MAX) {
        gfx_SetTextXY(CENTER_X - HALF_TEXT_X, CENTER_Y - HALF_TEXT_Y);
        gfx_PrintInt(info->multiplier, 1);
    } else if (info->nums[3] == UINT8_MAX) {
        gfx_FillRectangle(centerX[i] - HALF_TEXT_X, centerY[i] - HALF_TEXT_Y, 4, 8);
    } else {
        gfx_FillRectangle(CENTER_X - HALF_TEXT_X, CENTER_Y - HALF_TEXT_Y, 4, 8);
        drawRainbow();
    }
}

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

int wrap(int num, int max) {
    return (num % max + max) % max;
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
        gfx_PrintString(screens[0].name);
        gfx_Sprite(time % 2 ? screens[i].image1 : screens[i].image2, GFX_LCD_WIDTH - SPRITE_WIDTH, y);
        if (i == num) global();
    }
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
}

void copyLast() {
    for (int i = 0; i < 8; i++) {
        kb_LastData[i] = kb_Data[i];
    }
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

    Screen screens[NUM_SCREENS];
    makeScreen(&screens[0], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    makeScreen(&screens[1], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    makeScreen(&screens[2], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    makeScreen(&screens[3], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    makeScreen(&screens[4], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    makeScreen(&screens[5], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    makeScreen(&screens[6], "LEDEncryption", LEDEncryptionImage1, LEDEncryptionImage2, initLEDEncryption, procLEDEncryption);
    current = NULL;

    while (!kb_On) {
        if (getKey(kb_Data, kb_KeyDel)) {
            current = NULL;
        }

        if (current == NULL) defaultDraw(screens);
        else current->proc(data);

        gfx_SwapDraw();
        copyLast();
        kb_Scan();
    }

    gfx_End();
    return 0;
}