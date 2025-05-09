#include <ti/screen.h>
#include <ti/getcsc.h>
#include "graphx.h"
#include "keypadc.h"
#include <stdlib.h>
#include "gfx/gfx.h"
#include <sys/rtc.h>
#include "debug.h"

#define NUM_SCREENS 1
#define CENTER_X GFX_LCD_WIDTH / 2
#define CENTER_Y GFX_LCD_HEIGHT / 2

#define MENU_START 16
#define MENU_HEIGHT 16

#define SPRITE_WIDTH 16

#define LED_ENCRYPTION_MARGIN 10
#define LED_ENCRYPTION_WIDTH GFX_LCD_WIDTH - 2 * LED_ENCRYPTION_MARGIN
#define LED_ENCRYPTION_HEIGHT GFX_LCD_HEIGHT - 2 * LED_ENCRYPTION_MARGIN
#define LED_ENCRYPTION_CENTER_WIDTH 30
#define LED_ENCRYPTION_CENTER_HEIGHT 20


uint8_t kb_LastData = 0;

bool getKey(uint8_t key) {
    return kb_Data[key >> 8] & key;
}

bool getKeyDown(uint8_t key) {
    return getKey(key) && !getKey(key);
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
    int multiplier;
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
}

char getLetter() {
    if (getKeyDown(kb_Math)) return 'A';
    else if (getKeyDown(kb_Apps)) return 'B';
    else if (getKeyDown(kb_Prgm)) return 'C';
    else if (getKeyDown(kb_Recip)) return 'D';
    else if (getKeyDown(kb_Sin)) return 'E';
    else if (getKeyDown(kb_Cos)) return 'F';
    else if (getKeyDown(kb_Tan)) return 'G';
    else if (getKeyDown(kb_Suppr)) return 'H';
    else if (getKeyDown(kb_Square)) return 'I';
    else if (getKeyDown(kb_Comma)) return 'J';
    else if (getKeyDown(kb_LParen)) return 'K';
    else if (getKeyDown(kb_RParen)) return 'L';
    else if (getKeyDown(kb_Div)) return 'M';
    else if (getKeyDown(kb_Log)) return 'N';
    else if (getKeyDown(kb_7)) return 'O';
    else if (getKeyDown(kb_8)) return 'P';
    else if (getKeyDown(kb_9)) return 'Q';
    else if (getKeyDown(kb_Mul)) return 'R';
    else if (getKeyDown(kb_Ln)) return 'S';
    else if (getKeyDown(kb_4)) return 'T';
    else if (getKeyDown(kb_5)) return 'U';
    else if (getKeyDown(kb_6)) return 'V';
    else if (getKeyDown(kb_Sub)) return 'W';
    else if (getKeyDown(kb_Sto)) return 'X';
    else if (getKeyDown(kb_1)) return 'Y';
    else if (getKeyDown(kb_2)) return 'Z';
    else return UINT8_MAX;
}

uint8_t getNumber() {
    if (getKeyDown(kb_0)) return 0;
    else if (getKeyDown(kb_1)) return 1;
    else if (getKeyDown(kb_2)) return 2;
    else if (getKeyDown(kb_3)) return 3;
    else if (getKeyDown(kb_4)) return 4;
    else if (getKeyDown(kb_5)) return 5;
    else if (getKeyDown(kb_6)) return 6;
    else if (getKeyDown(kb_7)) return 7;
    else if (getKeyDown(kb_8)) return 8;
    else if (getKeyDown(kb_9)) return 9;
    else return UINT8_MAX;
}

char solveLEDEncryption(LEDEncryption* info) {
    for (int i = 0; i < 4; i++) {
        int val = (info->nums[i] * info->multiplier) % 26;
        if (val == info->nums[3 - i]) {
            return info->nums[i] + 'A';
        }
    }
    dbg_printf("Um");
    return UINT8_MAX;
}

