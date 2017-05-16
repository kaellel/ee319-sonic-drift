
#include <stdint.h>
#include "tm4c123gh6pm.h"

void Graphics_Init (void); 

void Graphics_DrawBitmap(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h); 

void Graphics_OutScreen(uint8_t forceRedraw); 

void Graphics_DrawPixel(uint16_t x, uint16_t y, uint16_t color); 

void Graphics_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color); 
