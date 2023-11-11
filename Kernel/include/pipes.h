#include <semaphore.h>
//implementacion para un pipe anonimo
#define BUFFER_SIZE 1024
#define FDSPERPIPE 2
#define READEXT 0
#define WRITEEXT 1
#define EOF '\377'

typedef struct pipeCDT {
    char * buffer;
    sem_ptr writesAvailable;
    sem_ptr readsAvailable;
    sem_ptr hasAccess;
    int readPos;
    int writePos;
    int brokenPipe;
    int cantProcessesConnected;
} pipeCDT;

typedef struct pipeCDT * pipeADT;

void * createPipe();
int writePipe(pipeADT pipe, char * buffer, int size);
int readPipe(pipeADT pipe, char * buffer, int size);
void closePipe(pipeADT pipe);
void destroyPipe(pipeADT pipe);