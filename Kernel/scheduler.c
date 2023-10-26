#include "scheduler.h"
#include <stdio.h>
#include "MemoryManager.h"

#define MAX_SIZE_PCB 2

#define RUNNING 2
#define READY 1 
#define BLOCKED 0
#define TERMINATED 3

#define KERNEL_STACK_BASE 0x352000 

uint16_t nextPid; 
uint16_t lastSelected;

// ticks no modu => block 
pcbEntryADT PCB[MAX_SIZE_PCB]; 

void blockProcess(int pid, uint16_t blockReason){
    // sys bloq => 1º modif PCB.state
    //             2º fuerzo interr
    for (int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[i]->blockReason = blockReason;
        }
    }
}

void unblockProcess(int pid){
    for (int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = BLOCKED;
            // aviso q info espera en struct
                    }
    }
    PCB[lastSelected]->state = READY;
}

void initializeScheduler(){
        nextPid = 1;
        lastSelected =0;

        PCB[0]->parentPid = nextPid;
        PCB[0]->pid = nextPid++;
        PCB[0]->priority = 1;
        PCB[0]->state = TERMINATED; 
        //PCB[1]->state = BLOCKED; 
}

void * scheduler(void * stackPointer){
    
    // identificio rsp del kernel para no guardarlo 
    if ( stackPointer < (void *) KERNEL_STACK_BASE){
        PCB[0]->state = RUNNING; 
        return (void *)0x710f60;
        return PCB[0]->stackPointer;
    } else
      
    PCB[lastSelected]->stackPointer = stackPointer;
    /*
    int i;
    // puedo apuntar a ese ultimo nodo seleccio

    for ( i=lastSelected+1; i!=lastSelected; i++ ){
        if ( i==MAX_SIZE_PCB){
            i=0;
            if ( i==lastSelected)
                break;
            continue;
        }
        if ( PCB[i]->state==READY)
            break;
    }

    if ( i==lastSelected && PCB[lastSelected]->state == BLOCKED)
        return NULL; 
    // Si es el =, se van a pisar => evi comparacion 
    
    PCB[lastSelected]->state = READY;
    PCB[i]->state = RUNNING;
    lastSelected = i;
    
    return PCB[lastSelected]->stackPointer;
    */
   return PCB[lastSelected]->stackPointer;
}

//flujo de estados:
// INIT (PID = PARENTPID = 1)
// Arranca en running pero despues siempre
int deleteFromScheduler(uint16_t pid){
    for(int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = TERMINATED;
            freeMemory(PCB[i]->baseMemAllocated);
            return 1;
        }
    }
    return 0;
}

int addToScheduler(void * stackPointer, void * baseMemAllocated){
    PCB[0]->stackPointer = stackPointer;
    return nextPid; 
    for (int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->state == TERMINATED){
            PCB[i]->pid = nextPid;
            PCB[i]->priority = 1;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->state = READY;
            PCB[i]->baseMemAllocated = baseMemAllocated;
            return 1;
        }
    }
    return 0;
}

int getPid(){
    return PCB[lastSelected]->pid;
}

int getStatus(int pid){
    for (int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            return PCB[i]->state;
        }
    }
    return -1;
}

int nopProcess(){
    while(1){
        _halt();
    }
}