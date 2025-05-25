#include "bombStuff.h"
#include "graphicsUtils.h"
#include "graphx.h"
#include "keyUtils.h"
#include "stdio.h"
#include "gfx/gfx.h"
#include "mathUtils.h"
#include <stddef.h>
#include "alloca.h"

#define MARGIN 20

#define SERIAL_PADDING 4
#define SERIAL_WIDTH ((TEXT_WIDTH * 6) + SERIAL_PADDING)
#define SERIAL_HEIGHT (TEXT_HEIGHT + SERIAL_PADDING)
#define SERIAL_X (CENTER_X - SERIAL_WIDTH / 2)
#define SERIAL_Y MARGIN

#define BATTERY_MARGIN 100
#define BATTERY_WIDTH 40
#define BATTERY_MARGIN_Y 8
#define BATTERY_HEIGHT (TEXT_HEIGHT + SERIAL_PADDING)
#define AA_BATTERY_Y (SERIAL_Y + SERIAL_HEIGHT + MARGIN - (BATTERY_HEIGHT / 2 + BATTERY_MARGIN_Y / 2))
#define D_BATTERY_Y (AA_BATTERY_Y + BATTERY_HEIGHT + BATTERY_MARGIN_Y)
#define BATTERY_Y (SERIAL_Y + SERIAL_HEIGHT + MARGIN)

#define SELECTOR_WIDTH (CENTER_X - 25)
#define SELECTOR_HEIGHT 138 // originally calculated as (GFX_LCD_HEIGHT - SELECTOR_Y - MARGIN). Needs to be divisible by NUM_PORTS
#define SELECTOR_Y (BATTERY_Y + BATTERY_HEIGHT + MARGIN)

#define PORT_HEIGHT (SELECTOR_HEIGHT / NUM_PORTS)

#define INDICATOR_X_NUM 2
#define INDICATOR_X (GFX_LCD_WIDTH - MARGIN - SELECTOR_WIDTH)
#define INDICATOR_WIDTH (SELECTOR_WIDTH / INDICATOR_X_NUM)
#define INDICATOR_HEIGHT (16 + SERIAL_PADDING)

enum input {
	SERIAL_INPUT,
	AA_INPUT,
	D_INPUT,
	HOLDER_INPUT,
	PORT_INPUT,
	INDICATOR_INPUT,
};

BombStuff* bombStuff;

typedef struct {
    uint8_t current;    
    uint8_t serialDst;
    uint8_t selectedIndicator;
} BombStuffSetterData;

void reinitBombStuff(BombStuffSetterData* info) {
    info->current = 0;
    info->serialDst = 0;
    info->selectedIndicator = 0;
}

void initBombStuff() {
	const char indicatorNames[][4] = {"SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"};
	static BombStuff bomb;

    bombStuff = &bomb;
    bombStuff->numAA = UINT8_MAX;
    bombStuff->numD = UINT8_MAX;
    bombStuff->numBatteryHolders = UINT8_MAX;

    for (int i = 0; i < SERIAL_LENGTH + 1; i++) {
        bombStuff->serial[i] = 0;
    }

    for (int i = 0; i < NUM_PORTS; i++) {
        bombStuff->ports[i] = false;
    }

    for (int i = 0; i < NUM_INDICATORS; i++) {
        bombStuff->indicators[i].exists = false;
        bombStuff->indicators[i].on = false;
		bombStuff->indicators[i].name[0] = indicatorNames[i][0];
		bombStuff->indicators[i].name[1] = indicatorNames[i][1];
		bombStuff->indicators[i].name[2] = indicatorNames[i][2];
		bombStuff->indicators[i].name[3] = indicatorNames[i][3];
    }
	bombStuff->duplicates = false;
}

void initBombStuffSetter(void** data) {
    *data = malloc(sizeof(BombStuffSetterData));
    reinitBombStuff((BombStuffSetterData*)*data);
}

void moveUp(BombStuffSetterData *info) {
    switch (info->current) {
        case SERIAL_INPUT: info->current = PORT_INPUT; break;

        case AA_INPUT:
        case HOLDER_INPUT:
            info->current = SERIAL_INPUT;
            break;

		case D_INPUT:
			info->current = AA_INPUT;
			break;

        case PORT_INPUT:
        case INDICATOR_INPUT:
            info->current -= 2;
            break;
    }
}

void moveDown(BombStuffSetterData* info) {
    switch (info->current) {
        case SERIAL_INPUT: info->current++; break;

		case AA_INPUT:
			info->current = D_INPUT;
			break;

        case D_INPUT:
        case HOLDER_INPUT:
            info->current += 2;
            break;

        case PORT_INPUT:
        case INDICATOR_INPUT:
            info->current = SERIAL_INPUT;
            break;
    }
}

