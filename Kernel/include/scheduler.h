#include <stdint.h>
#include <time.h>
//#include "interrupts.h"

#define TRUE 1
#define FALSE 0

#define MAX_SIZE_PCB 5
#define SIZE_ENTRY 12
#define BLOCKBYREAD 3
#define BLOCKBYWRITE 4
#define BLOCKBYIPC 5
#define BLOCKBYUSER 6
#define BLOCKBYWAITCHILDREN 7

#define RUNNING 0
#define READY 1 
#define BLOCKED 2
#define TERMINATED 3

#define MAX_FD_PER_PROCESS 4
#define MAX_SEM_PER_PROCESS 2
#define STDIN 0
#define STDOUT 1

#define MAX_CHILDREN_PER_PROCESS 5

typedef struct pcbEntryCDT * pcbEntryADT;

typedef struct statProcess * stat;

void initializeScheduler();
void * scheduler(void * stackPointer);
int deleteFromScheduler(uint16_t pid);
int addToScheduler(void * stackPointer, void * topMemAllocated, void * basePointer, uint8_t isForeground);
int getPid();
int getStatus(int pid);
void unblockProcess(int pid);
void blockProcess(int pid, uint16_t blockReason);

void blockRunningProcess(uint8_t blockReason, uint16_t size, void * waitingBuf );
void * getFdBuffer(int pid, int i);
void updateTicks(int pid, int ticks);
void updatePriority(int pid, int priority);
int getPriority(int pid);
void tryToUnlockRead(int dim );
void tryToUnlockPipe(int dim );
int getForegroundPid();
void signalHandler(int signal);
void createNewPipe(int writePid, int readPid);
void getAllProcessInfo(stat * arrayStats);
int addSemToPCB(char * name, int pid);
int deleteSemFromPCB(char * name, int pid);