#ifndef screen_h
#define screen_h

#include "graphx.h"

typedef void (*Init)(void** data);
typedef void (*Proc)(void* data);

typedef struct {
    const char* name;
    gfx_sprite_t* image1;
    gfx_sprite_t* image2;
    Init init;
    Proc proc;
} Screen;

#endif