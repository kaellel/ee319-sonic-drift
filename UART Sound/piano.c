// Piano.c
// This software configures the off-board piano keys
// Runs TM4C123
// Program written by: Kenneth Lin, Kunpeng Qin
// Date Created: 3/6/17 
// Last Modified: 3/12/17 
// Lab number: 6

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "tm4c123gh6pm.h"

// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
// PE2-0 will be the input keys. 
#define PE_F 0x31
#define PE_2_0 0x7
#define PF_4 0x10
#define PF_2 0x4

uint32_t HeartBeatCount = 1; 

void Piano_Init(void){ 
  volatile uint8_t waiter = 0; 
  // First turn on the clock. 
  SYSCTL_RCGC2_R |= PE_F; 
  // Wait for clock to stabilize. 
  waiter++; waiter++; waiter++; 
  // Unlock PortF
  GPIO_PORTF_LOCK_R = 0x4C4F434B; 
  GPIO_PORTF_CR_R = 0x1F;
  // Set Direction Registers. 
  GPIO_PORTE_DIR_R &= ~PE_2_0;  
  GPIO_PORTF_DIR_R &= ~PF_4; 
  GPIO_PORTF_PUR_R |= PF_4;
  GPIO_PORTF_DIR_R |= PF_2; 
  GPIO_PORTA_DIR_R |= 0x4; 
  // Set Digital Enable Registers. 
  GPIO_PORTE_DEN_R |= PE_2_0; 
  GPIO_PORTF_DEN_R |= PF_4 | PF_2; 
  GPIO_PORTA_DEN_R |= 0x4; 
}

// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2
// PF_4 is the button that toggles between Piano and Play Song. 
uint32_t Piano_In(void){
  HeartBeatCount++; 
  if (HeartBeatCount == 0x8FFF) {
    GPIO_PORTF_DATA_R = ((GPIO_PORTF_DATA_R & 0x4) ^ 0x4); 
    HeartBeatCount = 0; 
  }
  return (GPIO_PORTE_DATA_R & PE_2_0) | ((((~GPIO_PORTF_DATA_R) & PF_4)) >> 1); 
}
