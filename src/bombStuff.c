#include "bombStuff.h"
#include "graphicsUtils.h"
#include "graphx.h"
#include "keyUtils.h"
#include "stdio.h"
#include "gfx/gfx.h"
#include "mathUtils.h"

#define MARGIN 20

#define SERIAL_PADDING 4
#define SERIAL_WIDTH ((TEXT_WIDTH * 6) + SERIAL_PADDING)
#define SERIAL_HEIGHT (TEXT_HEIGHT + SERIAL_PADDING)
#define SERIAL_X (CENTER_X - SERIAL_WIDTH / 2)
#define SERIAL_Y MARGIN

#define BATTERY_MARGIN 100
#define BATTERY_WIDTH 40
#define BATTERY_HEIGHT (TEXT_HEIGHT + SERIAL_PADDING)
#define BATTERY_Y (SERIAL_Y + SERIAL_HEIGHT + MARGIN)

#define SELECTOR_WIDTH (CENTER_X - 25)
#define SELECTOR_HEIGHT 138 // originally calculated as (GFX_LCD_HEIGHT - SELECTOR_Y - MARGIN). Needs to be divisible by NUM_PORTS
#define SELECTOR_Y (BATTERY_Y + BATTERY_HEIGHT + MARGIN)

#define PORT_HEIGHT (SELECTOR_HEIGHT / NUM_PORTS)

#define INDICATOR_X (GFX_LCD_WIDTH - MARGIN - SELECTOR_WIDTH)
#define INDICATOR_WIDTH (SELECTOR_WIDTH / 3)
#define INDICATOR_HEIGHT (SELECTOR_HEIGHT / 4)

BombStuff* bombStuff;

typedef struct {
    uint8_t current;    
    uint8_t selectedIndicator;
} BombStuffSetterData;

void reinitBombStuff(BombStuffSetterData* info) {
    info->current = 0;
    info->selectedIndicator = 0;
}

void initBombStuff() {
    bombStuff = malloc(sizeof(BombStuff));
    bombStuff->numBatteries = UINT8_MAX;
    bombStuff->numBatteryHolders = UINT8_MAX;

    for (int i = 0; i < SERIAL_LENGTH; i++) {
        bombStuff->serial[i] = 0;
    }

    for (int i = 0; i < NUM_PORTS; i++) {
        bombStuff->ports[i] = false;
    }

    for (int i = 0; i < NUM_INDICATORS; i++) {
        bombStuff->indicators[i] = (Indicator){false, false};
    }
}

void initBombStuffSetter(void** data) {
    *data = malloc(sizeof(BombStuffSetterData));
    reinitBombStuff((BombStuffSetterData*)*data);
}

void moveUp(BombStuffSetterData *info) {
    switch (info->current) {
        case 0: info->current = 3; break;

        case 1:
        case 2:
            info->current = 0;
            break;

        case 3:
        case 4:
            info->current -= 2;
            break;
    }
}

void moveDown(BombStuffSetterData* info) {
    switch (info->current) {
        case 0: info->current++; break;

        case 1:
        case 2:
            info->current += 2;
            break;

        case 3:
        case 4:
            info->current = 0;
            break;
    }
}

void moveHorizontal(BombStuffSetterData* info) {
    switch (info->current) {
        case 1:
        case 3:
            info->current++;
            break;

        case 2:
        case 4:
            info->current--;
            break;
    }
}

void moveNext(BombStuffSetterData* info) {
    info->current++;
    copyLast();
}

void drawSerial(BombStuffSetterData* info) {
    int i = 0;
    for (i = 0; bombStuff->serial[i] != 0; i++);

    if (info->current == 0) {
        if (getKeyDown(kb_Clear)) {
            bombStuff->serial[0] = 0;
        }
        else if (i < 6 && getKeypad() != UINT8_MAX) {
            bombStuff->serial[i] = getKeypad();
            i++;
        }

        if (i >= SERIAL_LENGTH) {
            moveNext(info);
        } else {
            gfx_PrintStringXY(
                getAlphaed() ? "a" : "a\u2588",
                SERIAL_X + SERIAL_PADDING / 2 + TEXT_WIDTH * i,
                MARGIN + SERIAL_PADDING / 2);
        }
    }

    gfx_SetTextXY(SERIAL_X + SERIAL_PADDING / 2, MARGIN + SERIAL_PADDING / 2);
    gfx_PrintString(bombStuff->serial);

}

void drawBattery(BombStuffSetterData* info) {
    if (info->current == 1 && getNumber() != UINT8_MAX) {
        bombStuff->numBatteries = getNumber();
        moveNext(info);
    }

    uint8_t posX = BATTERY_MARGIN + SERIAL_PADDING / 2;
    uint8_t posY = BATTERY_Y + SERIAL_PADDING / 2;
    if (bombStuff->numBatteries != UINT8_MAX) {
        gfx_SetTextXY(posX, posY);
        gfx_PrintInt(bombStuff->numBatteries, 1);
    } else if (info->current == 1) {
        gfx_FillRectangle(posX, posY, TEXT_WIDTH, TEXT_HEIGHT);
    }
}

void drawBatteryHolder(BombStuffSetterData* info) {
    if (info->current == 2 && getNumber() != UINT8_MAX) {
        bombStuff->numBatteryHolders = getNumber();
        moveNext(info);
    }

    uint8_t posX = GFX_LCD_WIDTH - BATTERY_MARGIN - BATTERY_WIDTH + SERIAL_PADDING / 2;
    uint8_t posY = BATTERY_Y + SERIAL_PADDING / 2;
    if (bombStuff->numBatteryHolders != UINT8_MAX) {
        gfx_SetTextXY(posX, posY);
        gfx_PrintInt(bombStuff->numBatteryHolders, 1);
    } else if (info->current == 2) {
        gfx_FillRectangle(posX, posY, TEXT_WIDTH, TEXT_HEIGHT);
    }
}

