#include <semaphore.h>
//implementacion para un pipe anonimo
#define BUFFER_SIZE 1024
#define FDSPERPIPE 2
#define READEXT 0
#define WRITEEXT 1

typedef struct pipeCDT {
    char * buffer;
    sem_ptr numWrites;
    sem_ptr hasAccess;
    int readPos;
    int writePos;
    int readPid;
    int writePid;
} pipeCDT;

typedef struct pipeCDT * pipeADT;

void * createPipe(int writePid, int readPid);
int writePipe(pipeADT pipe, char * buffer, int size);
int readPipe(pipeADT pipe, char * buffer, int size);
void destroyPipe(pipeADT pipe);