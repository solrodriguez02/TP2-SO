#include <pipes.h>
#include "MemoryManager.h"
#include <sync.h>

void * createPipe(int writePid, int readPid){
    pipeADT pipe = allocMemory(sizeof(pipeCDT));
    pipe->buffer = allocMemory(BUFFER_SIZE);
    pipe->numWrites = createSem("numWrites", 0);
    pipe->hasAccess = createSem("numWrites", 1);
    pipe->readPos = 0;
    pipe->writePos = 0;
    pipe->readPid = readPid;
    pipe->writePid = writePid;
    return pipe;
}

//se devuelve el numero de chars que fueron leidos
int readPipe(pipeADT pipe, char * buffer, int size){
    int i = 0;
    while(i < size){
        waitSem(pipe->numWrites);
        if( i ==0){
            waitSem(pipe->hasAccess);
        }
        if(pipe->readPos == pipe->writePos){
            postSem(pipe->hasAccess);
            return i;
        }
        buffer[i] = pipe->buffer[pipe->readPos];
        pipe->readPos = (pipe->readPos + 1) % BUFFER_SIZE;
        i++;
    }
    postSem(pipe->hasAccess);
    return i;
}

int writePipe(pipeADT pipe, char * buffer, int size){
    int i = 0;
    while(i < size){
        if( i ==0){
            //se le da prioridad a un solo writer
            waitSem(pipe->hasAccess);
            postSem(pipe->numWrites);
        }
        if(pipe->writePos == BUFFER_SIZE){
            postSem(pipe->hasAccess);
            return i;
        }
        pipe->buffer[pipe->writePos] = buffer[i];
        pipe->writePos = (pipe->writePos + 1) % BUFFER_SIZE;
        i++;
    }
    postSem(pipe->hasAccess);
    return i;
}

void destroyPipe(pipeADT pipe){
    destroySem(pipe->numWrites);
    destroySem(pipe->hasAccess);
    freeMemory(pipe->buffer);
    freeMemory(pipe);
}