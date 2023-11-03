#include <scheduler.h>
#include <interrupts.h>
#include "MemoryManager.h"
#include <listADT.h>
#include <time.h>
#include <videodriver.h>
#include <pipes.h>
#define MAX_SIZE_PCB 5

#define KERNEL_STACK_BASE 0x352000 

extern char buffer;
int lock;
uint16_t nextPid; 
uint16_t lastSelected;
uint8_t halt=0;

typedef struct blockedReasonCDT {
    uint8_t blockReason;
    uint16_t size;
    void * waitingBuf; 
} blockedReasonCDT;
typedef struct pcbEntryCDT
{
    uint8_t * name;
    uint16_t parentPid;
    uint16_t pid; 
    void * stackPointer;
    void * basePointer;
    uint8_t state;
    void * topMemAllocated;
    void * fds[MAX_FD_PER_PROCESS];
    blockedReasonCDT blockedReasonCDT; 
    uint8_t priority;
    uint16_t ticksBeforeBlock;
    uint8_t isForeground;
    uint16_t countChildren;
} pcbEntryCDT;

// la informacion que tendra disponible el usuario
struct statProcess{
    uint8_t * name;
    int16_t pid;
    uint8_t state;
    uint8_t priority;
    uint8_t isForeground;
    void * stackPointer;
    void * basePointer;
    //FDS? 
};

typedef struct pcbEntryCDT * pcbEntryADT;

// ticks no modu => block 
pcbEntryADT PCB[MAX_SIZE_PCB]; 
// blockFunctions 

void tryToUnlockRead(int dim ){
    for ( int i=1; i<MAX_SIZE_PCB; i++){
        if ( PCB[i]->state == BLOCKED && PCB[i]->blockedReasonCDT.blockReason == BLOCKBYREAD
            && PCB[i]->blockedReasonCDT.waitingBuf == STDIN ){
            
                if ( PCB[i]->blockedReasonCDT.size == dim )
                    PCB[i]->state = READY;
                return;
        }
    }
}

void tryToUnlockPipe(int dim){
    for(int i=lastSelected; i<MAX_SIZE_PCB; i++){
        if ( PCB[i]->state == BLOCKED && PCB[i]->blockedReasonCDT.blockReason == BLOCKBYIPC
            && PCB[i]->blockedReasonCDT.size <= dim ){
                PCB[i]->state = READY;
                return;
        }
    }
}

void blockRunningProcess(uint8_t blockReason, uint16_t size, void * waitingBuf ){
    PCB[lastSelected]->state = BLOCKED;
    // aviso q info espera en struct
    PCB[lastSelected]->blockedReasonCDT.blockReason = blockReason;
    PCB[lastSelected]->blockedReasonCDT.size = size;
    PCB[lastSelected]->blockedReasonCDT.waitingBuf = waitingBuf;
    forceTimerInt();
}

// en realidad solo se llama cuando el user lo bloquea
// => blockReason = BLOCKBYUSER
//voy creando lista
/*
listADT processList;

//creo funcion para comparar por id
int comparePid(elemType elem1, elemType elem2){
    return elem1->pid - elem2->pid;
}
*/
void blockProcess(int pid, uint16_t blockReason){
    if ( pid == PCB[lastSelected]->pid || pid == RUNNING ){
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
            return;
        }
    }
}

int getForegroundPid(){
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->state == RUNNING && PCB[i]->isForeground){
            return PCB[i]->pid;
        }
    }
    return -1;
}

void signalHandler(int signal){
    if ( signal == CTRLD){
        updatePriority(lastSelected, 1);//para cambiar en funcion de lo que preguntemos
    }else if ( signal == CTRLC){
        deleteFromScheduler(getForegroundPid);
    }
}


