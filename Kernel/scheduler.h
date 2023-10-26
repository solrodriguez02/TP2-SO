#include <stdint.h>


void initializeScheduler();

void * scheduler(void * stackPointer);

int addToScheduler(void * stackPointer);