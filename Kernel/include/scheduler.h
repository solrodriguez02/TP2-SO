#include <stdint.h>
#include <time.h>
//#include "interrupts.h"

#define TRUE 1
#define FALSE 0

#define SIZE_ENTRY 12
#define BLOCKBYIO 0
#define BLOCKBYIPC 1
#define BLOCKBYUSER 2

#define RUNNING 2
#define READY 1 
#define BLOCKED 0
#define TERMINATED 3

#define RUNNING_PROCESS 0

typedef struct pcbEntryCDT
{
    uint16_t parentPid;
    uint16_t pid; 
    void * stackPointer;
    void * topMemAllocated;
    uint8_t state;
    uint8_t priority;
    uint16_t ticksBeforeBlock;
    uint16_t blockReason;
    uint16_t isForeground;
} pcbEntryCDT;

typedef struct pcbEntryCDT * pcbEntryADT;



void initializeScheduler();
void * scheduler(void * stackPointer);
int deleteFromScheduler(uint16_t pid);
int addToScheduler(void * stackPointer, void * baseMemAllocated);
int getPid();
int getStatus(int pid);
void unblockProcess(int pid);
void blockProcess(int pid, uint16_t blockReason);
void updateTicks(int pid, int ticks);
void updatePriority(int pid, int priority);
int getPriority(int pid);
