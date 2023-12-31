#include <stdint.h>
#include <time.h>
//#include "interrupts.h"

#define TRUE 1
#define FALSE 0

#define MAX_SIZE_PCB 15
#define SIZE_ENTRY 12
#define BLOCKBYREAD 4
#define BLOCKBYSYNC 5
#define BLOCKBYUSER 6
#define BLOCKBYWAITCHILDREN 7
#define BLOCKBYWAITCHILD 8

#define RUNNING 0
#define READY 1 
#define BLOCKED 2
#define TERMINATED 3

#define MAX_FD_PER_PROCESS 2
#define MAX_SEM_PER_PROCESS 5
#define STDIN 0
#define STDOUT 1

#define MUTEX 0
#define WAITING 1

#define TICKETS_BEFORE_LOOSING_PRIOR (QUANTUM*QUANTUM)
#define MAX_PRIORITY 25

#define SHELL_PID 1
typedef struct pcbEntryCDT * pcbEntryADT;

typedef struct statProcess * stat;

void initializeScheduler();
void * scheduler(void * stackPointer, unsigned lastTicks);
int deleteFromScheduler(uint16_t pid);
int addToScheduler(void * stackPointer, char * name, void * topMemAllocated, void * basePointer, uint8_t isForeground);
int getPid();
int getStatus(int pid);
void unblockProcess(int pid);
void blockProcess(int pid, uint16_t blockReason);

void blockRunningProcess(uint8_t blockReason, uint16_t size, void * waitingBuf );
void * getFdBuffer(int pid, int i);
int updatePriority(int pid, int priority);
void updateRunningPriority(unsigned lastTicks);
int getPriority(int pid);
void tryToUnlockRead(int dim );
void tryToUnlockSem(void * semLock, int reason);
void tryToUnlockPipe(int dim );
int getForegroundPid();
void signalHandler(int signal);
void createNewPipe(char ** params1, char ** params2);
int getAllProcessInfo(stat arrayStats);
int addSemToPCB(char * name, int pid);
int deleteSemFromPCB(char * name, int pid);
