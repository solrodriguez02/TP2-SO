#include <pipes.h>
#include "MemoryManager.h"
#include <sync.h>

void * createPipe(){
    pipeADT pipe = allocMemory(sizeof(pipeCDT));
    pipe->buffer = allocMemory(BUFFER_SIZE);
    pipe->writesAvailable = createSem("numWrites", BUFFER_SIZE);
    pipe->readsAvailable = createSem("numReads", 0);
    pipe->hasAccess = createSem("hasAccess", 1);
    pipe->readPos = 0;
    pipe->writePos = 0;
    pipe->brokenPipe = 0;
    pipe->cantProcessesConnected = 2;
    return pipe;
}

//se devuelve el numero de chars que fueron leidos
int readPipe(pipeADT pipe, char * buffer, int size){
    int i = 0;
    while(i < size && !pipe->brokenPipe){
        //priorizo que haya varios lectores
        /* se maneja con semaforos
        if(pipe->readPos == pipe->writePos){
            postSem(pipe->hasAccess);
            return i;
        }
        */
        waitSem(pipe->readsAvailable);
        waitSem(pipe->hasAccess);
        buffer[i] = pipe->buffer[pipe->readPos];
        pipe->readPos = (pipe->readPos + 1) % BUFFER_SIZE;
        i++;
        postSem(pipe->writesAvailable);
        postSem(pipe->hasAccess);
    }
    if (pipe->brokenPipe){
        buffer[i] = EOF;
        return -1;
    }
    return 0;
}

int writePipe(pipeADT pipe, char * buffer, int size){
    int i = 0;
    while(i < size && !pipe->brokenPipe){
        /*
        if(pipe->writePos == BUFFER_SIZE){
            //! no deberia bloquearse si llega a BUFFER_SIZE?
            //! hasta q no se haga un read ?
            postSem(pipe->hasAccess);
            return i;
        }
        */
        waitSem(pipe->writesAvailable);
        waitSem(pipe->hasAccess);
        pipe->buffer[pipe->writePos] = buffer[i];
        pipe->writePos = (pipe->writePos + 1) % BUFFER_SIZE;
        i++;
        postSem(pipe->readsAvailable);
        postSem(pipe->hasAccess);
    }
     if (pipe->brokenPipe){
        return -1;
    }
    return 0;
}

void closePipe(pipeADT pipe){
    pipe->brokenPipe = 1;
    pipe->cantProcessesConnected--;
    if (pipe->cantProcessesConnected == 0){
        destroyPipe(pipe);
    }
}

void destroyPipe(pipeADT pipe){
    destroySem(pipe->readsAvailable);
    destroySem(pipe->writesAvailable);
    destroySem(pipe->hasAccess);
    freeMemory(pipe->buffer);
    freeMemory(pipe);
}