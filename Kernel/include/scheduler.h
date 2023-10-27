#include <stdint.h>
#define SIZE_ENTRY 12
#define BLOCKBYIO 0
#define BLOCKBYIPC 1
#define BLOCKBYUSER 2

#define RUNNING 2
#define READY 1 
#define BLOCKED 0
#define TERMINATED 3
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



void initializeScheduler();
void * scheduler(void * stackPointer);
int deleteFromScheduler(uint16_t pid);
int addToScheduler(void * stackPointer, void * baseMemAllocated);
int getPid();
int getStatus(int pid);
void unblockProcess(int pid);
void blockProcess(int pid, uint16_t blockReason);
