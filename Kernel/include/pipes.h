#include <semaphore.h>
//implementacion para un pipe anonimo
#define BUFFER_SIZE 1024
#define FDSPERPIPE 2
#define READEXT 0
#define WRITEEXT 1

typedef struct pipeCDT {
    char * buffer;
    sem_ptr writesAvailable;
    sem_ptr readsAvailable;
    sem_ptr hasAccess;
    int readPos;
    int writePos;
    int readPid;
    int writePid;
} pipeCDT;

typedef struct pipeCDT * pipeADT;

void * createPipe();
void writePipe(pipeADT pipe, char * buffer, int size);
void readPipe(pipeADT pipe, char * buffer, int size);
void destroyPipe(pipeADT pipe);