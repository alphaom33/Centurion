#ifndef bomb_stuff_h
#define bomb_stuff_h

#include "stdint.h"
#include "stdbool.h"

#define NUM_PORTS 6
#define SERIAL_LENGTH 6
#define NUM_INDICATORS 11

typedef enum {
    SND,
    CLR,
    CAR,
    IND,
    FRQ,
    SIG,
    NSA,
    MSA,
    TRN,
    BOB,
    FRK
} IndicatorNum;

typedef struct {
    bool on;
    bool exists;
	char name[4];
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
    Indicator indicators[NUM_INDICATORS];
    bool ports[NUM_PORTS];
} BombStuff;

extern BombStuff* bombStuff;

void initBombStuff();
void initBombStuffSetter(void** data);
void procBombStuff(void* data);

uint8_t getLastSerialNumber();
uint8_t getLargestSerialNumber();

bool indicatorInSerial(Indicator indicator);

#endif
