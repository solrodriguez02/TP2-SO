#include <semaphore.h>
#include "MemoryManager.h"
#include <scheduler.h>

void * createSem(){
    sem_ptr sem = allocMemory(sizeof(sem_t));
    sem->value = 0;
    return sem;
}

void waitSem(sem_ptr sem){
    if(sem->value == 0){
        blockProcess(0, BLOCKBYIPC);
        sem->value--;
    }
    else{
        sem->value--;
    }
}

void postSem(sem_ptr sem){
    sem->value++;
    if(sem->value <= 0){
        //tal vez sería mejor usar la info del struct del pipe especifico en vez de buscar (por ahi hay más de un pipe abierto entre distintos procesos)
        tryToUnlockPipe(0);
    }
}