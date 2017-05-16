// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5
#include <stdint.h>
#include "FiFo.h"
#include "UART.h"
#include "tm4c123gh6pm.h"

#define PF3       (*((volatile uint32_t *)0x40025020))

uint32_t DataLost; 
  
// Initialize UART1
// Baud rate is 115200 bits/sec
// Make sure to turn ON UART1 Receiver Interrupt (Interrupt 6 in NVIC)
// Write UART1_Handler
void UART1_Init(void){
  // Basic . 
  SYSCTL_RCGCUART_R |= 0x2; 
  
  SYSCTL_RCGCGPIO_R |= 0x4; 
  

  UART1_CTL_R &= ~0x00000001;    // disable UART
  UART1_IBRD_R = 43;     // IBRD = int(80,000,000/(16*115,200)) = int(43.40278)
  UART1_FBRD_R = 26;     // FBRD = round(0.40278 * 64) = 26
  UART1_LCRH_R = 0x00000070;  // 8 bit, no parity bits, one stop, FIFOs
  
  GPIO_PORTC_AFSEL_R |= 0x30;    // enable alt funct on PC5-4
  GPIO_PORTC_DEN_R |= 0x30;      // configure PC5-4 as UART1
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
  GPIO_PORTC_AMSEL_R &= ~0x30;   // disable analog on PC5-4
  
  
  //UART1_CTL_R |= 0x1;
  
  
  // Interrupt .
  UART1_IM_R |= UART_IM_RXIM; 
  
  UART1_IFLS_R |= 0x2 <<3; 
  
  UART1_CTL_R |= 0x1; 
  
  NVIC_PRI1_R |= 0x2 << 21; 
  
  NVIC_EN0_R = NVIC_EN0_INT6; 
  
  return; 
}

// input ASCII character from UART
// spin if RxFifo is empty
char UART1_InChar(void){ 
  while ((UART1_FR_R & 0x10) != 0); 
  return (char) (UART1_DR_R & 0xFF); 
}
//------------UART1_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART1_OutChar(char data){
  while ((UART1_FR_R & 0x20) != 0); 
  UART1_DR_R = data; 
}

uint32_t RxCounter = 0; 

// hardware RX FIFO goes from 7 to 8 or more items
// UART receiver Interrupt is triggered; This is the ISR
void UART1_Handler(void){
  uint8_t i = 0; 
  char GetC; 
  
  PF3 ^= 0x8; 
  PF3 ^= 0x8; 
  
  for (; i < 8; i++) {
    GetC = UART1_InChar(); 
    FiFo_Put(GetC); 
  }
    
  RxCounter++; 
  
  // Acknowledge
  UART1_ICR_R = 0x10; 
  
  PF3 ^= 0x8; 
}
