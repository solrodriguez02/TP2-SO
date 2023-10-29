#include "scheduler.h"
#include <interrupts.h>
#include "MemoryManager.h"
#define MAX_SIZE_PCB 4

#define KERNEL_STACK_BASE 0x352000 

extern char buffer;

uint16_t nextPid; 
uint16_t lastSelected;


typedef struct blockedReasonCDT {
    uint8_t blockReason;
    uint16_t size;
    void * waitingBuf; 
} blockedReasonCDT;

typedef struct pcbEntryCDT
{
    uint16_t parentPid;
    uint16_t pid; 
    void * stackPointer;
    uint8_t state;
    uint8_t priority;
    void * topMemAllocated;
    void * fds[MAX_FD_PER_PROCESS];
    blockedReasonCDT blockedReasonCDT; 
} pcbEntryCDT;

typedef struct pcbEntryCDT * pcbEntryADT;

// ticks no modu => block 
pcbEntryADT PCB[MAX_SIZE_PCB]; 
// blockFunctions 

void blockRunningProcess(uint8_t blockReason, uint16_t size, void * waitingBuf ){
    PCB[lastSelected]->state = BLOCKED;
    // aviso q info espera en struct
    PCB[lastSelected]->blockedReasonCDT.blockReason = blockReason;
    PCB[lastSelected]->blockedReasonCDT.size = size;
    PCB[lastSelected]->blockedReasonCDT.waitingBuf = waitingBuf;
    forceTimerInt();
}

uint8_t canBeUnlocked ( int i, uint8_t reason){
    //if (  sizeof( PCB[i]->blockedReasonCDT.waitingBuf) == ){
    if (  buffer != 0 ){
        return 1; 
    }
    return 0; 
}

// en realidad solo se llama cuando el user lo bloquea
// => blockReason = BLOCKBYUSER
void blockProcess(int pid, uint16_t blockReason){
    
    if ( pid == PCB[lastSelected]->pid || pid == RUNNING_PROCESS ){
            PCB[lastSelected]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[lastSelected]->blockedReasonCDT.blockReason = blockReason;
            forceTimerInt();
            return;
    }

    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[i]->blockedReasonCDT.blockReason = blockReason;
        }
    }
}

void unblockProcess(int pid){
    // saco: " pid == PCB[lastSelected]->pid || " pues si o si no va a estar
    // corriendo si llame a unblock desde la shell
    //* en reali ni las sysBlock lo llaman, pues ya el scheduler las agarra 
    if ( pid == RUNNING_PROCESS ){
            PCB[lastSelected]->state = READY;
            // ni me preocupo en elim info de blockCDT, total solo
            // se consulta cuando state== BLOCKED, => va a estar sobreescrita
            forceTimerInt();
            return;
    }

    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = READY;
            
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

    if ( stackPointer == 0)
        buffer = 'A';
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
        //! ojo waitpid => hay q bajarle la prioridad
        if ( PCB[i]->state==BLOCKED){
            if ( canBeUnlocked(i, PCB[i]->blockedReasonCDT.blockReason) ){
                // llamo a funcion 
                PCB[i]->state = READY; 
                break; 
            }
        }

        if ( PCB[i]->state==READY)
            break;
    }

    // retorno una direccion xq asm no tiene null
    if ( i==lastSelected && (PCB[lastSelected]->state == BLOCKED || PCB[lastSelected]->state == TERMINATED)){
        if ( PCB[lastSelected]->state == BLOCKED && canBeUnlocked(i, PCB[i]->blockedReasonCDT.blockReason)) {
                // llamo a funcion 
                PCB[i]->state = READY; 
        } else 
            return (void *) 0x0; 
    }
        
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
            PCB[i]->fds[0] = STDIN;
            PCB[i]->fds[1] = STDOUT;
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

void * getFd(uint8_t i){
    if ( i > MAX_FD_PER_PROCESS || i < 0 )
        return 0;
    return PCB[lastSelected]->fds[i];   
}