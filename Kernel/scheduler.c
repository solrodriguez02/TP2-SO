#include "scheduler.h"
#include <stdio.h>

#define MAX_SIZE_PCB 2

#define RUNNING 2
#define READY 1 
#define BLOCKED 0
#define TERMINATED 3

#define KERNEL_STACK_BASE 0x352000 

uint16_t nextPid; 
uint16_t lastSelected;

typedef struct pcbEntryCDT
{
    uint16_t pid; 
    void * stackPointer;
    uint8_t state;
    uint8_t priority;
} pcbEntryCDT;

typedef struct pcbEntryCDT * pcbEntryADT;

pcbEntryADT PCB[MAX_SIZE_PCB]; 

// ticks no modu => block 

void blockRunningProcess(){
    // sys bloq => 1º modif PCB.state
    //             2º fuerzo interr
    PCB[lastSelected]->state = BLOCKED;
    // aviso q info espera en struct
}

void initializeScheduler(){
        nextPid = 1;
        lastSelected =0;

/*
        PCB[0]->pid = nextPid++;
        PCB[0]->priority = 1;
        PCB[0]->state = TERMINATED; 
*/
        PCB[0]->state = TERMINATED;
        PCB[1]->state = TERMINATED; 
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
   PCB[0]->state = RUNNING;
   return PCB[0]->stackPointer;
}
/*
//para syscall bloqueante
void blockRunningProcess(){

}
*/

int deleteFromScheduler(uint16_t pid){
    for(int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = TERMINATED;
            return 0;
        }
    }
    return 1;
}

int addToScheduler(void * stackPointer){
    int i=0; 
     PCB[i]->pid = nextPid;
            PCB[i]->priority = 1;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->state = READY;
    /*
    int i; 
    for ( i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->state == TERMINATED){
            PCB[i]->pid = nextPid;
            PCB[i]->priority = 1;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->state = READY;
        }
    }
    if ( i == MAX_SIZE_PCB)
        return -1;
        */
    return nextPid++;
}