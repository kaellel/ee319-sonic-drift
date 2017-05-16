// IO.h
// This file has interface for IO module
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 6


#include "tm4c123gh6pm.h"
#include <stdint.h>
//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void);

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void);
void IO_HeartBeat2(void); 


//------------IO_Touch------------
// wait for release and touch of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void);

uint8_t IO_GetButton2(void);
uint8_t IO_GetButton1(void);
