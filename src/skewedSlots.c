#include "skewedSlots.h"
#include "screen.h"
#include "graphicsUtils.h"
#include "keyUtils.h"

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
    return MARGIN + i * SLOT_WIDTH;
}

void drawFinal(SkewedSlotsData* info) {

}

void drawGetter(SkewedSlotsData* info) {
    int i;
    for (i = 0; i < 3 && info->nums[i] != UINT8_MAX; i++) {
        gfx_SetTextXY(getPos(i) + MIDDLE_OFFSET_X, MARGIN + MIDDLE_OFFSET_Y);
        gfx_PrintInt(info->nums[i], 1);
    }

    gfx_FillRectangle(getPos(i) + MIDDLE_OFFSET_X, MARGIN + MIDDLE_OFFSET_Y, TEXT_WIDTH, TEXT_HEIGHT);
}

void procSkewedSlots(void* data) {
    SkewedSlotsData* info = (SkewedSlotsData*)data;

    gfx_FillScreen(1);
    gfx_SetColor(2);
    for (int i = 0; i < 3; i++) {
        gfx_Rectangle(getPos(i), MARGIN, SLOT_WIDTH, SLOT_HEIGHT);
    }

    if (info->nums[2] != UINT8_MAX) {
        drawFinal(info);
    } else {
        if (getNumber() != UINT8_MAX) {
            int i = 0;
            for (i = 0; info->nums[i] != UINT8_MAX; i++);
            info->nums[i] = getNumber();
        }
        drawGetter(info);
    }
}