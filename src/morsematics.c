#include "morsematics.h"
#include "graphicsUtils.h"
#include "bombStuff.h"
#include "mathUtils.h"
#include "stdlib.h"
#include <stdint.h>
#include "keyUtils.h"
#include "graphx.h"
#include "stdio.h"

#define MARGIN 8
#define BOX_WIDTH ((GFX_LCD_WIDTH - MARGIN * 2) / 3)
#define BOX_HEIGHT ((GFX_LCD_HEIGHT - MARGIN * 2))

typedef struct {
	uint8_t num;
	uint8_t chars[3];
} MorsematicsData;

void checkMatches(MorsematicsData* info, int8_t* num1, int8_t* num2) {
	for (int k = 0; k < NUM_INDICATORS; k++) {
		if (!bombStuff->indicators[k].exists) continue;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (bombStuff->indicators[k].name[i] == info->chars[j]) {
					if (bombStuff->indicators[k].on) *num1 += 1;
					else *num2 += 1;

					goto nextInd;
				}
			}
		}
nextInd:
		continue;
	}
}

uint8_t toNum(char c) {
	if (c == '0') return 26;
	else if (c < 'A') return c - '0';
	else return c - 'A' + 1;
}

uint8_t weirdWrap(int8_t num) {
	return wrap(num - 1, 26) + 1;
}

void calcChar(MorsematicsData* info) {
	int8_t num1 = toNum(bombStuff->serial[3]);
	int8_t num2 = toNum(bombStuff->serial[4]);

	checkMatches(info, &num1, &num2);
	num1 = weirdWrap(num1);
	num2 = weirdWrap(num2);

	if (isSquare(weirdWrap(num1 + num2))) num1 += 4;
	else num2 -= 4;

	num1 += toNum(maxArrayUnsigned(info->chars, 3));

	for (int i = 0; i < 3; i++) {
		int8_t charNum = toNum(info->chars[i]);
		if (isPrime(charNum)) num1 -= charNum;
		if (isSquare(charNum)) num2 -= charNum;
		if (getNumBatteries() > 0 && charNum % getNumBatteries() == 0) {
			num1 -= charNum;
			num2 -= charNum;
		}
	}

	num1 = weirdWrap(num1);
	num2 = weirdWrap(num2);
	int8_t result = num1 == num2
				? num1 
				: num1 > num2
					? num1 - num2
					: num1 + num2;
	info->num = weirdWrap(result) - 1 + 'A';
}

void reinitMorsematics(MorsematicsData* info) {
	info->num = UINT8_MAX;

	info->chars[0] = UINT8_MAX;
	info->chars[1] = UINT8_MAX;
	info->chars[2] = UINT8_MAX;
}

void initMorsematics(void** data) {
	*data = malloc(sizeof(MorsematicsData));
	reinitMorsematics((MorsematicsData*)*data);
}

void procMorsematics(void* data) {
	MorsematicsData* info = (MorsematicsData*)data;
	int current = 0;
	for (current = 0; info->chars[current] != UINT8_MAX && current < 3; current++);

	gfx_FillScreen(1);
	gfx_SetColor(2);


	if (current < 3 && getLetter() != UINT8_MAX) {
		info->chars[current] = getLetter();
		if (current == 2) calcChar(info);
	}

	if (info->num != UINT8_MAX) {
		gfx_SetTextXY(CENTER_X - HALF_TEXT_WIDTH, CENTER_Y - HALF_TEXT_HEIGHT);
		gfx_PrintChar(info->num);

		if (getKeyDown(kb_KeyEnter)) reinitMorsematics(info);

		return;
	} else if (getKeyDown(kb_KeyClear)) {
		info->chars[current - 1] = UINT8_MAX;
	}


	for (int i = 0; i < 3; i++) {
		uint8_t x = MARGIN + BOX_WIDTH * i;
		gfx_Rectangle(x, MARGIN, BOX_WIDTH, BOX_HEIGHT);
		if (info->chars[i] != UINT8_MAX) {
			gfx_SetTextXY(x + BOX_WIDTH / 2 - HALF_TEXT_WIDTH, MARGIN + BOX_HEIGHT / 2 - HALF_TEXT_HEIGHT);
			gfx_PrintChar(info->chars[i]);
		}
	}
}

void testMatches(void) {
#define SETUP_TEST() do { \
	num1 = 1, num2 = 1; \
	initBombStuff(); \
	{ \
		MorsematicsData redata = {0, {0, 0, 0}}; \
		info = redata; \
	} \
} while (false)

#define RUN_TEST(name) do { \
	num1 = weirdWrap(num1); \
	num2 = weirdWrap(num2); \
	printf("%s: %d, %d\n", name, num1, num2); \
} while (false)

	MorsematicsData info;

	int8_t num1, num2;

	SETUP_TEST();
	checkMatches(&info, &num1, &num2);
	RUN_TEST("basic");

	SETUP_TEST();
	bombStuff->serial[0] = 'S';
	info.chars[0] = 'S';
	bombStuff->indicators[SIG].exists = true;
	checkMatches(&info, &num1, &num2);
	RUN_TEST("sigOff");

	SETUP_TEST();
	bombStuff->serial[0] = 'S';
	info.chars[0] = 'S';
	bombStuff->indicators[SIG].exists = true;
	bombStuff->indicators[SIG].on = true;
	checkMatches(&info, &num1, &num2);
	RUN_TEST("sigOn");

	SETUP_TEST();
	num1 = weirdWrap(27);
	num2 = weirdWrap(-1);
	RUN_TEST("wrap");

	SETUP_TEST();
	info.chars[0] = 'A';
	info.chars[1] = 'Z';
	info.chars[2] = 'B';
	printf("Max: %d\n", toNum(maxArrayUnsigned(info.chars, 3)));

	printf("squares: %d, %d, %d, %d, %d\n", isSquare(1), isSquare(4), isSquare(3), isSquare(9), isSquare(16));
	printf("toNum: %d, %d, %d, %d, %d\n", toNum('A'), toNum('Z'), toNum('0'), toNum('1'), toNum('9'));

#undef SETUP_TEST
#undef RUN_TEST
}
