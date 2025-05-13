#include "keyUtils.h"
#include "keypadc.h"

uint8_t kb_LastData[20];
bool alphaed = false;

bool getKeyDown(kb_lkey_t key) {
    return getKey(kb_Data, key) && !getKey(kb_LastData, key);
}

void copyLast() {
    for (int i = 0; i < 8; i++) {
        kb_LastData[i] = kb_Data[i];
    }
}

bool getAlphaed() {
    return alphaed;
}

void checkAlpha() {
    if (getKeyDown(kb_KeyAlpha)) alphaed = !alphaed;
}

uint8_t getKeypad() {
    if (getAlphaed()) {
        if (getLetter() != UINT8_MAX) return getLetter();
    } else { // technically this could have been else if but I like consistent formatting okay
        if (getNumber() != UINT8_MAX) return getNumber() + '0';
    }

    return UINT8_MAX;
}

uint8_t getLetter() {
    if (getKeyDown(kb_KeyMath)) return 'A';
    else if (getKeyDown(kb_KeyApps)) return 'B';
    else if (getKeyDown(kb_KeyPrgm)) return 'C';
    else if (getKeyDown(kb_KeyRecip)) return 'D';
    else if (getKeyDown(kb_KeySin)) return 'E';
    else if (getKeyDown(kb_KeyCos)) return 'F';
    else if (getKeyDown(kb_KeyTan)) return 'G';
    else if (getKeyDown(kb_KeyPower)) return 'H';
    else if (getKeyDown(kb_KeySquare)) return 'I';
    else if (getKeyDown(kb_KeyComma)) return 'J';
    else if (getKeyDown(kb_KeyLParen)) return 'K';
    else if (getKeyDown(kb_KeyRParen)) return 'L';
    else if (getKeyDown(kb_KeyDiv)) return 'M';
    else if (getKeyDown(kb_KeyLog)) return 'N';
    else if (getKeyDown(kb_Key7)) return 'O';
    else if (getKeyDown(kb_Key8)) return 'P';
    else if (getKeyDown(kb_Key9)) return 'Q';
    else if (getKeyDown(kb_KeyMul)) return 'R';
    else if (getKeyDown(kb_KeyLn)) return 'S';
    else if (getKeyDown(kb_Key4)) return 'T';
    else if (getKeyDown(kb_Key5)) return 'U';
    else if (getKeyDown(kb_Key6)) return 'V';
    else if (getKeyDown(kb_KeySub)) return 'W';
    else if (getKeyDown(kb_KeySto)) return 'X';
    else if (getKeyDown(kb_Key1)) return 'Y';
    else if (getKeyDown(kb_Key2)) return 'Z';
    else return UINT8_MAX;
}

uint8_t getNumber() {
    if (getKeyDown(kb_Key0)) return 0;
    else if (getKeyDown(kb_Key1)) return 1;
    else if (getKeyDown(kb_Key2)) return 2;
    else if (getKeyDown(kb_Key3)) return 3;
    else if (getKeyDown(kb_Key4)) return 4;
    else if (getKeyDown(kb_Key5)) return 5;
    else if (getKeyDown(kb_Key6)) return 6;
    else if (getKeyDown(kb_Key7)) return 7;
    else if (getKeyDown(kb_Key8)) return 8;
    else if (getKeyDown(kb_Key9)) return 9;
    else return UINT8_MAX;
}