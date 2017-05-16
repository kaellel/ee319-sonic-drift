// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: Kenneth Lin, Kunpeng Qin
// Last modification date: Apr. 5, 2017

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function PE4. 
// Input: none
// Output: none
// Code source: Textbook and class notes. 
void ADC_Init(void){ 
  volatile uint32_t delay; 
  SYSCTL_RCGCGPIO_R |= 0x10;
  // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10)
  == 0){};
  GPIO_PORTE_DIR_R &= ~0x2;
  // 2) make PE2 input
  GPIO_PORTE_AFSEL_R |= 0x2;
  // 3) enable alternate fun on
  // PE2
  GPIO_PORTE_DEN_R &= ~0x2;
  // 4) disable digital I/O on
  // PE2
  GPIO_PORTE_AMSEL_R |= 0x2;
  // 5) enable analog fun on PE2
  SYSCTL_RCGCADC_R |= 0x1;
  // 6) activate ADC0
  delay = SYSCTL_RCGCADC_R;
  // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
    // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  // extra time to stabilize
  delay = SYSCTL_RCGCADC_R;
  ADC0_PC_R = 0x01;
  // 7) configure for 125K
  ADC0_SSPRI_R = 0x3210;
  // 8) Seq 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;
  // 9) disable sample sequencer
  // 3
  ADC0_EMUX_R &= ~0xF000;
  // 10) seq3 is software trigger
  ADC0_SSMUX3_R =
  (ADC0_SSMUX3_R&0xFFFFFFF0)+1;
  // 11) Ain1 (PE2)
  ADC0_SSCTL3_R = 0x0006;
  // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;
  // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;
  // 14) enable sample sequencer
  // 3
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
  uint32_t data;
  ADC0_PSSI_R = 0x0008;
  while((ADC0_RIS_R&0x08)==0){};
  data = ADC0_SSFIFO3_R&0xFFF;
  ADC0_ISC_R = 0x0008;
  return data;
}


