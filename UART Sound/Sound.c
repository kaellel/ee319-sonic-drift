// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on TM4C123
// Program written by: Kenneth Lin, Kunpeng Qin
// Date Created: 3/6/17 
// Last Modified: 3/12/17 
// Lab number: 6

#include <stdint.h>
#include "dac.h"
#include "tm4c123gh6pm.h"

// Waveform for 6bits DAC. Changing this waveform is changing the instrument. 
const int32_t sin64_32pts[32] = {
  0,6,12,18,23,27,30,31,32,31,30,27,23,18,12,6,0,-5,-11,-17,-22,-26,-29,-30,-31,-30,-29,-26,-22,-17,-11,-5
};   
// Waveform for 6bits DAC. Piano wave below. 
const int32_t sin64_32pts_piano[32] = {
  -27,-16,-7,1,10,11,12,12,19,25,28,29,32,29,28,26,26,32,31,27,18,4,-1,-7,-13,-11,-18,-16,-19,-22,-28,-31
};  
// dB Table
const int32_t dBtable[90] = {
  5,30,60,90,100,100,100,100,100,100, 93,78,69,63,58,54,51,48,45,43,41,39,37,36,34,33,32,30,29,28,27,26,25,24,23,23,22,21,20,19,19,18,17,17,16,16,15,14,14,13,13,12,12,11,11,10,10,9,9,8,8,8,7,7,6,6,6,5,5,5,4,4,3,3,3,2,2,2,2,1,1,1,0,0,0,0,0,0,0,0
}; 
// The following array stores the periods of all piano notes divided by 32, from 0 to 108. 
// The periods are in units of microseconds (10^-6 seconds). 
// Every notePeriodTable[n] microseconds, the sine wave should shift the the next value to play the nth piano note. 
// There is aliasing for very high pitches. 
const uint32_t notePeriodTable[109] = {
  1911,1804,1703,1607,1517,1432,1352,1276,1204,1136,1072,1012,956,902,851,804,758,716,676,638,602,568,536,506,478,451,426,402,379,358,338,319,301,284,268,253,239,225,213,201,190,179,169,159,150,142,134,127,119,113,106,100,95,89,84,80,75,71,67,63,60,56,53,50,47,45,42,40,38,36,34,32,30,28,27,25,24,22,21,20,19,18,17,16,15,14,13,13,12,11,11,10,9,9,8,8,7,7,7,6,6,6,5,5,5,4,4,4
}; 

// ActiveNoteMaxiumum defines the number of notes allowed to be sounded at one time. 
uint8_t ActiveNoteMaxiumum = 2;
// MaxNoteEnvelope defines the length of a sound. 
// For envelope, use 0x3FFFF
// Else, use 0xFFFF
#define MaxNoteEnvelope 0x3FFFF 
uint32_t activeNotePeriods[4]; 
uint32_t activeNoteEnvelope[4]; 
uint32_t activeNoteCounter[4]; 
uint32_t activeNoteWaveLocation[4]; 
// ActiveNoteIndex is used to determine where the next noteOn should add a note to. 
uint8_t activeNoteIndex; 

uint32_t activeSingleNotePeriod = 0; 
uint32_t activeSingleNoteCounter = 0; 
uint32_t activeSingleNoteLocation = 0; 

void DAC_Out_6bits(uint32_t data); 
void DisableInterrupts(void);
void EnableInterrupts(void);
void Sound_Play(uint32_t period); 
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
  
