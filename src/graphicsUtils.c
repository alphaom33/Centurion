#include "graphicsUtils.h"
#include "graphx.h"

void global() {
    gfx_SetTextBGColor(1);
    gfx_SetTextFGColor(2);
    gfx_SetColor(1);
}

void selected() {
    gfx_SetTextBGColor(2);
    gfx_SetTextFGColor(1);
    gfx_SetColor(2);
}

void menuGlobal() {
    gfx_SetColor(2);
    gfx_SetTextFGColor(2);
    gfx_SetTextBGColor(1);
}

void menuSelected() {
    gfx_SetColor(2);
    gfx_SetTextFGColor(1);
    gfx_SetTextBGColor(2);
}