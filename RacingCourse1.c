/*
  RacingCourse1.c
  Written by Kenneth Lin. 
  For EE 319K Lab 10. 
  
  Graphics inspired by Sonic Drift, by Yawackhary. 
  
  This file represents a simple Green Hill Zone track. 
*/

/***************************************************************************************
 Header Includes. 
 ***************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "Graphics.h"
#include "tm4c123gh6pm.h"
#include "IO.h"
#include "Print.h"
#include "Graphics.h" 
#include "TextDraw.h"
#include "Sprites.h"

/***************************************************************************************
 Structure Definitions. 
 ***************************************************************************************/
struct RoadDefinition {
  int32_t center_x; 
  uint8_t width; 
  uint8_t class_number; 
  uint8_t roadbarwidth; 
} typedef road_t; 

road_t RoadObject[53] = {
  {64000, 25, 0, 1},
  {64000, 27, 0, 1},
  {64000, 29, 1, 1},
  {64000, 31, 1, 1},
  {64000, 33, 1, 1},
  {64000, 35, 2, 1},
  {64000, 37, 2, 2},
  {64000, 39, 2, 2},
  {64000, 41, 2, 2},
  {64000, 42, 3, 2},
  {64000, 44, 3, 2},
  {64000, 46, 3, 2},
  {64000, 48, 3, 3},
  {64000, 50, 4, 3},
  {64000, 52, 4, 3},
  {64000, 54, 4, 3},
  {64000, 56, 4, 3},
  {64000, 58, 4, 3},
  {64000, 60, 4, 4},
  {64000, 62, 4, 4},
  {64000, 64, 5, 4},
  {64000, 66, 5, 4},
  {64000, 68, 5, 4},
  {64000, 70, 5, 4},
  {64000, 71, 5, 5},
  {64000, 73, 5, 5},
  {64000, 75, 5, 5},
  {64000, 77, 6, 5},
  {64000, 79, 6, 5},
  {64000, 81, 6, 5},
  {64000, 83, 6, 6},
  {64000, 85, 6, 6},
  {64000, 87, 6, 6},
  {64000, 89, 6, 6},
  {64000, 91, 6, 6},
  {64000, 93, 6, 6},
  {64000, 95, 6, 7},
  {64000, 97, 6, 7},
  {64000, 99, 6, 7},
  {64000, 100, 6, 7},
  {64000, 102, 7, 7},
  {64000, 104, 7, 7},
  {64000, 106, 7, 8},
  {64000, 108, 7, 8},
  {64000, 110, 7, 8},
  {64000, 112, 7, 8},
  {64000, 114, 7, 8},
  {64000, 116, 7, 8},
  {64000, 118, 7, 9},
  {64000, 120, 7, 9},
  {64000, 122, 7, 9},
  {64000, 124, 7, 9},
  {64000, 126, 7, 9}
}; 

struct MapDef {
  uint8_t x; 
  uint8_t y; 
} typedef map_t; 

/***************************************************************************************
 Minimap Coordinates. 
 ***************************************************************************************/
