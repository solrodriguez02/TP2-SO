#include <stdint.h>
#include <time.h>
//#include "interrupts.h"

#define TRUE 1
#define FALSE 0

#define SIZE_ENTRY 12
#define BLOCKBYREAD 0
#define BLOCKBYWRITE 1
#define BLOCKBYIPC 2
#define BLOCKBYUSER 3

#define RUNNING 2
#define READY 1 
#define BLOCKED 0
#define TERMINATED 3

#define RUNNING_PROCESS 0

#define MAX_FD_PER_PROCESS 4
#define STDIN 0
#define STDOUT 1

#define MAX_CHILDREN_PER_PROCESS 5
typedef struct pcbEntryCDT * pcbEntryADT;

void initializeScheduler();
void * scheduler(void * stackPointer);
int deleteFromScheduler(uint16_t pid);
int addToScheduler(void * stackPointer, void * topMemAllocated, uint8_t isForeground);
int getPid();
int getStatus(int pid);
void unblockProcess(int pid);
void blockProcess(int pid, uint16_t blockReason);

void blockRunningProcess(uint8_t blockReason, uint16_t size, void * waitingBuf );
void * getFd(uint8_t i);
void updateTicks(int pid, int ticks);
void updatePriority(int pid, int priority);
int getPriority(int pid);
void tryToUnlockRead(int dim );