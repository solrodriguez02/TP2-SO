#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdlib.h>

#define ADRESS_MEM_FOR_MMANAGER 0x300000
#define ADRESS_MEM_FOR_USER 0X710000
#define MEM_AVAILABLE 1048576	// byTes DISPO PARA EL USER, 2^20

#define BLOCK_SIZE 1024 // byTes
#define NUM_BLOCKS MEM_AVAILABLE/BLOCK_SIZE
#define FREE_BLOCK 0
#define USED_BLOCK 1
#define BOUNDARY_BLOCK 2

typedef struct MemoryManagerCDT *MemoryManagerADT;


// podria en reali elegir desde la fun el espacio / mem para el mem man, osea memoryForMemoryManager es inne
MemoryManagerADT createMemoryManager();

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate);


void freeMemory(MemoryManagerADT const restrict memoryManager, void *const restrict memoryToFree );

unsigned int getOccupiedMemory(MemoryManagerADT const restrict memoryManager);

unsigned int getTotalMemory(MemoryManagerADT const restrict memoryManager);

unsigned int getFreeMemory(MemoryManagerADT const restrict memoryManager);


#endif