static map_t MapPoints[116] = {
  {69,63},
  {67,63},
  {65,63},
  {63,63},
  {61,63},
  {59,63},
  {57,63},
  {55,63},
  {53,63},
  {51,63},
  {49,63},
  {47,63},
  {45,63}, 
  {43,63},
  {41,63},
  {39,63},
  {37,63},
  {35,63},
  {33,63},
  {31,63},
  {29,63},
  {27,63},
  {25,63},
  {23,62},
  {21,62},
  {19,61},
  {17,61},
  {16,60},
  {15,59},
  {14,57},
  {14,55},
  {14,53},
  {14,51},
  {14,49},
  {16,46},
  {18,44},
  {20,42},
  {22,40},
  {25,38},
  {28,36},
  {30,34},
  {33,32},
  {35,30},
  {37,28},
  {40,26},
  {43,24},
  {45,22},
  {48,20},
  {50,18},
  {53,16},
  {56,14},
  {58,13},
  {61,12},
  {63,11},
  {65,11},
  {67,10},
  {69,10},
  {71,10},
  {73,9},
  {75,9},
  {77,9},
  {79,9},
  {81,9},
  {83,9},
  {85,9},
  {87,9},
  {89,9},
  {91,9},
  {93,9},
  {95,9},
  {96,10},
  {100,10},
  {102,11}, 
  {104,12},
  {106,13},
  {106,15},
  {107,17},
  {107,19},
  {107,21},
  {107,23},
  {107,25},
  {107,27},
  {107,29},
  {107,31},
  {107,33},
  {107,35},
  {107,37},
  {107,39},
  {107,41},
  {107,43},
  {107,45},
  {107,47},
  {107,49},
  {107,51},
  {107,53},
  {107,55},
  {105,57},
  {105,59}, 
  {103,61}, 
  {101,61},
  {99,63},
  {97,63},
  {95,63},
  {93,63},
  {91,63},
  {89,63},
  {87,63},
  {85,63},
  {83,63},
  {81,63},
  {79,63},
  {77,63},
  {75,63},
  {73,63},
  {71,63},
  {69,63}
}; 

/***************************************************************************************
 Subroutine Definitions. 
 ***************************************************************************************/
static void changeCourseDirection(uint8_t NewDirection); 
uint32_t ADC_In(void); 
void ChangeState(uint8_t toWhat, uint8_t secondArgs); 
void SendSound(char songIndex); 

/***************************************************************************************
 Racing Course Related Variables. 
 ***************************************************************************************/
static uint8_t velocity = 1;                          // The velocity is either 1 (moving) or 0. 
static uint32_t progress = 0;                         // Progress is ticked at every UpdateCourse if velocity > 0. 
static uint32_t time = 0;                             // Time is used to track the time elapsed. It is ticked every UpdateCourse
static uint32_t eggman_progress = 30;                 // Eggman_Progress tracks Eggman. 
#define GO_STRAIGHT 1                                 // Defining the road object's state. 
#define GO_LEFT 2                                     // Defining the road object's state. 
#define GO_RIGHT 3                                    // Defining the road object's state. 
#define MAINTAIN 0                                    // Defining the road object's state. 
static uint8_t TrackMode = GO_STRAIGHT;               // TrackMode stores the road's state. 
static uint32_t TurnProgress = 0;                     // TurnProgress isn't really used too much. 
static uint32_t countdown = 3;                        // Countdown is th 3..2..1.. Go!
static uint32_t ccountdown = 8;                       // Each countdown-- takes Ccountdown ticks. 
#define PLAYER_LEFT 0                                 // Defining a character's state. 
#define PLAYER_STRAIGHT 1                             // Defining a character's state. 
#define PLAYER_RIGHT 2                                // Defining a character's state.  
static uint8_t playerInput = PLAYER_STRAIGHT;         // playerInput is the player's state. 
#define CHARACTER_TAILS 1                             // Defining a character select. 
#define CHARACTER_SONIC 0                             // Defining a character select. 
static uint8_t CHARACTER_SELECT = CHARACTER_TAILS;    // Character selected by the user. 
static uint8_t isSonicDone = 0;                       // Used to see who wins. 
static uint8_t isEggmanDone = 0;                      // Used to see who wins. 
static uint8_t SonicFinAnimation = 0;                 // Tracks the player's finish animation. 
static uint8_t titleCard = 0;                         // Tracks the title card animation 
static uint8_t youWin = 0;                            // Who wins? 1 is you win, 0 is not you. 
static uint8_t drawResults = 0;                       // 1 means the race is over, and draw the results. 
static uint32_t parallax_X_pos = 140;                 // Parallax background setting. 

/***************************************************************************************
 initCourse1
 Resets the course. 
 ***************************************************************************************/
