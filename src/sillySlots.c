#include "sillySlots.h"
#include "graphicsUtils.h"
#include "graphx.h"
#include "keyUtils.h"
#include "stdio.h"
#include <stdint.h>
#include "mathUtils.h"

#define SELECTOR_Y_DST 5
#define SELECTOR_MARGIN 5
#define SELECTOR_WIDTH (TEXT_WIDTH * 9 + 2)
#define SELECTOR_HEIGHT (TEXT_HEIGHT + 4)
#define SELECTOR_LEFT_X (CENTER_X - SELECTOR_MARGIN - SELECTOR_WIDTH)
#define SELECTOR_RIGHT_X (CENTER_X + SELECTOR_MARGIN)
#define ITEM_HEIGHT (TEXT_HEIGHT + 2)

#define SELECTOR_Y(i) (SELECTOR_HEIGHT + SELECTOR_Y_DST) * ((i) + 1) + SELECTOR_Y_DST

typedef enum {
	SOGGY, SILLY, SASSY
} Attrs;

typedef enum {
	SALLY, SIMON, SAUSAGE, STEVEN
} Names;

typedef struct {
	uint8_t keyword;
	uint8_t attr[3];
	uint8_t name[3];
	uint8_t current;
} SillySlotsData;

void reinitSillySlots(SillySlotsData* info) {
	info->keyword = UINT8_MAX;

	info->attr[0] = UINT8_MAX;
	info->attr[1] = UINT8_MAX;
	info->attr[2] = UINT8_MAX;

	info->name[0] = UINT8_MAX;
	info->name[1] = UINT8_MAX;
	info->name[2] = UINT8_MAX;

	info->current = 0;
}

void initSillySlots(void** data) {
	static SillySlotsData info;
	*data = &info;
	reinitSillySlots(&info);
}

const char attrs[][8] = {"Soggy", "Silly", "Sassy"};
const char names[][8] = {"Sally", "Simon", "Sausage", "Steven"};

void selector(uint8_t y, uint8_t idx, SillySlotsData* info) {
	gfx_Rectangle(SELECTOR_LEFT_X, y, SELECTOR_WIDTH, SELECTOR_HEIGHT);
	gfx_Rectangle(SELECTOR_RIGHT_X, y, SELECTOR_WIDTH, SELECTOR_HEIGHT);
	if (info->attr[idx] != UINT8_MAX) {
		gfx_PrintStringXY(attrs[info->attr[idx]], SELECTOR_LEFT_X + 2, y + 2);
	}
	if (info->name[idx] != UINT8_MAX) {
		gfx_PrintStringXY(names[info->name[idx]], SELECTOR_RIGHT_X + 2, y + 2);
	}

}

void menu(uint8_t x, uint8_t y, uint8_t add, char names[][8], uint8_t* setry, uint8_t length, uint8_t current) {

	gfx_Rectangle(x, y, SELECTOR_WIDTH, ITEM_HEIGHT * length + 5);

	gfx_SetColor(1);
	gfx_FillRectangle(x + 1, y + 1, SELECTOR_WIDTH - 2, ITEM_HEIGHT * length);
	gfx_SetColor(2);

	for (int i = 0; i < length; i++) {
		char buf[20];
		sprintf(buf, "%d: %s", i + add, names[i]);
		gfx_PrintStringXY(buf, x + 2, y + ITEM_HEIGHT * i + 3);

		if (getNumber() == i + add) {
			setry[current] = i;
		}
	}
}

bool checkHappiness(SillySlotsData* info) {
	for (uint8_t i = 0; i < 7; i++) {
		if ((&info->keyword)[i] == UINT8_MAX) return false;
	}
	return true;
}

uint8_t countWithParams(uint8_t attr, uint8_t name, SillySlotsData* info) {
	uint8_t sum = 0;
	for (int i = 0; i < 3; i++) {
		if ((attr == UINT8_MAX || info->attr[i] == attr) && (name == UINT8_MAX || info->name[i] == name)) sum++;
	}
	return sum;
}

bool shouldPull(SillySlotsData* info) {
	for (int i = 0; i < 3; i++) {
		if (info->name[i] != SAUSAGE) continue;

		if (i > 0 && info->name[i - 1] == SALLY && info->attr[i - 1] != SOGGY) return true;
		if (i < 2 && info->name[i + 1] == SALLY && info->attr[i + 1] != SOGGY) return true;
	}

	return 
		countWithParams(SILLY, SAUSAGE, info) == 1
		|| countWithParams(SASSY, SALLY, info) == 1 //
		|| countWithParams(SOGGY, STEVEN, info) >= 2
		|| (countWithParams(UINT8_MAX, SIMON, info) && countWithParams(SASSY, SIMON, info) == 0)
		|| (countWithParams(SILLY, UINT8_MAX, info) == 2 && countWithParams(SILLY, STEVEN, info) != 2)
		|| (countWithParams(SOGGY, UINT8_MAX, info) == 1) //
		|| (countWithParams(SILLY, SIMON, info) > 0) //
		;
}

void procSillySlots(void* data) {
	SillySlotsData* info = (SillySlotsData*)data;

	gfx_FillScreen(1);
	gfx_SetColor(2);

	if (getKeyDown(kb_KeyUp)) info->current = wrap(info->current - 1, 4);
	else if (getKeyDown(kb_KeyDown)) info->current = wrap(info->current + 1, 4);

	gfx_Rectangle(CENTER_X - (SELECTOR_WIDTH / 2), SELECTOR_Y_DST, SELECTOR_WIDTH, SELECTOR_HEIGHT);
	if (info->keyword != UINT8_MAX) {
		gfx_PrintStringXY(attrs[info->keyword], CENTER_X - (SELECTOR_WIDTH / 2) + 2, SELECTOR_Y_DST + 2);
	}

	for (int i = 0; i < 3; i++) {
		selector(SELECTOR_Y(i), i, info);
	}
	if (info->current != 0) {
		menu(SELECTOR_LEFT_X, SELECTOR_Y(info->current - 1) + SELECTOR_HEIGHT - 2, 0, attrs, info->attr, 3, info->current - 1);
		menu(SELECTOR_RIGHT_X, SELECTOR_Y(info->current - 1) + SELECTOR_HEIGHT - 2, 3, names, info->name, 4, info->current - 1);
	} else {
		menu(CENTER_X - (SELECTOR_WIDTH / 2), SELECTOR_Y_DST + SELECTOR_HEIGHT - 2, 0, attrs, &info->keyword, 7, 0);
	}

	if (checkHappiness(info)) {
		gfx_PrintStringXY(shouldPull(info) ? "y" : "n", CENTER_X - TEXT_WIDTH / 2, SELECTOR_Y(3));
	}
}
