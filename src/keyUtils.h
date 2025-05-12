#ifndef key_utils_h
#define key_utils_h

#include "keypadc.h"

#define getKey(datum, key) (datum[(key) >> 8] & (key))

bool getKeyDown(kb_lkey_t key);
void copyLast();
uint8_t getLetter();
uint8_t getNumber();

void checkAlpha();
bool getAlphaed();
uint8_t getKeypad();

#endif