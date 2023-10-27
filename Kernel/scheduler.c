#include "scheduler.h"
#include <stdio.h>
#include "MemoryManager.h"
#define MAX_SIZE_PCB 3

#define KERNEL_STACK_BASE 0x352000 

uint16_t nextPid; 
uint16_t lastSelected;

// ticks no modu => block 
pcbEntryADT PCB[MAX_SIZE_PCB]; 

void blockProcess(int pid, uint16_t blockReason){
    // sys bloq => 1º modif PCB.state
    //             2º fuerzo interr
    if ( pid == PCB[lastSelected]->pid ){
            PCB[lastSelected]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[lastSelected]->blockReason = blockReason;
            forceTimerInt();
            return;
    }

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
            PCB[i]->state = READY;
            // aviso q info espera en struct
        }
    }
}

void initializeScheduler(){
        nextPid = 1;
        lastSelected =0;

        PCB[0] = allocMemory( SIZE_ENTRY );
        PCB[1] = PCB[0]+8 *10;
        PCB[0]->state = TERMINATED;
        PCB[1]->state = TERMINATED; 
        PCB[2] = PCB[1]+8 *10;
        PCB[2]->state = TERMINATED; 
}

void * scheduler(void * stackPointer){
    
    restartTicks();
    // identificio rsp del kernel para no guardarlo 
    if ( stackPointer < (void *) KERNEL_STACK_BASE){
        PCB[0]->state = RUNNING; 
        return PCB[0]->stackPointer;
    } 
      
    PCB[lastSelected]->stackPointer = stackPointer;
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

    // retorno una direccion xq asm no tiene null
    if ( i==lastSelected && (PCB[lastSelected]->state == BLOCKED || PCB[lastSelected]->state == TERMINATED))
        return PCB[lastSelected]->stackPointer; 
    // Si es el =, se van a pisar => evi comparacion 
     
    PCB[lastSelected]->state = READY;
    PCB[i]->state = RUNNING;
    lastSelected = i;
    
    return PCB[lastSelected]->stackPointer;
}

//flujo de estados:
// INIT (PID = PARENTPID = 1)
// Arranca en running pero despues siempre
int deleteFromScheduler(uint16_t pid){
    if ( pid == PCB[lastSelected]->pid ){
            PCB[lastSelected]->state = TERMINATED;
            // aviso q info espera en struct
            forceTimerInt();
            return;
    }

    for(int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = TERMINATED;
            freeMemory(PCB[i]->baseMemAllocated);
            return 1;
        }
    }
    return -1;
}


int addToScheduler(void * stackPointer, void * baseMemAllocated){

    for (int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->state == TERMINATED){
            //* CREAR NODO
            PCB[i]->pid = nextPid++;
            PCB[i]->priority = 1;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->state = READY;
            PCB[i]->baseMemAllocated = baseMemAllocated;
            return PCB[i]->pid;
        }
    }
    forceTimerInt();

    return -1;
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
        _hlt();
    }
}