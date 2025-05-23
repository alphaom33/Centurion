#ifndef math_utils_h
#define math_utils_h

#include "stdint.h"
#include "stdbool.h"

uint8_t wrap(uint8_t num, uint8_t max);

bool inFibonacci(int8_t num);
int8_t nextFibonacci(int8_t num);

bool isPrime(int8_t num);
bool isEven(int8_t num);

int8_t addBinaryDigits(int8_t num);

bool isSquare(int8_t num);

int8_t maxArray(int8_t* nums, uint8_t length);
uint8_t maxArrayUnsigned(uint8_t* nums, uint8_t length);

int8_t pow(int8_t a, int8_t b);
uint8_t powUnsigned(uint8_t a, uint8_t b);

void itoa(int8_t num, char* buf);
void itoaUnsigned(uint8_t num, char* buf);

#endif
