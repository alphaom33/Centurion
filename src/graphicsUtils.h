#ifndef graphics_utils_h
#define graphics_utils_h

#define CENTER_X GFX_LCD_WIDTH / 2
#define CENTER_Y GFX_LCD_HEIGHT / 2

#define TEXT_WIDTH 8
#define TEXT_HEIGHT 8
#define HALF_TEXT_WIDTH (TEXT_WIDTH / 2)
#define HALF_TEXT_HEIGHT (TEXT_HEIGHT / 2)

void global();
void selected();

void menuGlobal();
void menuSelected();

#endif