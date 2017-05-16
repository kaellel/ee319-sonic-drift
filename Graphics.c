/*******************************************
 * Graphics.c
 * Designed and written by Kenneth Lin, Kunpeng Qin 
 *
 * Implements a double buffered system. 
 *
 * See descriptor file for more information. 
 *******************************************/

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "IO.h"
#define E_COLOR 0x1234
#define GRAPHICS_WIDTH  128
#define GRAPHICS_HEIGHT 160
#define TRANSPARENT 0xA254

#define Graphics_Buffer_SIZE   10240
#define PALETTE_SIZE           5120
#define COLOR_POSSIBILITIES    16

static uint8_t Graphics_Buffer[Graphics_Buffer_SIZE];
static uint16_t Graphics_Palette[PALETTE_SIZE];
static uint8_t Graphics_BufferWrite; 

/*******************************************
 * Graphics_DrawPixel
 * Places a pixel onto the graphics buffer. 
 *******************************************/
void Graphics_DrawPixel(int16_t x, int16_t y, uint16_t color){
  uint32_t Cell_X,                   // Cell_X is used in calculating the Cell Number.  
           Cell_Y,                   // Cell_Y is used in calculating the Cell Number.  
           Inner_X,                  // Inner_X is the x-coordinate in the Cell, taking 0 - 7.  
           Inner_Y,                  // Inner_Y is the y-coordinate in the Cell, taking 0 - 7.  
           Color_Palette_Num,        // Color_Palette_Num is the index of the Palette array for the corresponding Color Cell. 
           Color_Palette_Value = 0;  // Color_Palette_Value is the 0-15 value of color. 
  uint32_t Cell_Num;                 // Cell_Num is the index of the Color Cell in the Graphics_Buffer array. 
  uint16_t counter, counter2;        // counter is used in loops. 
  if (x < 0 || y < 0 || x > 127 || y > 159) return;      // If the pixel is out of bounds, it can be safely ignored. 
  Graphics_BufferWrite = 1;                              // Since the buffer is being written to, the flag should be set. 
  Cell_X = x >> 3;                                       // Calculates Color Cell the (x, y) is in. This is equivalent to x / 8. 
  Cell_Y = y >> 3;                                       // Calculates Color Cell the (x, y) is in. This is equivalent to y / 8. 
  Cell_Num = (Cell_Y << 4) + Cell_X;                     // Calculates Color Cell the (x, y) is in. This is Cell_Y * 16 + Cell_X. 
  Inner_X = x & 7;                                       // Calculates Inner_X. This is x % 8. 
  Inner_Y = y & 7;                                       // Calculates Inner_Y. This is y % 8. 
  Color_Palette_Num = COLOR_POSSIBILITIES * Cell_Num;    // Calculates Color_Palette_Num. 
  uint32_t t1, t2, t3;                          // Some variables for calculations. 
  t1 = (Inner_X + (Inner_Y << 3));              // t1 represents InnerX + InnerY * 8. It is the index of the pixel in the 8 by 8 cell. 
  t2 = (t1 & 1) << 2;                           // t2 is (t1 % 2) * 4. It is used to mask the shift the mask around when putting bits into the lower or top 4 bits of the buffer byte. 
  t3 = (Cell_Num << 5) + (t1 >> 1);             // t3 is Cell_Num * 32 + t1 / 2. It used to calculate which index of Graphics_Buffer to store the color information into.  
  while (Color_Palette_Value < 16) {                                                    // See if the color is already in the Palette. 
    if (Graphics_Palette[Color_Palette_Num + (++Color_Palette_Value)] == color) break;  // If the color is already in Palette[n], then simply place that index into the buffer. 
  }                                                                                     // Move along swiftly, said the Stormtrooper. 
  if (Color_Palette_Value == 16) {                                                      // If the color is not in the palette, then try to add it.   
    Color_Palette_Value = 0;                                                            // Reset the detection. 
    while (Color_Palette_Value < 16) {                                                  // Look for empty spots. 
      if (Graphics_Palette[Color_Palette_Num + (++Color_Palette_Value)] == E_COLOR){    // Check empty. 
        Graphics_Palette[Color_Palette_Num + Color_Palette_Value] = color;              // Add color to empty palette. 
        break;                                                                          // We're done looking for the color. 
      }                                                                                 // Move along swiftly. 
    }                                                                                   // These aren't the droids you're looking for. 
    if (Color_Palette_Value == 16) {                                                    // All slots in the palette have been taken. 
      int32_t differences[16];                                                          // The differences array is used to store the differences between palette colors and current color. 
      for (counter = 0; counter < 16; counter++) {                                      // Find the color in palette that is closest to the color. 
        int32_t tmp = (Graphics_Palette[Color_Palette_Num + counter] - color);          // Store the differences. 
        differences[counter] = tmp;                                                     // Store the differences. 
        if (tmp < 0) differences[counter] = 0 - tmp;                                    // We want to get rid of signs. 
      }                                                                                 // Move along. 
      uint8_t flags = 0;                                                                // This will be used to find the smallest difference. 
      for (counter = 0; counter < 16; counter++) {                       // Cycle thru each difference. 
        flags = 0;                                                       // Assume that this difference is the least. 
        for (counter2 = 0; counter2 < 16; counter2++) {                  // Cycle thru all other differences. 
          if (counter == counter2) continue;                             // Don't compare to itself. 
          if (differences[counter] > differences[counter2]) flags = 1;   // If the current difference is not the smallest difference, then this isn't it. 
          break;                                                         // Break out if this difference is not it. 
        }                                       // Nothing to see here. 
        if (flags == 0) break;                  // If we found the smallest difference, then that is it. 
      }                                         // Nothing to see here. 
      Color_Palette_Value = counter;            // The color value is the closest color currently in the palette.                                         
    }                                           // Move along.                                                                       
  }                                             // This chunk stores the pixel information into the Graphics_Buffer. 
  Graphics_Buffer[t3] &= ~ ((0xF) << t2);       // Clear the bits corresponding to (x, y) in the Graphics_Buffer. 
  Color_Palette_Value <<= t2;                   // Move the color information to the correct spot. 
  Graphics_Buffer[t3] |= Color_Palette_Value;   // Store the color into the Graphics_Buffer. 
  Graphics_BufferWrite = 0;                     // Everything is done, so clear the Graphics_Buffer write flag. 
}

