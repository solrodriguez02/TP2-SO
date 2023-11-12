#include <pipes.h>
#include <semaphore.h>
#include "MemoryManager.h"
#include <sync.h>

void * createPipe(){
    pipeADT pipe = allocMemory(sizeof(pipeCDT));
    pipe->buffer = allocMemory(BUFFER_SIZE);
    pipe->writesAvailable = createSem("numWrites", BUFFER_SIZE);
    pipe->readsAvailable = createSem("numReads", 0);
    pipe->hasAccess = createSem("hasAccess", 1);
    pipe->mutexBrokenPipe = createSem("mutexBroken", 1);
    pipe->readPos = 0;
    pipe->writePos = 0;
    pipe->brokenPipe = 0;
    pipe->sendEOF = 0;
    pipe->cantProcessesConnected = 0;
    return pipe;
}

int readPipe(pipeADT pipe, char * buffer, int size){
    int i = 0;
    while(i < size){
        waitSem(pipe->readsAvailable);
        waitSem(pipe->hasAccess);
        buffer[i] = pipe->buffer[pipe->readPos];
        pipe->readPos = (pipe->readPos + 1) % BUFFER_SIZE;
        i++;
        postSem(pipe->writesAvailable);
        postSem(pipe->hasAccess);
    }
    return 0;
}

int writePipe(pipeADT pipe, char * buffer, int size){
    int i = 0;
    while(i < size){
        waitSem(pipe->brokenPipe);
        if (pipe->brokenPipe && !pipe->sendEOF){
            postSem(pipe->brokenPipe);
            return -1;
        }
        postSem(pipe->brokenPipe);

        waitSem(pipe->writesAvailable);
        waitSem(pipe->hasAccess);
        pipe->buffer[pipe->writePos] = buffer[i];
        pipe->writePos = (pipe->writePos + 1) % BUFFER_SIZE;
        i++;
        postSem(pipe->readsAvailable);
        postSem(pipe->hasAccess);
    }
    return 0;
}

void closePipe(pipeADT pipe){
    
    waitSem(pipe->mutexBrokenPipe);
    pipe->cantProcessesConnected--;
    if (pipe->cantProcessesConnected == 1){
        pipe->brokenPipe = 1;
    }
    postSem(pipe->mutexBrokenPipe);

    if (pipe->cantProcessesConnected == 0){
        destroyPipe(pipe);
    } 
}

void destroyPipe(pipeADT pipe){
    destroySem(pipe->readsAvailable);
    destroySem(pipe->writesAvailable);
    destroySem(pipe->hasAccess);
    destroySem(pipe->mutexBrokenPipe);
    freeMemory(pipe->buffer);
    freeMemory(pipe);
}

void connectToPipe(pipeADT pipe){
    ++pipe->cantProcessesConnected;
}

int isBrokenPipe(pipeADT pipe){
    waitSem(pipe->mutexBrokenPipe);
    if (pipe->brokenPipe){
        postSem(pipe->mutexBrokenPipe);
        return 1;
    }
    postSem(pipe->mutexBrokenPipe);
    return 0;
}
void allowEOF(pipeADT pipe){
    waitSem(pipe->mutexBrokenPipe);
    pipe->sendEOF = 1;
    postSem(pipe->mutexBrokenPipe);
}