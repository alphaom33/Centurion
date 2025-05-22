#include "calculator.h"
#include "keyUtils.h"
#include "stdint.h"
#include "stdlib.h"
#include "alloca.h"
#include "graphx.h"
#include "stdbool.h"

typedef struct {
	char integerPart[5];
	bool positive;
} CalculatorData;

CalculatorData datas[10];

void initCalculator(void** data) {
}

void procCalculator(void* data) {
	gfx_FillScreen(1);

}
