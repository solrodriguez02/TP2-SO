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
};

//para wrapper en syscall;

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
            blockRunningProcess(BLOCKBYIPC, 0, &sem->lockMutex);
            enterRegion(&sem->lockMutex);
            //leave_region(&sem->lockMutex);
            //sem->value--;
        }
        else{
            break;
            //sem->value--;
        }
    }   
    sem->value--;
    leaveRegion(&sem->lockMutex);
}

void postSem(sem_ptr sem){
    enterRegion(&sem->lockMutex);
    sem->value++;
    leaveRegion(&sem->lockMutex);
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