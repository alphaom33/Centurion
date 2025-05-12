#ifndef bomb_stuff_h
#define bomb_stuff_h

#include "stdint.h"
#include "stdbool.h"

#define NUM_PORTS 6
#define SERIAL_LENGTH 6

typedef struct {
    char name[4];
    bool on;
    Indicator* next;
} Indicator;

typedef enum {
    DVID,
    PARALLEL,
    PS2,
    RJ45,
    SERIAL,
    STEREO_RCA
} Port;

typedef struct {
    char serial[SERIAL_LENGTH + 1];
    uint8_t numBatteries;
    uint8_t numBatteryHolders;
    Indicator indicatorHead;
    bool ports[NUM_PORTS];
} BombStuff;

extern BombStuff* bombStuff;

void initBombStuff();
void initBombStuffSetter(void** data);
void procBombStuff(void* data);

#endif