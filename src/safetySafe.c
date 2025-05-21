#include "safetySafe.h"
#include "graphicsUtils.h"
#include "stdlib.h"
#include "bombStuff.h"
#include "graphx.h"
#include "alloca.h"

#define MARGIN 8
#define BOX_WIDTH ((GFX_LCD_WIDTH - 8) / 3)
#define BOX_HEIGHT ((GFX_LCD_HEIGHT - 8) / 2)

typedef struct {
	uint8_t nums[6];
} SafetySafe;

uint8_t table[][6] = {
	['A'] = {8, 	3, 	4, 	8, 	9, 	0},
	['B'] = {10, 	1, 	3, 	7, 	3, 	8},
	['C'] = {2, 	1, 	1, 	5, 	3, 	6},
	['D'] = {11, 	6, 	11,	11,	7, 	7},
	['E'] = {0, 	5, 	5, 	8, 	2, 	1},
	['F'] = {4, 	2, 	7, 	7, 	1, 	5},
	['G'] = {7, 	4, 	4, 	2, 	10,	5},
	['H'] = {8, 	3, 	6, 	6, 	6, 	5},
	['I'] = {0, 	11,	0, 	0, 	9, 	10},
	['J'] = {2, 	11,	8, 	0, 	5, 	6},
	['K'] = {5, 	2, 	5, 	1, 	0, 	4},
	['L'] = {1, 	9, 	8, 	11,	11,	11},
	['M'] = {1, 	7, 	9, 	5, 	6, 	2},
	['N'] = {9, 	5, 	1, 	4, 	4, 	9},
	['O'] = {5, 	9, 	8, 	10,	2, 	8},
	['P'] = {3, 	10,	9, 	1, 	9, 	7},
	['Q'] = {4, 	10,	6, 	1, 	4, 	8},
	['R'] = {8, 	0, 	4, 	0, 	6, 	11},
	['S'] = {9, 	4, 	0, 	6, 	3, 	10},
	['T'] = {7, 	6, 	7, 	11,	5, 	3},
	['U'] = {11, 	9, 	6, 	3, 	11,	1},
	['V'] = {11, 	11,	2, 	8, 	1, 	0},
	['W'] = {6, 	0, 	11,	6, 	11,	2},
	['X'] = {4, 	2, 	7, 	2, 	8, 	10},
	['Y'] = {10, 	7, 	10,	10,	8, 	9},
	['Z'] = {3, 	7, 	1, 	10,	0, 	4},
	['0'] = {7, 	0, 	3, 	5, 	8, 	6},
	['1'] = {9, 	10,	10,	9, 	1, 	2},
	['2'] = {2, 	5, 	11,	7, 	7, 	3},
	['3'] = {10, 	8, 	10,	4, 	10,	4},
	['4'] = {6, 	8, 	0, 	3, 	5, 	0},
	['5'] = {6, 	3, 	3, 	3, 	0, 	11},
	['6'] = {1, 	1, 	5, 	2, 	7, 	3},
	['7'] = {0, 	6, 	2, 	4, 	2, 	1},
	['8'] = {5, 	4, 	9, 	9, 	10,	7},
	['9'] = {3, 	8, 	2, 	9, 	4, 	9},
};

void reinitSafetySafe(SafetySafe* info) {
	/*int ports = 0;*/
	/*for (int i = 0; i < NUM_PORTS; i++) {*/
	/*	if (bombStuff->ports[i]) ports++;*/
	/*}*/

	/*uint8_t indicatorsOnInSerial = 0;*/
	/*uint8_t indicatorsOffInSerial = 0;*/
	/*for (int i = 0; i < NUM_INDICATORS; i++) {*/
	/*	if (bombStuff->indicators[i].exists && indicatorInSerial(bombStuff->indicators[i])) {*/
	/*		if (bombStuff->indicators[i].on) indicatorsOnInSerial++;*/
	/*		else indicatorsOffInSerial++;*/
	/*	}*/
	/*}*/
	/**/
	/*uint8_t num = 0;*/
	/**/
	/*num += ports * 7;*/
	/*num += indicatorsOnInSerial * 5;*/
	/*num += indicatorsOffInSerial;*/
	/**/
	/*info->nums[5] = num;*/
	/*for (int i = 0; i < 5; i++) {*/
	/*	info->nums[i] = table[(uint8_t)bombStuff->serial[i]][i] + num;*/
	/*	info->nums[5] = table[(uint8_t)bombStuff->serial[i]][i];*/
	/*}*/
	/*info->nums[5] = table[(uint8_t)bombStuff->serial[5]][5];*/
}

void initSafetySafe(void** data) {
    *data = malloc(sizeof(SafetySafe));
	/*reinitSafetySafe((SafetySafe*)*data);*/
}

void procSafetySafe(void* data) {
	/*SafetySafe* info = (SafetySafe*)data;*/

	gfx_FillScreen(1);
	/*for (int x = 0; x < 3; x++) {*/
	/*	for (int y = 0; y < 2; y++) {*/
	/*		gfx_Rectangle(MARGIN + BOX_WIDTH * x, MARGIN + BOX_HEIGHT * y, BOX_WIDTH, BOX_HEIGHT);*/
	/*		gfx_SetTextXY(MARGIN + BOX_WIDTH * x + BOX_WIDTH / 2, MARGIN + BOX_HEIGHT * y + BOX_HEIGHT / 2);*/
	/*		gfx_PrintInt(info->nums[x + y * 3], 1);*/
	/*	}*/
	/*}*/
	gfx_SwapDraw();
}
