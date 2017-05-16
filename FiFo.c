// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/10/2017 
// Student names: Silly Kenneth, Silly Kunpeng
// Last modification date: Silly Saturday Apr 15

#include <stdint.h>
#define FIFO_SIZE 65
int32_t static PutI; 
int32_t static GetI;
int32_t Fifo[FIFO_SIZE]; 

// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void FiFo_Init() {
  PutI = GetI = FIFO_SIZE - 1; 
}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(int32_t data)
{
  if (((PutI - 1) == GetI) | (((PutI) + (FIFO_SIZE - 1)) == GetI)) return 0;
  Fifo[PutI] = data;
  PutI--;
  if (PutI < 0) PutI = FIFO_SIZE - 1;
  return 1;
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(int32_t *datapt)
{
  if (GetI == PutI) return 0;
  *datapt = Fifo[GetI];
  GetI--;
  if (GetI < 0) GetI = FIFO_SIZE-1;
  return 1;
}



