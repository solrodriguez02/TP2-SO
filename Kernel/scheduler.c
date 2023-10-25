#include "scheduler.h"
#include <stdio.h>

#define MAX_SIZE_PCB 2

#define RUNNING 2
#define READY 1 
#define BLOCKED 0

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

        PCB[0]->pid = nextPid++;
        PCB[0]->priority = 1;
        PCB[0]->state = RUNNING; 

        PCB[1]->state = BLOCKED; 
}

void * scheduler(void * stackPointer){
    
    PCB[lastSelected]->stackPointer = stackPointer;
    int i;
    // puedo apuntar a ese ultimo nodo seleccio

    for ( i=lastSelected+1; i!=lastSelected; i++ ){
        if ( i==MAX_SIZE_PCB){
            i=0;
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
    
}
/*
//para syscall bloqueante
void blockRunningProcess(){

}
*/

int deleteFromScheduler(uint16_t pid){
    //= si killed 
}

int addToScheduler(void * stackPointer){

}