void moveHorizontal(BombStuffSetterData* info) {
    switch (info->current) {
        case D_INPUT:
        case PORT_INPUT:
            info->current++;
            break;

		case AA_INPUT:
			info->current = HOLDER_INPUT;
			break;

        case HOLDER_INPUT:
        case INDICATOR_INPUT:
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

    if (info->current == SERIAL_INPUT) {
        if (getKeyDown(kb_KeyClear)) {
            info->serialDst -= getOffset(bombStuff->serial[i - 1]);
            bombStuff->serial[i - 1] = 0;
            i--;
        }
        if (i < 6) {
            if (getKeypad() != UINT8_MAX) {
                uint8_t keypad = getKeypad();
                bombStuff->serial[i] = keypad;
                i++;
                info->serialDst += getOffset(keypad);

                if (i >= SERIAL_LENGTH) {
                    moveNext(info);
                    return;
                }
            }
            gfx_FillRectangle(
                    SERIAL_X + SERIAL_PADDING / 2 + info->serialDst,
                    MARGIN + SERIAL_PADDING / 2,
                    TEXT_WIDTH,
                    TEXT_HEIGHT);
        }
    }

    gfx_Rectangle(SERIAL_X, SERIAL_Y, SERIAL_WIDTH, SERIAL_HEIGHT);
    gfx_PrintStringXY(bombStuff->serial, SERIAL_X + SERIAL_PADDING / 2, MARGIN + SERIAL_PADDING / 2);
}

void drawAA(BombStuffSetterData* info) {
    if (info->current == AA_INPUT && getNumber() != UINT8_MAX) {
        bombStuff->numAA = getNumber();
        moveNext(info);
    }

    gfx_Rectangle(BATTERY_MARGIN, AA_BATTERY_Y, BATTERY_WIDTH, BATTERY_HEIGHT);

    uint8_t posX = BATTERY_MARGIN + SERIAL_PADDING / 2;
    uint8_t posY = AA_BATTERY_Y + SERIAL_PADDING / 2;

    if (bombStuff->numAA != UINT8_MAX) {
        gfx_SetTextXY(posX, posY);
        gfx_PrintInt(bombStuff->numAA, 1);
    } else if (info->current == AA_INPUT) {
        gfx_FillRectangle(posX, posY, TEXT_WIDTH, TEXT_HEIGHT);
    }
}


void drawD(BombStuffSetterData* info) {
    if (info->current == D_INPUT && getNumber() != UINT8_MAX) {
        bombStuff->numD = getNumber();
        moveNext(info);
    }

    gfx_Rectangle(BATTERY_MARGIN, D_BATTERY_Y, BATTERY_WIDTH, BATTERY_HEIGHT);

    uint8_t posX = BATTERY_MARGIN + SERIAL_PADDING / 2;
    uint8_t posY = D_BATTERY_Y + SERIAL_PADDING / 2;
    if (bombStuff->numD != UINT8_MAX) {
        gfx_SetTextXY(posX, posY);
        gfx_PrintInt(bombStuff->numD, 1);
    } else if (info->current == D_INPUT) {
        gfx_FillRectangle(posX, posY, TEXT_WIDTH, TEXT_HEIGHT);
    }
}


void drawBatteryHolder(BombStuffSetterData* info) {
    if (info->current == HOLDER_INPUT && getNumber() != UINT8_MAX) {
        bombStuff->numBatteryHolders = getNumber();
        moveNext(info);
    }

    gfx_Rectangle(GFX_LCD_WIDTH - BATTERY_MARGIN - BATTERY_WIDTH, BATTERY_Y, BATTERY_WIDTH, BATTERY_HEIGHT);

    int posX = GFX_LCD_WIDTH - BATTERY_MARGIN - BATTERY_WIDTH + SERIAL_PADDING / 2;

    uint8_t posY = BATTERY_Y + SERIAL_PADDING / 2;
    if (bombStuff->numBatteryHolders != UINT8_MAX) {
        gfx_SetTextXY(posX, posY);
        gfx_PrintInt(bombStuff->numBatteryHolders, 1);
    } else if (info->current == HOLDER_INPUT) {
        gfx_FillRectangle(posX, posY, TEXT_WIDTH, TEXT_HEIGHT);
    }
}

const char* portNames[] = {"DVI-D", "PARALLEL", "PS/2", "RJ-45", "SERIAL", "STEREO RCA"};
const gfx_sprite_t* portSprites[] = {DVIDImage, ParallelImage, PS2Image, RJ45Image, SerialImage, StereoRCAImage};

void drawPorts(BombStuffSetterData* info) {
    if (info->current == PORT_INPUT) {
		if (getNumber() < NUM_PORTS) {
            bombStuff->ports[getNumber()] = !bombStuff->ports[getNumber()];
		} else if (getKeyDown(kb_Key2nd)) {
			bombStuff->duplicates = !bombStuff->duplicates;
		}
    }

    gfx_Rectangle(MARGIN, SELECTOR_Y, SELECTOR_WIDTH, SELECTOR_HEIGHT);

    for (int i = 0; i < NUM_PORTS; i++) {
        uint8_t y = SELECTOR_Y + i * PORT_HEIGHT;

        if (bombStuff->ports[i]) {
            menuSelected();
            gfx_FillRectangle(MARGIN, y, SELECTOR_WIDTH, PORT_HEIGHT);
        } else {
            menuGlobal();
            gfx_Rectangle(MARGIN, y, SELECTOR_WIDTH, PORT_HEIGHT);
        }


        gfx_SetTextXY((info->current == PORT_INPUT ? (i == 1 ? 1 : 0) : 14) + MARGIN + SERIAL_PADDING / 2, y + (PORT_HEIGHT - TEXT_HEIGHT) / 2);
        char num[13];
        if (info->current == PORT_INPUT) sprintf(num, "%d: %s", i, portNames[i]);
        else sprintf(num, "%s", portNames[i]);
        gfx_PrintString(num);

        gfx_TransparentSprite(portSprites[i], CENTER_X - 9 - portSprites[i]->width, y + (PORT_HEIGHT - 16) / 2);
    }
    menuGlobal();

}

void drawIndicators(BombStuffSetterData* info) {
    if (info->current == INDICATOR_INPUT) {
        if (getKeypad() != UINT8_MAX) {
            uint8_t keypad = getKeypad();
            info->selectedIndicator = keypad - (keypad < 'A' ? '0' : 'A' - 10);
        }

        Indicator *indicator = &bombStuff->indicators[info->selectedIndicator];
        if (getKeyDown(kb_KeyEnter)) {
            indicator->exists = !indicator->exists;
            if (!indicator->exists) indicator->on = false;
        } else if (getKeyDown(kb_Key2nd)) {
			indicator->exists = true;
            indicator->on = !indicator->on;
        }
    } else {
        info->selectedIndicator = 0;
    }

    gfx_Rectangle(INDICATOR_X, SELECTOR_Y, SELECTOR_WIDTH, SELECTOR_HEIGHT);

    for (int i = 0; i < NUM_INDICATORS; i++) {
        if (bombStuff->indicators[i].exists) menuSelected();
        else menuGlobal();

        uint8_t x = INDICATOR_X + INDICATOR_WIDTH * (i % INDICATOR_X_NUM);
        uint8_t y = SELECTOR_Y + INDICATOR_HEIGHT * (i / INDICATOR_X_NUM);
        if (bombStuff->indicators[i].exists) gfx_FillRectangle(x, y, INDICATOR_WIDTH, INDICATOR_HEIGHT);
        else gfx_Rectangle(x, y, INDICATOR_WIDTH, INDICATOR_HEIGHT);

        if (info->current == INDICATOR_INPUT && info->selectedIndicator == i) gfx_Rectangle(x + 1, y + 1, INDICATOR_WIDTH - 2, INDICATOR_HEIGHT - 2);

        char buf[7];
        if (info->current == INDICATOR_INPUT) {
            sprintf(buf, "%c: %s", i + (i < 10 ? '0' : 'A' - 10), bombStuff->indicators[i].name);
        } else {
            sprintf(buf, "%s", bombStuff->indicators[i].name);
        }

        gfx_PrintStringXY(
            buf,
            (info->current == INDICATOR_INPUT ? i == 1 : 14) + x + SERIAL_PADDING,
            y + INDICATOR_HEIGHT / 2 - HALF_TEXT_HEIGHT);

        gfx_TransparentSprite(
            bombStuff->indicators[i].on ? IndicatorOn : IndicatorOff,
            x + INDICATOR_WIDTH - 16 - SERIAL_PADDING / 2,
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

    drawSerial(info);
    drawAA(info);
	drawD(info);
    drawBatteryHolder(info);
    drawPorts(info);
    drawIndicators(info);
}

uint8_t getLastSerialNumber() {
    int i;
    for (i = SERIAL_LENGTH - 1; i >= 0; i++) {
        uint8_t toNum = bombStuff->serial[i] - '0';
        if (toNum < 10) return toNum;
    }

    return UINT8_MAX;
}

uint8_t getLargestSerialNumber() {
    uint8_t max = 0;

    for (int i = 0; i < SERIAL_LENGTH; i++) {
        uint8_t toNum = bombStuff->serial[i] - '0';
        if (toNum < 10 && toNum > max) max = toNum;
    }

    return max;
}

bool indicatorInSerial(Indicator indicator) {
	for (int i = 0; i < SERIAL_LENGTH; i++) {
		for (int k = 0; k < 3; k++) {
			if (indicator.exists && bombStuff->serial[i] == indicator.name[k]) return true;
		}
	}
	return false;
}

uint8_t getNumBatteries() {
	return bombStuff->numD + bombStuff->numAA;
}
