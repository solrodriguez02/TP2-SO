#include <scheduler.h>
#include <interrupts.h>
#include "MemoryManager.h"
#include <listADT.h>
#include <time.h>
#define MAX_SIZE_PCB 4

#define KERNEL_STACK_BASE 0x352000 



uint16_t nextPid; 
uint16_t lastSelected;

// ticks no modu => block 
pcbEntryADT PCB[MAX_SIZE_PCB]; 
//voy creando lista
/*
listADT processList;

//creo funcion para comparar por id
int comparePid(elemType elem1, elemType elem2){
    return elem1->pid - elem2->pid;
}
*/
void blockProcess(int pid, uint16_t blockReason){
    if ( pid == PCB[lastSelected]->pid || pid == RUNNING_PROCESS ){
            PCB[lastSelected]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[lastSelected]->blockReason = blockReason;
            forceTimerInt();
            return;
    }

    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[i]->blockReason = blockReason;
        }
    }
}

void unblockProcess(int pid){
    if (!pid){
        PCB[lastSelected]->state = READY;
        return;
    }
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = READY;
            return;
            // aviso q info espera en struct
        }
    }
}

void updateTicks(int pid, int ticks){
    if (!pid){
        PCB[lastSelected]->ticksBeforeBlock += ticks;
        updatePriority(lastSelected, (int) ((PCB[lastSelected]->ticksBeforeBlock % QUANTUM) / QUANTUM));
        return;
    }
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->ticksBeforeBlock += ticks;
            updatePriority(i, (int) ((PCB[i]->ticksBeforeBlock % QUANTUM) / QUANTUM));
            return;
            // aviso q info espera en struct
        }
    }
}

void updatePriority(int pid, int priority){
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->priority = priority;
            return;
            // aviso q info espera en struct
        }
    }
}

void initializeScheduler(){
        nextPid = 1;
        lastSelected =0;
        //processList = newList(comparePid);
        
        PCB[1] = allocMemory( SIZE_ENTRY );
        PCB[2] = allocMemory( SIZE_ENTRY );
        PCB[3] = allocMemory( SIZE_ENTRY );
        PCB[1]->state = TERMINATED;
        PCB[2]->state = TERMINATED; 
        PCB[3]->state = TERMINATED; 
}

void * scheduler(void * stackPointer){
    
    /* reinicio ticks en 
        + sysDispa sys block 
        + add
        + delete si el era el q estaba corriendo
    => sig int llama al scheduler 
    restartTicks();
    
*/
    //* identificio rsp del kernel para no guardarlo 
    //if ( stackPointer < (void *) KERNEL_STACK_BASE || ){
    if ( lastSelected==0 ){
        lastSelected = 1; 
        PCB[1]->state = RUNNING; 
        return PCB[1]->stackPointer;
    } 
 // GUARDA EL STACK  
    PCB[lastSelected]->stackPointer = stackPointer;
    int i;
    // puedo apuntar a ese ultimo nodo seleccio

    for ( i=lastSelected+1; i!=lastSelected; i++ ){
        if ( i==MAX_SIZE_PCB){
            i=1;
            if ( i==lastSelected)
                break;
        }
        if ( PCB[i]->state==READY)
            break;
    }

    // retorno una direccion xq asm no tiene null
    if ( i==lastSelected && (PCB[lastSelected]->state == BLOCKED || PCB[lastSelected]->state == TERMINATED))
        return (void *) 0x0; 
    // Si es el =, se van a pisar => evi comparacion 
     
     // SI proceso no fue ni bloqueado ni terminado
    if ( PCB[lastSelected]->state == RUNNING )
        PCB[lastSelected]->state = READY;
    PCB[i]->state = RUNNING;
    lastSelected = i;
    
    return PCB[lastSelected]->stackPointer;
}

//flujo de estados:
// INIT (PID = PARENTPID = 1)
// Arranca en running pero despues siempre
//* RUNNING_PROCESS asi se evita syscall getpid 
int deleteFromScheduler(uint16_t pid){
    if ( pid == lastSelected || pid == RUNNING_PROCESS){
            PCB[lastSelected]->state = TERMINATED;
            //* aca se sacaria al nodo de la lista y desp free
            //freeMemory(PCB[lastSelected]->topMemAllocated);
            forceTimerInt();
            return 0;
    }

    for(int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = TERMINATED;
            //* aca se sacaria al nodo de la lista y desp free
            //freeMemory(PCB[lastSelected]->topMemAllocated);
            forceTimerInt();
            return 0;
        }
    }
    return -1;
}


int addToScheduler(void * stackPointer, void * topMemAllocated){
    
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->state == TERMINATED){
            //* CREAR NODO
            PCB[i]->parentPid = PCB[lastSelected]->pid;
            PCB[i]->pid = nextPid++;
            PCB[i]->priority = 1;//es de los primeros que se ejecutaran pero podría haber un proceso con prioridad 1 que tenga menos ticks para terminar
            PCB[i]->ticksBeforeBlock = 0;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->state = READY;
            PCB[i]->topMemAllocated = topMemAllocated;
            PCB[i]->isForeground = TRUE;
            forceTimerInt();
            return PCB[i]->pid;
        }
    }

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

int getPriority(int pid){
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            return PCB[i]->priority;
        }
    }
    return -1;
}
