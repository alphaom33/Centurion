#include "ledEncryption.h"
#include "graphx.h"
#include "keyUtils.h"
#include "gfx/gfx.h"
#include "graphicsUtils.h"
#include "stdio.h"

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

typedef struct {
    uint8_t nums[4];
    uint8_t multiplier;
} LEDEncryption;

static void reinitLEDEncryption(LEDEncryption* data) {
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
        gfx_SetTextXY(centerX[i] - HALF_TEXT_WIDTH, centerY[i] - HALF_TEXT_HEIGHT);
        gfx_PrintChar(info->nums[i] + 'A');
    }

    if (info->multiplier != UINT8_MAX) {
        gfx_SetTextXY(CENTER_X - HALF_TEXT_WIDTH, CENTER_Y - HALF_TEXT_HEIGHT);
        gfx_PrintInt(info->multiplier, 1);
    } else if (info->nums[3] == UINT8_MAX) {
        gfx_FillRectangle(centerX[i] - HALF_TEXT_WIDTH, centerY[i] - HALF_TEXT_HEIGHT, 4, 8);
    } else {
        gfx_FillRectangle(CENTER_X - HALF_TEXT_WIDTH, CENTER_Y - HALF_TEXT_HEIGHT, 4, 8);
        drawRainbow();
    }
}