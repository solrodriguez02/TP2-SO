#include "scheduler.h"
#include <interrupts.h>
#include "MemoryManager.h"
#define MAX_SIZE_PCB 4

#define KERNEL_STACK_BASE 0x352000 



uint16_t nextPid; 
uint16_t lastSelected;

// ticks no modu => block 
pcbEntryADT PCB[MAX_SIZE_PCB]; 

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
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = READY;
            // aviso q info espera en struct
        }
    }
}

void initializeScheduler(){
        nextPid = 1;
        lastSelected =0;

        
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
        return PCB[lastSelected]->stackPointer; 
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
            PCB[i]->pid = nextPid++;
            PCB[i]->priority = 1;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->state = READY;
            PCB[i]->topMemAllocated = topMemAllocated;
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

int nopProcess(){
    while(1){
        _hlt();
    }
}