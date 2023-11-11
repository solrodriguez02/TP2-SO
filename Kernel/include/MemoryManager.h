#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

#define ADRESS_MEM_FOR_MMANAGER 0x35A000 // 0x700000
#define ADRESS_MEM_FOR_USER 0X710000
#define MEM_AVAILABLE 1048576	// byTes DISPO PARA EL USER, 2^20, hasta 2^25 puedo tranquilamente, solo q voy a iterar + 

#define BLOCK_SIZE 0x1000 // byTes 2^10 ideal, ahora 4096 (como en kernel.c) 
#define NUM_BLOCKS MEM_AVAILABLE/BLOCK_SIZE     //256
#define FREE_BLOCK 0
#define USED_BLOCK 1
#define BOUNDARY_BLOCK 2


typedef struct MemoryManagerCDT *MemoryManagerADT;


void createMemoryManager();

void *allocMemory(const size_t memoryToAllocate);

void freeMemory(void *const restrict memoryToFree );

unsigned int getOccupiedMemory();

unsigned int getTotalMemory();

unsigned int getFreeMemory();


#endif
