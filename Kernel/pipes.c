#include <pipes.h>
#include "MemoryManager.h"
#include <sync.h>

void * createPipe(int writePid, int readPid){
    pipeADT pipe = allocMemory(sizeof(pipeCDT));
    pipe->buffer = allocMemory(BUFFER_SIZE);
    pipe->numWrites = createSem();
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
            //enter_region();
        }
        if(pipe->readPos == pipe->writePos){
            //leave_region();
            return i;
        }
        buffer[i] = pipe->buffer[pipe->readPos];
        pipe->readPos = (pipe->readPos + 1) % BUFFER_SIZE;
        i++;
    }
    //leave_region();
    return i;
}

int writePipe(pipeADT pipe, char * buffer, int size){
    int i = 0;
    while(i < size){
        postSem(pipe->numWrites);
        if( i ==0){
            //enter_region();
        }
        if(pipe->writePos == BUFFER_SIZE){
            //leave_region();
            return i;
        }
        pipe->buffer[pipe->writePos] = buffer[i];
        pipe->writePos = (pipe->writePos + 1) % BUFFER_SIZE;
        i++;
    }
    //leave_region();
    return i;
}