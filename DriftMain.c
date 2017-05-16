/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "tm4c123gh6pm.h"
#include "TExaS.h"
#include "IO.h"
#include "Print.h"
#include "Graphics.h" 
#include "UART.h"

void drawCourse1(void); 
void initCourse1(uint8_t CHARACTER); 
void updateCourse1(void); 
void drawCourse2(void); 
void initCourse2(uint8_t CHARACTER); 
void updateCourse2(void); 
void UpdateGame(void); 
void ADC_Init(void); 
void Timer0A_Init(void(*task)(void), uint32_t period);
void menu1Update(void); 
void menu1Draw(void); 
void menu1Init(void); 
void menu2Init(void); 
void menu2Update(void); 
void menu2Draw(void); 

#define GAMEMODE_FIRSTMENU 0              // First Menu is select 1P or 2P. 
#define GAMEMODE_SECONDMENU 1             // Second Menu is select Sonic or Tails for 1P. 
#define GAMEMODE_THIRDMENU 5              // Third Menu is select Sonic or Tails for 2P. 
#define GAMEMODE_TRACK1 2                 // Racecourse 1. 
#define GAMEMODE_TRACK2 3                 // Racecourse 2. 
#define GAMEMODE_TRACK3 4                 // Racecourse 3. 
static uint8_t GAMEMODE = 0;              // Store the current state. 

// This function delays for count milliseconds. 
static void Delay(uint32_t count){
  uint16_t cnt; 
  uint16_t cnt2; 
  for (cnt = 0; cnt < count; cnt++) {
    for (cnt2 = 0; cnt2 < 0x2CF0; cnt2++ ){}
  } 
}

/************************
 Main: startup routine. 
 All game stuff is handled in timers. 
 ************************/
int main(void){  
  TExaS_Init();                               // Init routine.  
  UART1_Init();                               // Init routine.
  IO_Init();                                  // Init routine.  
  ADC_Init();                                 // Init routine.  
  ST7735_InitR(INITR_REDTAB);                 // Init routine.  
  ST7735_FillScreen(0xFFFF);                  // Init routine.  
  Timer0A_Init(&UpdateGame, 14000000);        // Init routine.  
  while (1);                                  // Infinitely loop.   
}

/************************
 ChangeState: a global function that changes the game engine state.  
 State changes are handled using this. 
 ************************/
void ChangeState(uint8_t toWhat, uint8_t secondArgs) {
  GAMEMODE = toWhat;                          // Change the state index. 
  switch (toWhat) {                           // There are specific initializations for some state changes. 
    case GAMEMODE_FIRSTMENU:                  // Changing to the first menu. 
      menu1Init();                            // Initialize the menu. 
      break;                                  // Break. 
    case GAMEMODE_SECONDMENU:                 // Changing to the second menu. 
      menu2Init();                            // Initialize the menu.
      break;                                  // Break. 
    case GAMEMODE_THIRDMENU:                  // Changing to the third menu. 
      // <INSERT>
      break;                                  // Break. 
    case GAMEMODE_TRACK1:                     // Changing to the track 1. 
      initCourse1(secondArgs);                // Initialize the track. 
      break;                                  // Break.  
    case GAMEMODE_TRACK2:                     // Changing to the track 2.  
      initCourse2(secondArgs);                // Initialize the track. 
      break;                                  // Break.                                  // Break.  
    case GAMEMODE_TRACK3:                     // Changing to the track 3. 
      
      break;                                  // Break.  
  }
}

/************************
* SendSound: Sends sound to UART. 
 ************************/
void SendSound(char songIndex) {
  UART1_OutChar(0x12); 
  UART1_OutChar(0x34); 
  UART1_OutChar(songIndex);
  UART1_OutChar(songIndex); 
  UART1_OutChar(songIndex); 
  UART1_OutChar(songIndex);   
  UART1_OutChar(0x56); 
  UART1_OutChar(0x78); 
}

/************************
 UpdateGame: a periodically called function that serves as the game engine.   
 ************************/
void UpdateGame(void) {
  switch (GAMEMODE) {                         // See what state we're in. 
    case GAMEMODE_FIRSTMENU:                  // In the first menu.
      Graphics_Init();                        // Clear the graphics. 
      menu1Update();                          // Update the menu. 
      menu1Draw();                            // Draw the menu. 
      Graphics_OutScreen(0);                  // Graphics draw the menu.
      break;                                  // Break. 
    case GAMEMODE_SECONDMENU:                 // In the second menu. 
      Graphics_Init();                        // Clear the graphics. 
      menu2Update();                          // Update the menu. 
      menu2Draw();                            // Draw the menu. 
      Graphics_OutScreen(0);                  // Graphics draw the menu.
      break;                                  // Break. 
    case GAMEMODE_THIRDMENU:                  // In the third menu. 
      
      ST7735_SetCursor(0, 0); 
      ST7735_FillScreen(0); 
      ST7735_OutString("2P Not Implemented. ");
      ST7735_SetCursor(0, 2); 
      ST7735_OutString("Press Button 2 to");
      ST7735_SetCursor(0, 3);
      ST7735_OutString("return. ");
      IO_Touch(); 
      Delay(1000); 
      ChangeState(GAMEMODE_FIRSTMENU, 0);     // NOTE: Usually this will be called in MENU.C or OTHER.C. 
      
      break;                                  // Break.
    
    case GAMEMODE_TRACK1:                     // In the track 1. 
      Graphics_Init();                        // First clear the graphics. 
      updateCourse1();                        // Update the course. 
      drawCourse1();                          // Draw the course. 
      Graphics_OutScreen(0);                  // Graphics draw the course. 
      break;                                  // Break.  
    case GAMEMODE_TRACK2:                     // In the track 2.  
      Graphics_Init();                        // First clear the graphics. 
      updateCourse2();                        // Update the course. 
      drawCourse2();                          // Draw the course. 
      Graphics_OutScreen(0);                  // Graphics draw the course. 
      break;                                  // Break.                                  // Break.  
    case GAMEMODE_TRACK3:                     // In the track 3. 
      
      break;                                  // Break.  
  }
}

