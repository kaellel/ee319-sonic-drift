// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: Kenneth Lin, Kunpeng Qin
// Date Created: N/A
// Last Modified: Mar. 23, 2017
// Lab number: 7


#include "tm4c123gh6pm.h"
#include <stdint.h>

// This function delays for count milliseconds. 
static void Delay(uint32_t count){
  uint16_t cnt; 
  uint16_t cnt2; 
  for (cnt = 0; cnt < count; cnt++) {
    for (cnt2 = 0; cnt2 < 0x2CF0; cnt2++ ){}
  } 
}

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// The switch is PF4 and the LED is PF2. 
// Input: none
// Output: none
#define PortF 0x20
#define PortF_LED 0x4
#define PortF_LED2 0x8
#define PortF_Switch 0x10
#define PortF_Switch2 0x1
void IO_Init(void) {
  volatile uint8_t waiter = 0; 
  // First turn on the clock. 
  SYSCTL_RCGC2_R |= PortF; 
  // Wait for clock to stabilize. 
  waiter++; waiter++; waiter++; 
  // Unlock PortF
  GPIO_PORTF_LOCK_R = 0x4C4F434B; 
  GPIO_PORTF_CR_R = 0x1F;
  // Set Direction Registers. 
  GPIO_PORTF_DIR_R |= PortF_LED + PortF_LED2;
  GPIO_PORTF_DIR_R &= ~PortF_Switch; 
  GPIO_PORTF_DIR_R &= ~PortF_Switch2; 
  GPIO_PORTF_PUR_R |= PortF_Switch;
  GPIO_PORTF_PUR_R |= PortF_Switch2;
  // Set Digital Enable Registers. 
  GPIO_PORTF_DEN_R |= PortF_LED + PortF_Switch + PortF_LED2 + PortF_Switch2; 
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
  GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R ^ PortF_LED;  
}

//------------IO_HeartBeat2------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat2(void) {
  GPIO_PORTF_DATA_R = GPIO_PORTF_DATA_R ^ PortF_LED2;  
}

//------------IO_Touch------------
// wait for release and press of the switch
// This is essentially a busy wait for switch, so that it waits for the user to toggle the images on the display in Lab7Main.c 
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
  // Wait for release; delay for 20ms; and then wait for press
  // First wait for release. Note: Negative Logic is used, i.e., a release is a 0. 
  while ((GPIO_PORTF_DATA_R & PortF_Switch)==0) {
  }
  // Then delay for 20 ms
  Delay(20); 
  // Finally wait for press
  while ((GPIO_PORTF_DATA_R & PortF_Switch)==PortF_Switch) {
  }
}  

uint8_t IO_GetButton2(void) {
  return (((~GPIO_PORTF_DATA_R) & PortF_Switch) >> 4); 
}

uint8_t IO_GetButton1(void) {
  return (((~GPIO_PORTF_DATA_R) & PortF_Switch2));
}


