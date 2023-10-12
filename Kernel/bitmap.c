#include "MemoryManager.h"



// no genera mucha fragmentacion externa? 
typedef struct MemoryManagerCDT {
    unsigned char bitmap[NUM_BLOCKS];
	//para el test
	char * user_adress;
} MemoryManagerCDT;


MemoryManagerADT createMemoryManager() {
	// creo array 
    MemoryManagerADT memoryManager = (MemoryManagerADT) ADRESS_MEM_FOR_MMANAGER;
	
	// para el test 
	memoryManager->user_adress = ADRESS_MEM_FOR_USER;

	for ( int i=0; i<NUM_BLOCKS; i++)
		memoryManager->bitmap[i] = FREE_BLOCK; 
	
	return memoryManager;
}

void *allocMemory(MemoryManagerADT const restrict memoryManager, const size_t memoryToAllocate) {
	
	if ( memoryToAllocate > MEM_AVAILABLE )
		return NULL;

	//1. REDONDEO A BLOQUE
	int requiredblocks = memoryToAllocate / BLOCK_SIZE + ((memoryToAllocate % BLOCK_SIZE) > 0 ? 1 : 0);

	//2. busco bloques libres
	int location = 0;
	int limit = NUM_BLOCKS - requiredblocks, i;
	while (location <= limit) {
		
		// Busco blocks libres
		for ( i = 0; i < requiredblocks; i++) {
			if ( memoryManager->bitmap[location + i] != FREE_BLOCK)
				break;
		}

		if (i == requiredblocks) {
			//char * allocation =  ADRESS_MEM_FOR_USER+ BLOCK_SIZE * location;

			//* para test
			char * allocation = memoryManager->user_adress + BLOCK_SIZE * location;

			// Mark the blocks as used
			memoryManager->bitmap[location++] = BOUNDARY_BLOCK;
			for ( i = 1; i < requiredblocks; i++){
				memoryManager->bitmap[location++] = USED_BLOCK;
			}

			return allocation;
		}
		
		location = location + i + 1;
	}

	// Allocation failed, return NULL
	return NULL;

}

void freeMemory(MemoryManagerADT const restrict memoryManager, void *const restrict memoryToFree ){

	if (memoryToFree == NULL || (char *) memoryToFree < memoryManager->user_adress)
		return;

	// Convert pointer to block index
	//int block = (int)(memoryToFree - ADRESS_MEM_FOR_USER) / BLOCK_SIZE;

	//* para test
	int block = (int)( (char *)memoryToFree - memoryManager->user_adress);

	// Ensure this is the start of the allocation
	if ( ( block % BLOCK_SIZE) != 0 )
		// el memoryToFree no es valido
		return;

	block /= BLOCK_SIZE;
	
	if ( memoryManager->bitmap[block] != BOUNDARY_BLOCK){
		//	perror("no se calculo bien el index del bloq");
		return;
	}
	
	// Mark blocks as free
	memoryManager->bitmap[block++] = FREE_BLOCK;
	while (block < NUM_BLOCKS && memoryManager->bitmap[block] == USED_BLOCK)	{
		memoryManager->bitmap[block++] = FREE_BLOCK;
	}	

}

static unsigned int getOccupiedBlocks(MemoryManagerADT const restrict memoryManager){
	int i=0, occupied=0;
	while ( i<NUM_BLOCKS ){
		if ( memoryManager->bitmap[i] != FREE_BLOCK )
			occupied++;
		i++;
	}
	return occupied;
}

// in bytes
unsigned int getOccupiedMemory(MemoryManagerADT const restrict memoryManager){
	return getOccupiedBlocks(memoryManager) *BLOCK_SIZE;
} 

unsigned int getTotalMemory(MemoryManagerADT const restrict memoryManager){
	return MEM_AVAILABLE;
}

unsigned int getFreeMemory(MemoryManagerADT const restrict memoryManager){
	return ( NUM_BLOCKS - getOccupiedBlocks(memoryManager) ) *BLOCK_SIZE;
} 