const char* portNames[] = {"DVI-D", "PARALLEL", "PS/2", "RJ-45", "SERIAL", "STEREO RCA"};

void drawPorts(BombStuffSetterData* info) {
    if (info->current == 3 && getNumber() < NUM_PORTS) {
            bombStuff->ports[getNumber()] = !bombStuff->ports[getNumber()];
    }

    for (int i = 0; i < NUM_PORTS; i++) {
        uint8_t y = SELECTOR_Y + i * PORT_HEIGHT;

        if (bombStuff->ports[i]) {
            menuSelected();
            gfx_FillRectangle(MARGIN, y, SELECTOR_WIDTH, PORT_HEIGHT);
        } else {
            menuGlobal();
            gfx_Rectangle(MARGIN, y, SELECTOR_WIDTH, PORT_HEIGHT);
        }


        gfx_SetTextXY((info->current == 3 ? (i == 1 ? 1 : 0) : 14) + MARGIN + SERIAL_PADDING / 2, y + (PORT_HEIGHT - TEXT_HEIGHT) / 2);
        char num[13];
        if (info->current == 3) sprintf(num, "%d: %s", i, portNames[i]);
        else sprintf(num, "%s", portNames[i]);
        gfx_PrintString(num);

        gfx_Sprite(LEDEncryptionImage1, CENTER_X - 25, y + (PORT_HEIGHT - 16) / 2);
    }
    menuGlobal();

}

const char* indicatorNames[] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};

void drawIndicators(BombStuffSetterData* info) {
    if (info->current == 4) {
        if (getKeypad() != UINT8_MAX) {
            uint8_t keypad = getKeypad();
            info->selectedIndicator = keypad - (keypad < 'A' ? 0 : 'A');
        }

        Indicator *indicator = &bombStuff->indicators[info->selectedIndicator];
        if (getKeyDown(kb_KeyEnter)) {
            indicator->exists = !indicator->exists;
        } else if (getKeyDown(kb_Key2nd)) {
            indicator->on = !indicator->on;
        }

        if (getKeyDown(kb_KeyUp)) {
            info->current = wrap(info->current - 3, NUM_INDICATORS);
        } else if (getKeyDown(kb_KeyRight)) {
            uint8_t x = info->current % 3;
            info->current = info->current - x + wrap(x + 1, 3);
        } else if (getKeyDown(kb_KeyLeft)) {
            uint8_t x = info->current % 3;
            info->current = info->current - x + wrap(x - 1, 3);
        } else if (getKeyDown(kb_KeyDown)) {
            info->current = wrap(info->current + 3, NUM_INDICATORS);
        }
    }

    for (int i = 0; i < NUM_INDICATORS; i++) {
        if (bombStuff->indicators[i].exists) menuSelected();
        else menuGlobal();

        uint8_t x = INDICATOR_X + INDICATOR_WIDTH * (i % 3);
        uint8_t y = SELECTOR_Y + INDICATOR_HEIGHT * (i / 3);
        gfx_Rectangle(x, y, INDICATOR_WIDTH, INDICATOR_HEIGHT);

        char buf[7];
        if (info->current == 4) {
            sprintf(buf, "%c: %s", i + (i < 10 ? '0' : 'A'), indicatorNames[i]);
        } else {
            sprintf(buf, "%s", indicatorNames[i]);
        }

        gfx_PrintStringXY(
            buf,
            (info->current == 4 ? 0 : 14) + x + SERIAL_PADDING / 2,
            y + SERIAL_PADDING / 2);

        gfx_Sprite(
            bombStuff->indicators[i].on ? LEDEncryptionImage1 : LEDEncryptionImage2,
            x + INDICATOR_WIDTH - 16 - SERIAL_PADDING,
            y + INDICATOR_HEIGHT / 2 - 8);
    }

    menuGlobal();
}

void procBombStuff(void* data) {
    BombStuffSetterData* info = (BombStuffSetterData*)data;
    
    if (getKeyDown(kb_KeyUp)) moveUp(info);
    else if (getKeyDown(kb_KeyDown)) moveDown(info);
    else if (getKeyDown(kb_KeyRight)) moveHorizontal(info);
    else if (getKeyDown(kb_KeyLeft)) moveHorizontal(info);

    gfx_FillScreen(1);
    gfx_SetColor(2);
    gfx_Rectangle(SERIAL_X, SERIAL_Y, SERIAL_WIDTH, SERIAL_HEIGHT);

    gfx_Rectangle(BATTERY_MARGIN, BATTERY_Y, BATTERY_WIDTH, BATTERY_HEIGHT);
    gfx_Rectangle(GFX_LCD_WIDTH - BATTERY_MARGIN - BATTERY_WIDTH, BATTERY_Y, BATTERY_WIDTH, BATTERY_HEIGHT);

    gfx_Rectangle(MARGIN, SELECTOR_Y, SELECTOR_WIDTH, SELECTOR_HEIGHT);
    gfx_Rectangle(INDICATOR_X, SELECTOR_Y, SELECTOR_WIDTH, SELECTOR_HEIGHT);

    drawSerial(info);
    drawBattery(info);
    drawBatteryHolder(info);
    drawPorts(info);
    drawIndicators(info);
}