/*******************************************
 * Graphics_Init
 * Clears the color palette.  
 *******************************************/
void Graphics_Init (void){
  Graphics_BufferWrite = 0;         // Clear the write flag. 
  uint16_t i = 0;                   // Counter used to clear the color palette. 
  for(;i<PALETTE_SIZE;i++)          // Clear the color palette. 
    Graphics_Palette[i] = E_COLOR;  // Clear the color palette. 
}

/*******************************************
 * Graphics_DrawBitmap
 * Draws a bitmap image.   
 *******************************************/
void Graphics_DrawBitmap(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h){
  uint16_t i, j;                                           // Counters used. 
  for (i = 0; i < h; i++)                                  // Cycle thru the vertical axis. 
    for (j = 0; j < w; j++)                                // Cycle thru the horizontal axis. 
      Graphics_DrawPixel(x + j, y - i, image[i * w + j]);  // Draw each pixel. 
}

/*******************************************
 * Graphics_OutScreen
 * Offloads the buffer in RAM onto the
 * display RAM, swapping the double buffers. 
 *******************************************/
void Graphics_OutScreen(uint8_t ForceRedraw){  
  ST7735_SetScreen();                           // Set the Screen RAM area to be changed. 
  IO_HeartBeat2();                              // Green heartbeat. 
  if (Graphics_BufferWrite == 1) return;        // If the Graphics_Buffer is not done writing, then do not output to screen. 
  uint16_t Cell_X = 0,                          // Cell_X is the Color Cell x-number, from 0 to 15.   
           Cell_Y = 0,                          // Cell_Y is the Color Cell y-number, from 0 to 19. 
           Inner_X = 0,                         // Inner_X is the pixel x within each cell, from 0 to 7. 
           Inner_Y = 0;                         // Inner_Y is the pixel y within each cell, from 0 to 7. 
  uint32_t t1, t2, t3;                          // Some variables for calculations. 
  uint32_t Cell_Num;                            // Some variables for calculations. 
  uint32_t X, Y;                                // Some counter variables. 
  for (Y = 0; Y < 160; Y++)                     // Cycle thru Color Cells.                               
    for (X = 0; X < 128; X++) {                 // Cycle thru Color Cells.            
      Inner_X = X & 7;                          // Calculates Inner_X. This is x % 8. 
      Inner_Y = Y & 7;                          // Calculates Inner_Y. This is y % 8. 
      Cell_X = X >> 3;                          // Calculates Color Cell the (x, y) is in. This is equivalent to x / 8. 
      Cell_Y = Y >> 3;                          // Calculates Color Cell the (x, y) is in. This is equivalent to y / 8. 
      Cell_Num = (Cell_Y << 4) + Cell_X;        // Calculate which cell we're at. 
      t1 = (Inner_X + (Inner_Y << 3));          // t1 represents InnerX + InnerY * 8. It is the index of the pixel in the 8 by 8 cell. 
      t2 = (t1 & 1) << 2;                       // t2 is ((t1 % 2)) * 4. It is used to mask the shift the mask around when putting bits into the lower or top 4 bits of the buffer byte. 
      t3 = (Cell_Num << 5) + (t1 >> 1);         // t3 is Cell_Num * 32 + t1 / 2. It used to calculate which index of Graphics_Buffer to store the color information into. 
      uint8_t Color_Palette_Value =             // Extract the 0-15 color information for the pixel now looped at. 
        ((Graphics_Buffer[t3] &                 // Look up the Graphics_Buffer. 
        (0xF) << t2))                           // Mask out the pixel information. 
        >> t2;                                  // Shift the pixel information to the correct place. 
      uint16_t color_out = Graphics_Palette[(Cell_Num << 4) + Color_Palette_Value];     // Get the color information from the Color Cell's palette. 
      ST7735_DrawPixel2(X, Y, color_out);       // Repaint the pixel. 
    }                                           // Nothing to see here. 
  IO_HeartBeat2();                              // Green heartbeat. 
}

/***********************************************
 * Graphics_FillRect
 * Draws a rectangle. 
 **********************************************/
void Graphics_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  int16_t i = 0;                                  // Counter used. 
  int16_t j = 0;                                  // Counter used. 
  for (i = 0; i < w; i ++)                        // Cycle thru the horizontal axis. 
    for (j = 0; j < h; j ++)                      // Cycle thru the vertical axis. 
      Graphics_DrawPixel(i + x, j + y, color);    // Draw the pixel. 
}



