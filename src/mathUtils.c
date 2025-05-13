#include "mathUtils.h"

uint8_t wrap(uint8_t num, uint8_t max) {
    return (num % max + max) % max;
}