void initCourse1(uint8_t CHARACTER) {
  velocity = 1; progress = 0; time = 0; eggman_progress = 30; 
  TrackMode = GO_STRAIGHT; TurnProgress = 0; countdown = 3; 
  ccountdown = 8; playerInput = PLAYER_STRAIGHT; 
  CHARACTER_SELECT = CHARACTER; 
  isSonicDone = 0; isEggmanDone = 0; SonicFinAnimation = 0; titleCard = 0;  
  youWin = 0; drawResults = 0; parallax_X_pos = 140; 
}

/***************************************************************************************
 drawCourse1
 Paints the course onto the Graphics. 
 ***************************************************************************************/
void drawCourse1(void) {
  // Variables for traversal and output. 
  uint32_t i, y, x, ct = 0, color = 0, redcolor, ctx, index, e_index; 
  uint32_t normalized_Sonic, normalized_Eggman; 
  volatile uint32_t test, diff; 
  
  // Heartbeat
  IO_HeartBeat(); 
  
  if (titleCard < 15) {
    Graphics_FillRect(0, 0, 128, 160, 0x0000); 
    Graphics_DrawBitmap(14, 120, greenhill_title, 100, 80); 
    titleCard++; 
    
    return;
  }
  
  
  
  if (drawResults != 0) {
    Graphics_FillRect(0, 0, 128, 160, 0xFFFF); 
    if (youWin) {
      Graphics_DrawBitmap(11, 107, winner_you, 106, 54); 
    } else {
      Graphics_DrawBitmap(11, 107, winner_not_you, 106, 54); 
    }
    drawResults++; 
    if (drawResults > 20) ChangeState(3, CHARACTER_SELECT); 
    SendSound(4);
    return; 
  } 
  
  SendSound(2);
  
  // First draw the map on top half. ONLY if it has been updated.
  index = progress / 5; 
  e_index = eggman_progress / 6;
  Graphics_DrawBitmap(0, 79, Course1Map, 128, 80); 
  
  // Then draw the parallax. Course1Parallax 325 by 26
  // This means one must cut off some stuff. 
  for (y = 0; y < 26; y++) {
    i = y * 325 + parallax_X_pos; 
    for (x = 0; x < 128; x++) {
      Graphics_DrawPixel(x, (26 - y) + 80, Course1Parallax[i]); 
      ct ++; 
      i++; 
    }
  }
  
  // Then draw the ground repeatedly. It is 2 by 53. 
  for (i = 0; i < 128 / 2; i++) Graphics_DrawBitmap(i * 2, 159, Ground, 2, 53); 
  
  // Then draw the actual road. 
  for (i = 0; i < 53; i++) {
    color = 0xC618; 
    redcolor = 31; 
    ctx = RoadObject[i].center_x / 1000; 
    if (RoadObject[i].class_number % 2 == 0) color = 0xFFFF; 
    if (color == 0xC618) redcolor = 0xFFFF; 
    Graphics_FillRect(ctx - (RoadObject[i].width / 2), i + 107, RoadObject[i].width, 1, redcolor); 
    Graphics_FillRect(ctx - ((RoadObject[i].width - RoadObject[i].roadbarwidth) / 2) + 1, i + 107, RoadObject[i].width - RoadObject[i].roadbarwidth - 1, 1, color); 
  }
  
  // Draw the opponent if he can be seen. 
  normalized_Sonic = progress * 6; 
  normalized_Eggman = eggman_progress * 5; 
  diff = (normalized_Eggman - normalized_Sonic); 
  if (diff >= 0x55 && diff <= 0x145) {
    test = 4000 / (0x55 - 0x73) * ((normalized_Eggman - normalized_Sonic) - 0x73) / 1000 + 3; 
    for (y = 0; y < 10; y++) {
      i = y * 8; 
      for (x = 0; x < 8; x++) {
        if (eggman_farthest[i] != 0xCFF7) Graphics_DrawPixel(RoadObject[test].center_x/1000 - 4 + x, 107 + test - 5 + (10-y) - 4, eggman_farthest[i]); 
        ct ++; 
        i++; 
      }
    }
  }
  if (diff >= 0x38 && diff <= 0x55) {
    test = 23000 / (0x38 - 0x55) * ((normalized_Eggman - normalized_Sonic) - 0x55) / 1000 + 7; 
    for (y = 0; y < 15; y++) {
      i = y * 14; 
      for (x = 0; x < 14; x++) {
        if (eggman_farther[i] != 0xCFF7) Graphics_DrawPixel(RoadObject[test].center_x/1000 - 7 + x, 107 + test - 7 + (15-y) - 4, eggman_farther[i]); 
        ct ++; 
        i++; 
      }
    }
  }
  if (diff >= 0x1A && diff <= 0x38) {
    test = 20000 / (0x1A - 0x38) * ((normalized_Eggman - normalized_Sonic) - 0x38) / 1000 + 30; 
    for (y = 0; y < 23; y++) {
      i = y * 22; 
      for (x = 0; x < 22; x++) {
        if (eggman_closest[i] != 0xCFF7) Graphics_DrawPixel(RoadObject[test].center_x/1000 - 11 + x, 107 + test - 5 + (20-y) - 4, eggman_closest[i]); 
        ct ++; 
        i++; 
      }
    }
  }
  
  // Draw the car character
  if (isSonicDone == 0) {
    if (CHARACTER_SELECT == CHARACTER_SONIC) {
      if (playerInput == PLAYER_STRAIGHT) {
        for (y = 0; y < 24; y++) {
          i = y * 30; 
          for (x = 0; x < 30; x++) {
            if (sonic_c[i] != 0xCFF7) Graphics_DrawPixel(49 + x, (24 - y) + 125, sonic_c[i]); 
            ct ++; 
            i++; 
          }
        }
      } else if (playerInput == PLAYER_LEFT) {
        for (y = 0; y < 24; y++) {
          i = y * 30; 
          for (x = 0; x < 30; x++) {
            if (sonic_left[i] != 0xCFF7) Graphics_DrawPixel(49 + x, (24 - y) + 125, sonic_left[i]); 
            ct ++; 
            i++; 
          }
        }
      } else {
        for (y = 0; y < 24; y++) {
          i = y * 30; 
          for (x = 0; x < 30; x++) {
            if (sonic_right[i] != 0xCFF7) Graphics_DrawPixel(49 + x, (24 - y) + 125, sonic_right[i]); 
            ct ++; 
            i++; 
          }
        }
      }
    } else {
      if (playerInput == PLAYER_STRAIGHT) {
        for (y = 0; y < 24; y++) {
          i = y * 30; 
          for (x = 0; x < 30; x++) {
            if (tails_c[i] != 0xCFF7) Graphics_DrawPixel(49 + x, (24 - y) + 125, tails_c[i]); 
            ct ++; 
            i++; 
          }
        }
      } else if (playerInput == PLAYER_LEFT) {
        for (y = 0; y < 24; y++) {
          i = y * 30; 
          for (x = 0; x < 30; x++) {
            if (tails_left[i] != 0xCFF7) Graphics_DrawPixel(49 + x, (24 - y) + 125, tails_left[i]); 
            ct ++; 
            i++; 
          }
        }
      } else {
        for (y = 0; y < 24; y++) {
          i = y * 30; 
          for (x = 0; x < 30; x++) {
            if (tails_right[i] != 0xCFF7) Graphics_DrawPixel(49 + x, (24 - y) + 125, tails_right[i]); 
            ct ++; 
            i++; 
          }
        }
      }
    }
  } else {
    SonicFinAnimation++; 
    if (SonicFinAnimation < 3) {
      // Draw the closer. 
      if (CHARACTER_SELECT == CHARACTER_SONIC) {
        for (y = 0; y < 20; y++) {
          i = y * 16; 
          for (x = 0; x < 16; x++) {
            if (sonic_closer[i] != 0xCFF7) Graphics_DrawPixel(56 + x, (20 - y) + 120, sonic_closer[i]); 
            ct ++; 
            i++; 
          }
        }
      } else {
        for (y = 0; y < 20; y++) {
          i = y * 16; 
          for (x = 0; x < 16; x++) {
            if (tails_closer[i] != 0xCFF7) Graphics_DrawPixel(56 + x, (20 - y) + 120, tails_closer[i]); 
            ct ++; 
            i++; 
          }
        }
      }
    } else if (SonicFinAnimation < 6) {
      // Draw the farther. 
      if (CHARACTER_SELECT == CHARACTER_SONIC) {
        for (y = 0; y < 15; y++) {
          i = y * 14; 
          for (x = 0; x < 14; x++) {
            if (sonic_farther[i] != 0xCFF7) Graphics_DrawPixel(57 + x, (15 - y) + 110, sonic_farther[i]); 
            ct ++; 
            i++; 
          }
        }
      } else {
        for (y = 0; y < 15; y++) {
          i = y * 14; 
          for (x = 0; x < 14; x++) {
            if (tails_farther[i] != 0xCFF7) Graphics_DrawPixel(57 + x, (15 - y) + 110, tails_farther[i]); 
            ct ++; 
            i++; 
          }
        }
      }
    } else if (SonicFinAnimation < 9) {
      // Draw the farthest. 
      if (CHARACTER_SELECT == CHARACTER_SONIC) {
        for (y = 0; y < 10; y++) {
          i = y * 8; 
          for (x = 0; x < 8; x++) {
            if (sonic_farthest[i] != 0xCFF7) Graphics_DrawPixel(60 + x, (10 - y) + 100, sonic_farthest[i]); 
            ct ++; 
            i++; 
          }
        }
      } else {
        for (y = 0; y < 10; y++) {
          i = y * 8; 
          for (x = 0; x < 8; x++) {
            if (tails_farthest[i] != 0xCFF7) Graphics_DrawPixel(60 + x, (10 - y) + 100, tails_farthest[i]); 
            ct ++; 
            i++; 
          }
        }
      }
    } else if (SonicFinAnimation >= 9) {
      drawResults = 1; 
    }
  }
    
  // Draw the car map character
  // Draw character face
  if (CHARACTER_SELECT == CHARACTER_SONIC) {
    for (y = 0; y < 8; y++) {
      i = y * 8; 
      for (x = 0; x < 8; x++) {
        if (sonic_head[i] != 0xCFF7) Graphics_DrawPixel(MapPoints[index].x + x, (8 - y) + MapPoints[index].y - 2, sonic_head[i]); 
        ct ++; 
        i++; 
      }
    }
  } else {
    for (y = 0; y < 8; y++) {
      i = y * 8; 
      for (x = 0; x < 8; x++) {
        if (tails_head[i] != 0xCFF7) Graphics_DrawPixel(MapPoints[index].x + x, (8 - y) + MapPoints[index].y - 2, tails_head[i]); 
        ct ++; 
        i++; 
      }
    }
  }
  // Draw opponent face
  for (y = 0; y < 8; y++) {
    i = y * 8; 
    for (x = 0; x < 8; x++) {
      if (eggman_head[i] != 0xCFF7) Graphics_DrawPixel(MapPoints[e_index].x + x, (8 - y) + MapPoints[e_index].y - 2, eggman_head[i]); 
      ct ++; 
      i++; 
    }
  }
  
  // Draw time: 0:00
  uint32_t temp = 0;
  uint32_t ttime = time >> 3;   
  uint32_t dX, dY; 
  // Draw the first digit of time. 
  dX = 48; 
  dY = 78; 
  temp = ttime / 60; 
  for (y = 0; y < 7; y++) 
    for (x = 0; x < 8; x++) 
      if (ScoreFont[56*temp + 8 * y + x] != 0xCFF7) Graphics_DrawPixel(dX + x, dY - y, ScoreFont[56*temp+ 8 * y + x]); 
  // Draw the colon. 
  dX = 56; 
  dY = 78; 
  temp = 10; 
  for (y = 0; y < 7; y++) 
    for (x = 0; x < 8; x++) 
      if (ScoreFont[56*temp + 8 * y + x] != 0xCFF7) Graphics_DrawPixel(dX + x, dY - y, ScoreFont[56*temp+ 8 * y + x]); 
  // Draw the 2nd digit of time. 
  dX = 64; 
  dY = 78; 
  temp = ttime % 60 / 10; 
  for (y = 0; y < 7; y++) 
    for (x = 0; x < 8; x++) 
      if (ScoreFont[56*temp + 8 * y + x] != 0xCFF7) Graphics_DrawPixel(dX + x, dY - y, ScoreFont[56*temp+ 8 * y + x]); 
  // Draw the 3rd digit of time. 
  dX = 72; 
  dY = 78; 
  temp = ttime % 10; 
  for (y = 0; y < 7; y++) 
    for (x = 0; x < 8; x++) 
      if (ScoreFont[56*temp + 8 * y + x] != 0xCFF7) Graphics_DrawPixel(dX + x, dY - y, ScoreFont[56*temp+ 8 * y + x]); 
  
  // Draw the countdown
  if (countdown > 0) {
    dX = 60; 
    dY = 90;
    for (y = 0; y < 7; y++) 
      for (x = 0; x < 8; x++) 
        if (ScoreFont[56*countdown + 8 * y + x] != 0xCFF7) Graphics_DrawPixel(dX + x, dY - y, ScoreFont[56*countdown+ 8 * y + x]); 
  }
  
  // Heartbeat
  IO_HeartBeat(); 
}

