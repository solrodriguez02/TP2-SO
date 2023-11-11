#include <semaphore.h>
#include <MemoryManager.h>
#include <lib.h>
#include <interrupts.h>
#include <sync.h>
#include <scheduler.h>
#define MAX_LEN_NAME 40
typedef struct sem_t{
    int lockMutex;
    int value;
    char name[MAX_LEN_NAME];
    int connectedProcesses;
};

sem_ptr createSem(char * name, int value){
    sem_ptr sem = allocMemory(sizeof(struct sem_t));
    memcpy(sem->name, name, strlen(name));
    sem->value = value;
    sem->lockMutex = 0;
    return sem;
}

void waitSem(sem_ptr sem){
    enterRegion(&sem->lockMutex);
    while (1){
        if (sem->value == 0){
            blockRunningProcess(BLOCKBYSYNC, WAITING, &sem->lockMutex);
            enterRegion(&sem->lockMutex);
        }
        else{
            break;
        }
    }   
    sem->value--;
    leaveRegion(&sem->lockMutex,MUTEX);
}

void postSem(sem_ptr sem){
    enterRegion(&sem->lockMutex);
    sem->value++;
    leaveRegion(&sem->lockMutex,MUTEX);
    tryToUnlockSem(&sem->lockMutex,WAITING);
}

void destroySem(sem_ptr sem){
    freeMemory(sem);
}

int getSemValue(sem_ptr sem){
    return sem->value;
}

char * getSemName(sem_ptr sem){
    return sem->name;
}

void processConnected( sem_ptr sem ){
    ++sem->connectedProcesses;
}

/**
 * @return retorna la cantidad de procesos que
 * siguen conectados al semaforo
 */
int disconnectProcess( sem_ptr sem ){
    return --sem->connectedProcesses;
}