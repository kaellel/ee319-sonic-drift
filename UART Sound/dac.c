// dac.c
// This software configures DAC output
// For the TM4C123
// Program written by: Kenneth Lin, Kunpeng Qin
// Date Created: 3/6/17 
// Last Modified: 3/9/17 
// Lab number: 6

#include <stdint.h>
#include "tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
#define PB 0x2
#define PB_3_0 0xF
void DAC_Init(void){
  volatile uint8_t waiter = 0; 
  // First turn on the clock. 
  SYSCTL_RCGC2_R |= PB; 
  // Wait for clock to stabilize. 
  waiter++; waiter++; waiter++; 
  // Set Direction Registers. 
  GPIO_PORTB_DIR_R |= PB_3_0;  
  // Set Digital Enable Registers. 
  GPIO_PORTB_DEN_R |= PB_3_0; 
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
#define PB_Data_3_0 (*((volatile uint32_t *) 0x4000503C))
void DAC_Out(uint32_t data){
  PB_Data_3_0 = data; 
}

// **************DAC_Init_6bits*********************
// Initialize 6-bit DAC, called once 
// Input: none
// Output: none
#define PB_5_0 0x3F
void DAC_Init_6bits(void){
  volatile uint8_t waiter = 0; 
  // First turn on the clock. 
  SYSCTL_RCGC2_R |= PB; 
  // Wait for clock to stabilize. 
  waiter++; waiter++; waiter++; 
  // Set Direction Registers. 
  GPIO_PORTB_DIR_R |= PB_5_0;  
  // Set Digital Enable Registers. 
  GPIO_PORTB_DEN_R |= PB_5_0; 
}

// **************DAC_Out_6bits*********************
// output to DAC
// Input: 6-bit data, 0 to 63
// Output: none
#define PB_Data_5_0 (*((volatile uint32_t *) 0x400050FC))
void DAC_Out_6bits(uint32_t data){
  PB_Data_5_0 = data; 
}

