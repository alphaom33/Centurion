#include "calculator.h"
#include "keyUtils.h"
#include "stdint.h"
#include "stdlib.h"
#include "alloca.h"
#include "graphx.h"
#include "stdbool.h"
#include "mathUtils.h"

#define INTEGER_LENGTH 8
#define NUM_DATAS 10

typedef struct {
	char integerPart[INTEGER_LENGTH];
	bool positive;
} CalculatorData;

CalculatorData datas[NUM_DATAS];
uint8_t currentNum;

void addNumber() {
	char* str = datas[currentNum].integerPart;

	uint8_t digits;
	for (digits = 0; digits < INTEGER_LENGTH - 2 && str[digits] != 0; digits++);

	str[digits] = getNumber() + '0';
}

void initData(int i) {
	for (int j = 0; j < INTEGER_LENGTH; j++) {
		datas[i].integerPart[j] = 0;
	}
	datas[i].positive = true;
}

void initCalculator(void** data) {
	for (int i = 0; i < NUM_DATAS; i++) {
		initData(i);
	}
	currentNum = 0;
}

int pop() {
	int out = atoi(datas[currentNum].integerPart);
	initData(currentNum);
	if (currentNum > 0) currentNum--;
	return out;
}
void push(int num) {
	itoaUnsignedInt((unsigned int)num, datas[currentNum].integerPart);
	datas[currentNum].positive = num >= 0;
}

void procCalculator(void* data) {
	gfx_FillScreen(1);

	gfx_SetTextXY(0, 0);
	for (uint8_t i = 0; i <= currentNum; i++) {
		if (!datas[i].positive) gfx_PrintChar('-');
		gfx_PrintString(datas[i].integerPart);
		gfx_PrintChar(' ');
		gfx_PrintChar(' ');
	}

		if (getNumber() != UINT8_MAX) {
			addNumber();
		} else if (getKeyDown(kb_Key2nd)) {
			datas[currentNum].positive = !datas[currentNum].positive;
		} else if (currentNum < NUM_DATAS - 1 && datas[currentNum].integerPart[0] != 0 && getKeyDown(kb_KeyEnter)) {
			currentNum++;
		}

		if (currentNum > 0) {
#define BINARY_OP(op) do { \
	int num2 = pop(); \
	int num1 = pop(); \
	if (currentNum != 0) currentNum++; \
	push(num1 op num2); \
} while (false)
			if (getKeyDown(kb_KeyAdd)) {BINARY_OP(+);}
			else if (getKeyDown(kb_KeySub)) {BINARY_OP(-);}
			else if (getKeyDown(kb_KeyMul)) {BINARY_OP(*);}
			else if (getKeyDown(kb_KeyDiv)) {BINARY_OP(/);}
			else if (getKeyDown(kb_KeyPower)) {
				int num2 = pop();
				int num1 = pop();
				push(pow(num1, num2));
			}
		}
#undef BINARY_OP
}
