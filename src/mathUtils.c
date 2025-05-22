#include "mathUtils.h"

const int8_t fibonaccis[] = {1, 1, 2, 3, 5, 8, 13, 21};

uint8_t wrap(uint8_t num, uint8_t max) {
    return (num % max + max) % max;
}

bool isInArray(int8_t num, int8_t* array, int8_t length) {
    for (int i = 0; i < length; i++) {
        if (num == array[i]) return true;
    }
    return false;
}

bool inFibonacci(int8_t num) {
    return isInArray(num, fibonaccis, 8);
}

int8_t nextFibonacci(int8_t num) {
    int i;
    for (i = 0; fibonaccis[i] != num; i++);

    return fibonaccis[i + 1];
}

bool isPrime(int8_t num) {
    const int8_t primes[] = {2, 3, 5, 7, 11, 13};
    return isInArray(num, primes, 6);
}

bool isEven(int8_t num) {
    return num % 2 == 0;
}

int8_t addBinaryDigits(int8_t num) {
    const int8_t binaries[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2};
    return binaries[num];
}

bool isSquare(int8_t num) {
	const int8_t squares[] = {1, 4, 9, 16, 25};
	return isInArray(num, squares, 5);
}

int8_t maxArray(int8_t* nums, uint8_t length) {
	int8_t max = nums[0];
	for (int i = 1; i < length; i++) {
		if (nums[i] > max) max = nums[i];
	}

	return max;
}

uint8_t maxArrayUnsigned(uint8_t* nums, uint8_t length) {
	int8_t max = nums[0];
	for (int i = 1; i < length; i++) {
		if (nums[i] > max) max = nums[i];
	}

	return max;
}