/***************************************************************************************
 updateCourse1
 Paints the course onto the Graphics. 
 ***************************************************************************************/
void updateCourse1(void) {
  uint8_t i; uint32_t offset; uint32_t input;  
  static uint8_t toggleable = 0; 
  
  if (titleCard < 5) return; 
  
  if (countdown > 0) {
    ccountdown --; 
    if (ccountdown==0) {ccountdown = 8; countdown--; }
    return; 
  }
  
  // Update progress according to velocity. 
  progress += velocity; 
  
  time++; 
  
  // Update opponent progress.
  eggman_progress += 1; 
  
  // Check if course is done
  if (5*116 - 1 < progress) {
    progress = 0; 
    isSonicDone = 1; 
    if (isEggmanDone == 0) youWin = 1; 
  }
  if (6*116 - 1 < eggman_progress) {
    eggman_progress = 0; 
    isEggmanDone = 1; 
    if (isSonicDone == 0) youWin = 0; 
  }
  
  if (isSonicDone) {
    velocity = 0; 
    return; 
  }
  
  // Adjust course direction/mode/turn/corner according to the preprogrammed track.
  // First straight.   
  if (progress/5 < 24) {
    changeCourseDirection (MAINTAIN); 
  // First corner. 
  } else if (progress/5 >= 24 && progress/5 < 31) {
    changeCourseDirection (GO_RIGHT); 
  // First corner. 
  } else if (progress/5 >= 31 && progress/5 < 35) {
    changeCourseDirection (GO_STRAIGHT); 
  // Second corner. 
  } else if (progress/5 >= 45 && progress/5 < 49) {
    changeCourseDirection (GO_RIGHT);
  // Second corner. 
  } else if (progress/5 >= 49 && progress/5 < 53) {
    changeCourseDirection (GO_STRAIGHT); 
  // Third corner. 
  } else if (progress/5 >= 64 && progress/5 < 70) {
    changeCourseDirection (GO_RIGHT);
  // Third corner. 
  } else if (progress/5 >= 70 && progress/5 < 74) {
    changeCourseDirection (GO_STRAIGHT); 
  // Fourth corner. 
  } else if (progress/5 >= 92 && progress/5 < 98) {
    changeCourseDirection (GO_RIGHT);
  // Fourth corner. 
  } else if (progress/5 >= 98 && progress/5 < 102) {
    changeCourseDirection (GO_STRAIGHT); 
  } else {
    changeCourseDirection (MAINTAIN); 
  } 
  
  // Check user IO and move road accordingly. 
  // The parallax should also be moved. 
  input = ADC_In(); 
  if (input < 1040) {
    // Left turn. 
    playerInput = PLAYER_LEFT; 
    for (i = 0; i < 53; i++) RoadObject[i].center_x += 5000; 
    parallax_X_pos --; 
    
  } else if (input > 3030) {
    // Right turn.
    playerInput = PLAYER_RIGHT; 
    for (i = 0; i < 53; i++) RoadObject[i].center_x -= 5000; 
    parallax_X_pos ++; 
    
  } else {
    // Back to forward. 
    playerInput = PLAYER_STRAIGHT; 
  }
  
  // Check to see if car is off road. If it is, slow down. 
  // Check to see if car is on road. If it is, accelerate to terminal velocity. 
  if (RoadObject[35].center_x / 1000 - RoadObject[35].width / 2 > 64 + 8) 
    velocity = 0; 
  else if (RoadObject[35].center_x / 1000 + RoadObject[35].width / 2 < 64 - 8) 
    velocity = 0; 
  else
    velocity = 1; 
  
  // Check button IO. 
  if (IO_GetButton2()) {
    // Button 2 is reset the game button. 
    ChangeState(3, CHARACTER_SELECT); 
  }
  if (IO_GetButton1()) {
    // Button 1 is the slow down button.
    velocity = 0; 
  }
  
  // Finally, update the roads. 
  // Progress mod 500 to make things slower. 
  if (velocity > 0) {
    for (i = 0; i < 53; i++) {
      // The following makes the road go forward. 
      if (toggleable == 0) {
        RoadObject[i].class_number ++; 
      } else {
        RoadObject[i].class_number --; 
      }
      toggleable ^= 0x1; 
      
      // The following turns the road according to a mode. Test against TrackMode and see what to do. 
      switch (TrackMode) {
        case MAINTAIN:
          break; 
        case GO_LEFT: 
          offset = (53 - i)*(53 - i) + 5000; 
          if (TurnProgress >= 12) offset = 5000; 
          RoadObject[i].center_x -= offset; 
          if (i == 52) TurnProgress++; 
          break; 
        case GO_RIGHT:
          offset = (53 - i)*(53 - i) + 5000; 
          if (TurnProgress >= 12) offset = 5000; 
          RoadObject[i].center_x += offset; 
          if (i == 52) TurnProgress++; 
          break; 
        case GO_STRAIGHT: 
          // See if already straight or straight enough. 
          if (((RoadObject[i].center_x - 64000 < 5000) && (RoadObject[i].center_x - 64000 > 0)) 
              || 
              ((64000 - RoadObject[i].center_x < 5000) && (64000 - RoadObject[i].center_x > 0))) {
            RoadObject[i].center_x = 64000; 
          }
          if (RoadObject[i].center_x == 64000) break; 
          // Calculate offset. 
          offset = (53 - i) * (53 - i); 
          if (offset < 5000) offset = 5000; 
          // Adjust the road. 
          if (RoadObject[i].center_x > 64000) RoadObject[i].center_x -= offset; 
          if (RoadObject[i].center_x < 64000) RoadObject[i].center_x += offset; 
          break; 
      }
    }    
  }
}

static void changeCourseDirection(uint8_t NewDirection) {
  TrackMode = NewDirection; 
  TurnProgress = 0; 
}
