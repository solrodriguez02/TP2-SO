// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <MemoryManager.h>


MemoryManagerADT memoryManager; 

typedef struct MemoryManagerCDT {
    unsigned char bitmap[NUM_BLOCKS];
	void * user_adress;
} MemoryManagerCDT;


void createMemoryManager() {
	
    memoryManager = (MemoryManagerADT) ADRESS_MEM_FOR_MMANAGER;
	memoryManager->user_adress = (void*) ADRESS_MEM_FOR_USER;

	for ( int i=0; i<NUM_BLOCKS; i++)
		memoryManager->bitmap[i] = FREE_BLOCK; 
	
}

void *allocMemory(const size_t memoryToAllocate) {
	
	if ( memoryToAllocate > MEM_AVAILABLE )
		return NULL;

	/* 1º Redondeo el bloque */
	int requiredblocks = memoryToAllocate / BLOCK_SIZE + ((memoryToAllocate % BLOCK_SIZE) > 0 ? 1 : 0);

	/* 2º Busco bloques libres */
	int location = 0;
	int limit = NUM_BLOCKS - requiredblocks, i;
	while (location <= limit) {
		
		for ( i = 0; i < requiredblocks; i++) {
			if ( memoryManager->bitmap[location + i] != FREE_BLOCK)
				break;
		}

		if (i == requiredblocks) {
			void * allocation = memoryManager->user_adress + BLOCK_SIZE * location;

			/* Marco los bloques como usados */
			memoryManager->bitmap[location++] = BOUNDARY_BLOCK;
			for ( i = 1; i < requiredblocks; i++){
				memoryManager->bitmap[location++] = USED_BLOCK;
			}

			return allocation;
		}
		
		location = location + i + 1;
	}

	return NULL;
}

void freeMemory(void *const restrict memoryToFree ){

	if (memoryToFree == NULL || memoryToFree < memoryManager->user_adress)
		return;

	/* Calculo indice del bloque */
	int block = (int)( memoryToFree - memoryManager->user_adress);

	/* Aseguro que sea el inicio de un bloque */
	if ( ( block % BLOCK_SIZE) != 0 )
		return;

	block /= BLOCK_SIZE;
	
	/* Aseguro que sea el bloque sea el inicio de una alocacion */
	if ( memoryManager->bitmap[block] != BOUNDARY_BLOCK){
		//	perror("no se calculo bien el index del bloq");
		return;
	}
	
	/* Marco los bloques como FREE */
	memoryManager->bitmap[block++] = FREE_BLOCK;
	while (block < NUM_BLOCKS && memoryManager->bitmap[block] == USED_BLOCK)	{
		memoryManager->bitmap[block++] = FREE_BLOCK;
	}	

}

static unsigned int getOccupiedBlocks(){
	int i=0, occupied=0;
	while ( i<NUM_BLOCKS ){
		if ( memoryManager->bitmap[i] != FREE_BLOCK )
			occupied++;
		i++;
	}
	return occupied;
}

/* Recupero la mem ocupada en bytes */
unsigned int getOccupiedMemory(){
	return getOccupiedBlocks(memoryManager) *BLOCK_SIZE;
} 

unsigned long getTotalMemory(){
	return MEM_AVAILABLE;
}

unsigned int getFreeMemory(){
	return ( NUM_BLOCKS - getOccupiedBlocks(memoryManager) ) *BLOCK_SIZE;
} 

unsigned getState(char * state){
	int i; 
	for (i = 0; i < NUM_BLOCKS; i++){
		state[i] = memoryManager->bitmap[i]+1;
	}
	state[i] = '\0';
	return getSizeMemBlock();
}

unsigned int getSizeMemBlock(){
	return BLOCK_SIZE;
}