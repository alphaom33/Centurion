#include "skewedSlots.h"
#include "screen.h"
#include "graphicsUtils.h"
#include "keyUtils.h"
#include "bombStuff.h"
#include "mathUtils.h"

#define MARGIN 8
#define SLOT_WIDTH ((GFX_LCD_WIDTH - MARGIN) / 3)
#define SLOT_HEIGHT (GFX_LCD_HEIGHT - MARGIN)

#define MIDDLE_OFFSET_X (SLOT_WIDTH / 2 - HALF_TEXT_WIDTH)
#define MIDDLE_OFFSET_Y (SLOT_HEIGHT / 2 - HALF_TEXT_HEIGHT)

typedef struct {
    uint8_t nums[3];
} SkewedSlotsData;

void reinitSkewedSlots(SkewedSlotsData *info) {
    info->nums[0] = UINT8_MAX;
    info->nums[1] = UINT8_MAX;
    info->nums[2] = UINT8_MAX;
}

void initSkewedSlots(void** data) {
    *data = malloc(sizeof(SkewedSlotsData));
    reinitSkewedSlots((SkewedSlotsData*)*data);
}

int getPos(int i) {
    return MARGIN / 2 + i * SLOT_WIDTH;
}

void calcFinal(SkewedSlotsData* info) {
    int numIndicators = 0;
    for (int i = 0; i < NUM_INDICATORS; i++) {
        if (bombStuff->indicators[i].exists) {
            numIndicators += bombStuff->indicators[i].on ? 1 : -1;
        }
    }

    int8_t finalNums[3];
    for (int i = 0; i < 3; i++) {
        finalNums[i] = info->nums[i];
        if (finalNums[i] == 2) finalNums[i] = 5;
        if (finalNums[i] == 7) finalNums[i] = 0;
        finalNums[i] += numIndicators;

        if (finalNums[i] % 3 == 0) finalNums[i] += 4;
        else if (finalNums[i] > 7) finalNums[i] *= 2;
        else if (finalNums[i] < 3 && isEven(finalNums[i])) finalNums[i] /= 2;
        else if (bombStuff->ports[STEREO_RCA] || bombStuff->ports[PS2]) continue;
        else finalNums[i] = info->nums[i] + bombStuff->numBatteries;
    }

    if (finalNums[0] > 5 && isEven(finalNums[0])) finalNums[0] /= 2;
    else if (isPrime(finalNums[0])) finalNums[0] += getLastSerialNumber();
    else if (bombStuff->ports[PARALLEL]) finalNums[0] *= -1;
    else if (!isEven(info->nums[1])) {}
    else finalNums[0] -= 2;

    if (bombStuff->indicators[BOB].exists && !bombStuff->indicators[BOB].on) {}
    else if (finalNums[1] == 0) finalNums[1] += info->nums[0];
    else if (inFibonacci(finalNums[1])) finalNums[1] += nextFibonacci(finalNums[1]);
    else if (finalNums[1] >= 7) finalNums[1] += 4;
    else finalNums[1] *= 3;

    if (bombStuff->ports[SERIAL]) finalNums[2] += getLargestSerialNumber();
    else if (info->nums[2] == info->nums[0] || info->nums[2] == info->nums[1]) {}
    else if (finalNums[2] >= 5) finalNums[2] = addBinaryDigits(info->nums[2]);
    else finalNums[2]++;

    for (int i = 0; i < 3; i++) {
        while (finalNums[i] > 9) finalNums[i] -= 10;
        while (finalNums[i] < 0) finalNums[i] += 10;

        info->nums[i] = finalNums[i];
    }
}

void drawGetter(SkewedSlotsData* info) {
    int i;
    for (i = 0; i < 3 && info->nums[i] != UINT8_MAX; i++) {
        gfx_SetTextXY(getPos(i) + MIDDLE_OFFSET_X, MARGIN / 2 + MIDDLE_OFFSET_Y);
        gfx_PrintInt(info->nums[i], 1);
    }

    if (i < 3) gfx_FillRectangle(getPos(i) + MIDDLE_OFFSET_X, MARGIN / 2 + MIDDLE_OFFSET_Y, TEXT_WIDTH, TEXT_HEIGHT);
}

void procSkewedSlots(void* data) {
    SkewedSlotsData* info = (SkewedSlotsData*)data;

    if (getKeyDown(kb_KeyEnter) && info->nums[2] != UINT8_MAX) {
        reinitSkewedSlots(info);
    }

    gfx_FillScreen(1);
    gfx_SetColor(2);
    for (int i = 0; i < 3; i++) {
        gfx_Rectangle(getPos(i), MARGIN / 2, SLOT_WIDTH, SLOT_HEIGHT);
    }

    if (getNumber() != UINT8_MAX) {
        int i = 0;
        for (i = 0; info->nums[i] != UINT8_MAX; i++);
        info->nums[i] = getNumber();

        if (i == 2) calcFinal(info);
    }
    drawGetter(info);
}