extern uint8_t PianoMode; 

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Called once, with sound initially off
void Sound_Init(uint32_t period){
  NVIC_ST_CTRL_R = 0; 
  NVIC_ST_RELOAD_R = period - 1; 
  NVIC_ST_CURRENT_R = 0; 
  NVIC_SYS_PRI3_R = ((NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x4000000); 
  NVIC_ST_CTRL_R = 0x7; 
  activeNoteIndex = 0; 
}

void SysTick_Set_Period(uint32_t period){ 
  NVIC_ST_RELOAD_R = period - 1; 
}

// **************SysTick_Handler*********************
// Increments wave pointer, plays sound by outputting it into DAC. 
void SysTick_Handler(void) {
  Sound_Play(0); 
}

// **************Sound_Play*********************
// This is executed every SysTick.Tick. This updates the waveforms and outputs to the DAC.  
// This uses th 6 bits DAC. 
void Sound_Play(uint32_t period) {
  uint8_t count = 0; 
  // The following line sets DAC to output 1.5 V when no sound occurs. 
  uint32_t outputVoltage = 32; 
  
    // For each active note, update the note waveform pointer if the corresponding time has elapsed. 
    // Then, add up the waveforms to produce a final waveform. 
    for (count = 0; count < ActiveNoteMaxiumum; count++) {
      // Add to the active note's time counter. 
      // The below is plus 5 since the timer is only able to do 5 ms units. 
      if (activeNoteEnvelope[count] <= 0) continue; 
      
      activeNoteCounter[count] += 5; 
      // Check if we need to increment the sine wave location. 
      if (activeNoteCounter[count] >= activeNotePeriods[count]) {
        activeNoteCounter[count] = 0; 
        activeNoteWaveLocation[count] ++; 
        
        if (activeNoteWaveLocation[count] >= 31) {
          activeNoteWaveLocation[count] = 0; 
        }
      }
      // Decrement the envelope. 
      if (activeNoteEnvelope[count] != 0) activeNoteEnvelope[count]--; 
      // Add the note to the output, taking into account the envelope, looking up the dBtable. 
      if (activeNoteEnvelope[count] > 0) {
        // Add the signal to the output voltage. The expression 
        // "dBtable[89 - activeNoteEnvelope[count] / 0xCCC ] / 200"
        // Looks up the dBtable by dividing the envelope location by 80, since there are only
        // 80 elements in the lookup table. 
        outputVoltage += sin64_32pts[activeNoteWaveLocation[count]]  
        // For the below line, comment out 100 to activate the envelope, and comment out
        // dBtable[79 - activeNoteEnvelope[count] / 0x182 ] to deactivate the envelope. 
        // Also, change the vaue of MaxNoteEnvelope
        * /*100/ */ dBtable[89 - activeNoteEnvelope[count] / 0xb60 ] / 300; 
      }
    }
    // After updating notes and generating the waveform, output to DAC. 
    DAC_Out_6bits(outputVoltage); 
  
}

// **************noteOn*********************
// Adds a note to the active notes. 
// noteNumber should follow MIDI numbering conventions. 
// noteOn does not play the note; the SysTick Handler calls Sound_Play(), which plays the notes. 
void noteOn(uint32_t noteNumber){
  long sr;
  sr = StartCritical();
  // Convert the MIDI compliant note number into a note recognized by the period array. 
  noteNumber -= 9; 
  // Add the note to active notes. 
  activeNotePeriods[activeNoteIndex] = notePeriodTable[noteNumber]; 
  activeNoteCounter[activeNoteIndex] = 0; 
  activeNoteWaveLocation[activeNoteIndex] = 0; 
  activeNoteEnvelope[activeNoteIndex] = MaxNoteEnvelope; 
  activeNoteIndex++; 
  // Do not write outside of the array. 
  if (activeNoteIndex == ActiveNoteMaxiumum) activeNoteIndex = 0; 
  EndCritical(sr);
}

void noteOn_Single(uint32_t noteNumber){
  long sr;
  sr = StartCritical();
  noteNumber -= 9;

  activeSingleNotePeriod = notePeriodTable[noteNumber]; 
  activeSingleNoteCounter = 0; 
  activeSingleNoteLocation = 0; 
  
  EndCritical(sr);
}

void noteOff_Single(uint32_t noteNumber) {
  long sr;
  sr = StartCritical();
  
  noteNumber -= 9;

  if (activeSingleNotePeriod == notePeriodTable[noteNumber])
    activeSingleNotePeriod = 0; 
  
  EndCritical(sr);
}

