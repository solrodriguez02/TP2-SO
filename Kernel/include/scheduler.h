#include <stdint.h>

typedef struct pcbEntryCDT
{
    uint16_t pid; 
    void * stackPointer;
    uint8_t state;
    uint8_t priority;
} pcbEntryCDT;

typedef struct pcbEntryCDT * pcbEntryADT;



void initializeScheduler();
void * scheduler(void * stackPointer);
int deleteFromScheduler(uint16_t pid);
int addToScheduler(void * stackPointer);
int getPid();
int getStatus(int pid);