void procLEDEncryption(void* data) {
    LEDEncryption *info = (LEDEncryption*)data;

    if (getKeyDown(kb_Enter)) {
        gfx_FillScreen(1);
        gfx_SetTextXY(CENTER_X, CENTER_Y);
        gfx_PrintChar(solveLEDEncryption(info));
        gfx_SwapDraw();
        while (!os_GetCSC());
        reinitLEDEncryption(info);
    } else if (getKeyDown(kb_Clear)) {
        reinitLEDEncryption(info);
    } else if (getLetter() != UINT8_MAX && info->nums[3] == UINT8_MAX) {
        int i;
        for (int i = 0; info->nums[i] != UINT8_MAX; i++);
        info->nums[i] = getLetter() - 'A';
    } else if (getNumber() != UINT8_MAX) {
        info->multiplier = getNumber();
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

    gfx_Rectangle(LED_ENCRYPTION_MARGIN, LED_ENCRYPTION_MARGIN, width, height);
    gfx_Rectangle(LED_ENCRYPTION_MARGIN + width, LED_ENCRYPTION_MARGIN, width, height);
    gfx_Rectangle(LED_ENCRYPTION_MARGIN, LED_ENCRYPTION_MARGIN + height, width, height);
    gfx_Rectangle(LED_ENCRYPTION_MARGIN + width, LED_ENCRYPTION_MARGIN + height, width, height);

    gfx_SetPalette(selected_palette, sizeof_selected_palette, 0);
    gfx_FillRectangle(CENTER_X - LED_ENCRYPTION_CENTER_WIDTH / 2, CENTER_Y - LED_ENCRYPTION_CENTER_HEIGHT / 2, LED_ENCRYPTION_CENTER_WIDTH, LED_ENCRYPTION_CENTER_HEIGHT);
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_Rectangle(CENTER_X - LED_ENCRYPTION_CENTER_WIDTH / 2, CENTER_Y - LED_ENCRYPTION_CENTER_HEIGHT / 2, LED_ENCRYPTION_CENTER_WIDTH, LED_ENCRYPTION_CENTER_HEIGHT);

    int i;
    for (i = 0; i < 4 && info->nums[i] != UINT8_MAX; i++) {
        gfx_SetTextXY(centerX[i], centerY[i]);
        gfx_PrintChar(info->nums[i] + 'A');
    }

    if (info->multiplier != UINT8_MAX) {
        gfx_PrintUInt(info->multiplier, 1);
    } else {
        gfx_FillRectangle(centerX[i], centerY[i], 4, 8);
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
    if (current != NULL) {
        free(data);
    }
    current = newCurrent;
    current->init(data);
}

int wrap(int num, int max) {
    return (num % max + max) % max;
}

void defaultDraw(Screen* screens) {

    if (getKeyDown(kb_Enter)) {
        setCurrent(&screens[num]);
        num = 0;
        return;
    } else if (getKeyDown(kb_Up)) {
        num = wrap(num - 1, NUM_SCREENS);
    } else if (getKeyDown(kb_Down)) {
        num = wrap(num + 1, NUM_SCREENS);
    }

    for (int i = 0, y = MENU_START; i < NUM_SCREENS; i++, y += MENU_HEIGHT) {
        if (i == num) gfx_SetPalette(selected_palette, sizeof_selected_palette, 0);
        else gfx_SetPalette(global_palette, sizeof_global_palette, 0);

        gfx_FillRectangle(0, y, GFX_LCD_WIDTH, MENU_HEIGHT);
        gfx_SetTextXY(0, y);
        gfx_PrintString(screens[0].name);
        gfx_Sprite(rtc_Seconds % 2 ? screens[i].image1 : screens[i].image2, GFX_LCD_WIDTH - SPRITE_WIDTH, y);
    }
}

/* Main function, called first */
int main(void) {
    gfx_Begin();
    gfx_SetDrawBuffer();
    gfx_SetPalette(global_palette, sizeof_global_palette, 0);
    gfx_SetTransparentColor(0);
    kb_Scan();
    kb_LastData = kb_Data;

    Screen screens[NUM_SCREENS];
    makeScreen(&screens[0], "LEDEncryption", oiram, icon, initLEDEncryption, procLEDEncryption);

    while (true) {
        gfx_FillScreen(1);
        if (getKeyDown(kb_Power)) {
            current = NULL;
        }

        if (current == NULL) defaultDraw(screens);
        else current->proc(data);

        gfx_SwapDraw();
        kb_LastData = kb_Data;
    }

    return 0;
}