void unblockProcess(int pid){

    // saco: " pid == PCB[lastSelected]->pid || " pues si o si no va a estar
    // corriendo si llame a unblock desde la shell
    //* en reali ni las sysBlock lo llaman, pues ya el scheduler las agarra 
    if ( pid == RUNNING ){
            PCB[lastSelected]->state = READY;
            PCB[lastSelected]->priority = 1;
            // ni me preocupo en elim info de blockCDT, total solo
            // se consulta cuando state== BLOCKED, => va a estar sobreescrita
            forceTimerInt();
            return;
    }

    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = READY;
            PCB[i]->priority = 1;
            return;
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

void createNewPipe(int writePid, int readPid){
    pipeADT pipe = createPipe(writePid, readPid);
    PCB[readPid]->fds[0] = pipe->buffer;
    PCB[writePid]->fds[1] = pipe->buffer;
}

void initializeScheduler(){
        nextPid = 0;
        lastSelected =0;
        //processList = newList(comparePid);
        
        
        // ya recervo espacio para todas las entradas de la tabla
        
        char sizeEntry = 15 * 8;  //sizeof(pcbEntryCDT); 
        PCB[0] = allocMemory( sizeEntry*MAX_SIZE_PCB );
        PCB[0]->state = TERMINATED;
        
        //chequeado que funciona
        for ( int i=1; i<MAX_SIZE_PCB; i++){
            PCB[i] = (char*) PCB[i-1] + sizeEntry;
            PCB[i]->state = TERMINATED;
            PCB[i]->fds[0] = STDIN;
            PCB[i]->fds[1] = STDOUT;
        }
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
    if ( lastSelected==0 ){
        halt = 0; 
        lastSelected = 1;  
        PCB[1]->state = RUNNING; 
        return PCB[1]->stackPointer;
    } 
    
    // GUARDA EL STACK  
    // si lo pongo antes, hlt stack se va a guardar=> queda afuera del while
    if ( !halt )
        PCB[lastSelected]->stackPointer = stackPointer;
    int i;

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
    if ( i==lastSelected && (PCB[lastSelected]->state == TERMINATED || PCB[lastSelected]->state == BLOCKED) ){
            halt = 1; 
            return PCB[0]->stackPointer; 
    }

    
    // Si es el =, se van a pisar => evi comparacion 
     // SI proceso no fue ni bloqueado ni terminado
    if ( PCB[lastSelected]->state == RUNNING ){
        PCB[lastSelected]->state = READY;
    }
    halt = 0; 
    PCB[i]->state = RUNNING;
    lastSelected = i;
    

    return PCB[lastSelected]->stackPointer;
}

//* con lista, paso puntero a nodo
static void deadChild(uint16_t index){
    // obtengo padre
    int parent = PCB[index]->parentPid;
    PCB[parent]->countChildren--;
    if ( PCB[parent]->state == BLOCKED && PCB[parent]->blockedReasonCDT.blockReason== BLOCKBYWAITCHILDREN && PCB[parent]->countChildren==0)
        PCB[parent]->state = READY;
    // no fuerzo interrupt pues el hijo al morir la genera
}

//flujo de estados:
// INIT (PID = PARENTPID = 1)
// Arranca en running pero despues siempre
//* RUNNING asi se evita syscall getpid 
int deleteFromScheduler(uint16_t pid){
    
    if ( pid == lastSelected || pid == RUNNING){
            // aviso al padre
            deadChild(lastSelected);
            PCB[lastSelected]->state = TERMINATED;
            //* aca se sacaria al nodo de la lista y desp free
            freeMemory(PCB[lastSelected]->topMemAllocated);
            forceTimerInt();
            return 0;
    }

    for(int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            PCB[i]->state = TERMINATED;
            deadChild(i);
            //* aca se sacaria al nodo de la lista y desp free
            freeMemory(PCB[lastSelected]->topMemAllocated);
            forceTimerInt();
            return 0;
        }
    }
    return -1;
}


int addToScheduler(void * stackPointer, char * name, void * topMemAllocated, void * basePointer, uint8_t isForeground){
    
    // creo halt
    if ( nextPid==0){
        PCB[0]->pid = nextPid++;
        PCB[0]->state = TERMINATED;
        PCB[0]->stackPointer = stackPointer;
        PCB[0]->fds[0] = &buffer;
        PCB[0]->fds[1] = BASEDIRVIDEO;
        return PCB[0]->pid;
    }

    PCB[lastSelected]->countChildren++;
    
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->state == TERMINATED){
            PCB[i]->name = name;
            PCB[i]->pid = nextPid++;
            PCB[i]->parentPid = PCB[lastSelected]->pid;
            PCB[i]->priority = 1;//es de los primeros que se ejecutaran pero podría haber un proceso con prioridad 1 que tenga menos ticks para terminar
            PCB[i]->ticksBeforeBlock = 0;
            PCB[i]->fds[0] = &buffer;
            PCB[i]->fds[1] = BASEDIRVIDEO;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->basePointer = basePointer;
            PCB[i]->topMemAllocated = topMemAllocated;
            PCB[i]->isForeground = isForeground;
            PCB[i]->state = READY;
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

void * getFdBuffer(int pid, int i){
    if ( i > MAX_FD_PER_PROCESS || i < 0 )
        return 0;
    if (pid == 0){
        return PCB[lastSelected]->fds[i];   
    }
    for (int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            return PCB[i]->fds[i];
        }
    }
}

int getPriority(int pid){
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            return PCB[i]->priority;
        }
    }
    return -1;
}

int getAllProcessInfo(stat * arrayStats){
    // no incluimos al halt
    int j=0,i;
    for ( i=1; i<MAX_SIZE_PCB; i++){
        if ( PCB[i]->state != TERMINATED ) {
            arrayStats[j]->name = PCB[i]->name;
            arrayStats[j]->pid = PCB[i]->pid;
            arrayStats[j]->state = PCB[i]->state;
            arrayStats[j]->stackPointer = PCB[i]->stackPointer;
            arrayStats[j]->basePointer = PCB[i]->basePointer;
            arrayStats[j++]->isForeground = PCB[i]->isForeground;
        }
    }
    return j; 
}