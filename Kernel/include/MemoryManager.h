#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

#define ADRESS_MEM_FOR_MMANAGER 0x35A000 
#define ADRESS_MEM_FOR_USER 0X710000
#define MEM_AVAILABLE 1048576	                    /* 1 MB */ 

#define BLOCK_SIZE 0x1000                           /* 4096 = 4kb (como en kernel.c) */
#define NUM_BLOCKS MEM_AVAILABLE/BLOCK_SIZE         /* 256 */
#define FREE_BLOCK 0
#define USED_BLOCK 1
#define BOUNDARY_BLOCK 2


typedef struct MemoryManagerCDT *MemoryManagerADT;


void createMemoryManager();

void *allocMemory(const size_t memoryToAllocate);

void freeMemory(void *const restrict memoryToFree );

unsigned int getOccupiedMemory();

unsigned long getTotalMemory();

unsigned int getFreeMemory();

unsigned getState(char * state);

unsigned int getSizeMemBlock();
#endif
