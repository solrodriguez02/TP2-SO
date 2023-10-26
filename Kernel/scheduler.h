#include <stdint.h>

typedef struct pcbEntryCDT
{
    uint16_t parentPid;
    uint16_t pid; 
    void * stackPointer;
    uint8_t state;
    uint8_t priority;
    void * baseMemAllocated;
    uint16_t blockReason;
} pcbEntryCDT;

typedef struct pcbEntryCDT * pcbEntryADT;

pcbEntryADT PCB[MAX_SIZE_PCB]; 

void initializeScheduler();
void * scheduler(void * stackPointer);
int deleteFromScheduler(uint16_t pid);
int addToScheduler(void * stackPointer, void * baseMemAllocated);
int getPid();
int getStatus(int pid);
void unblockProcess(int pid);
void blockProcess(int pid, uint16_t blockReason);

#define BLOCKBYIO 0
#define BLOCKBYIPC 1
#define BLOCKBYUSER 2