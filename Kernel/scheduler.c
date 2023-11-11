#include <scheduler.h>
#include <interrupts.h>
#include <MemoryManager.h>
#include <time.h>
#include <semaphore.h>
#include <videodriver.h>
#include <pipes.h>
#include <semaphore.h>
#include <sync.h>
#include <blockingSys.h>

#define KERNEL_STACK_BASE 0x352000 

extern char buffer[MAX_SIZE_BUF];
int lock;
uint16_t nextPid; 
uint16_t lastSelected;
uint8_t halt=1;
uint16_t lastNormal = 1; 
uint16_t lastWithPrior = 1; 

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
    char * sems[MAX_SEM_PER_PROCESS];
    blockedReasonCDT blockedReasonCDT; 
    uint8_t priority;
    uint16_t ticketsBeforeLoosingPrior;
    uint8_t isForeground;
    uint16_t countChildren;
    uint16_t lastSemOpen;
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

// no tiene en cuenta el halt
/**
 * @return retorna index del proceso si es que este
 * existe / no termino
 */
static inline int searchProcessByPid(uint16_t pid){
    for(int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            if ( PCB[i]->state == TERMINATED )
                return -1;
            return i;
        }
    }
    return -1;
}

static int getPCBIndex(int pid){
    for (int i = 0; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            return i;
        }
    }
    return -1;
}

void tryToUnlockRead(int dim ){
    for ( int i=1; i<MAX_SIZE_PCB; i++){
        if ( PCB[i]->state == BLOCKED && PCB[i]->blockedReasonCDT.blockReason == BLOCKBYREAD
            && PCB[i]->blockedReasonCDT.waitingBuf == STDIN ){
            
                if ( PCB[i]->blockedReasonCDT.size == dim ){
                    PCB[i]->state = READY;
                    //PCB[i]->priority = 1;
                }
                return;
        }
    }
}

void tryToUnlockSem(void * semLock, int reason){
    for(int i=lastSelected+1; i != lastSelected; i++){
        if ( i==MAX_SIZE_PCB){
            i=1;
            if ( i==lastSelected)
                break;
        }        
        if ( PCB[i]->state == BLOCKED && PCB[i]->blockedReasonCDT.blockReason == BLOCKBYIPC
            && PCB[i]->blockedReasonCDT.waitingBuf == semLock &&  PCB[i]->blockedReasonCDT.size == reason){
                PCB[i]->state = READY;
                return;
        }
    }
}


void tryToUnlockPipe(int dim){
    for(int i=lastSelected+1; i != lastSelected; i++){
        if ( i==MAX_SIZE_PCB){
            i=1;
            if ( i==lastSelected)
                break;
        }        
        if ( PCB[i]->state == BLOCKED && PCB[i]->blockedReasonCDT.blockReason == BLOCKBYIPC
            && PCB[i]->blockedReasonCDT.size <= dim ){
                PCB[i]->state = READY;
                return;
        }
    }
}


void blockRunningProcess(uint8_t blockReason, uint16_t size, void * waitingBuf ){
    if ( blockReason == BLOCKBYWAITCHILD ){
        int indexChild = searchProcessByPid(size);
        // chequea si es hijo 
        if ( indexChild<0 || PCB[indexChild]->parentPid != PCB[lastSelected]->pid)
        return;
    } else if ( blockReason == BLOCKBYWAITCHILDREN && !PCB[lastSelected]->countChildren )
        return;

    PCB[lastSelected]->state = BLOCKED;
    // aviso q info espera en struct
    PCB[lastSelected]->blockedReasonCDT.blockReason = blockReason;
    if (blockReason == BLOCKBYIPC){
        leave_region(waitingBuf);
    } 
    PCB[lastSelected]->blockedReasonCDT.size = size;
    PCB[lastSelected]->blockedReasonCDT.waitingBuf = waitingBuf;
    forceTimerInt();
}

int addSemToPCB(char * name, int pid){
    int pcbIndex = (pid != 0) ? searchProcessByPid(pid) : lastSelected;
    if (pcbIndex == -1){
        return -1;
    }
    for (int i = 0; i < MAX_SEM_PER_PROCESS; i++){
        if (PCB[pcbIndex]->sems[i] == NULL){
            PCB[pcbIndex]->sems[i] = name;
            PCB[pcbIndex]->lastSemOpen++;
            return 0;
        }
    }
    return -1;
}

int deleteSemFromPCB(char * name, int pid){
    int pcbIndex = (pid != 0) ? searchProcessByPid(pid) : lastSelected;
    if (pcbIndex == -1){
        return -1;
    }
    for (int i = 0; i < PCB[pcbIndex]->lastSemOpen; i++){
        if (strCmp(PCB[pcbIndex]->sems[i], name) == 0){
            PCB[pcbIndex]->sems[i] = NULL;
            return 0;
        }
    }
    return -1;
}

void blockProcess(int pid, uint16_t blockReason){
    // doy prioridad para disminuir la posibilidad de que el proximo
    // proceso que escoja el scheduler sea el que se quiere bloquear    
    PCB[lastSelected]->priority = 1;
    
    if ( pid == PCB[lastSelected]->pid || pid == RUNNING ){
            PCB[lastSelected]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[lastSelected]->blockedReasonCDT.blockReason = blockReason;
            forceTimerInt();
            return;
    }

    int i = searchProcessByPid(pid);
        if ( i>0){
            PCB[i]->state = BLOCKED;
            // aviso q info espera en struct
            PCB[i]->blockedReasonCDT.blockReason = blockReason;
            return;
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
        void * buf = getFdBuffer(0, STDIN);
        if (buf == &buffer){
            *buffer = -1;
            return;
        }
        else {
            pipeADT pipe = (pipeADT) buf;
            char c = -1;
            writePipe(pipe, &c, 1);
            return;
        }
        buf = getFdBuffer(0, STDOUT);
        if (buf != BASEDIRVIDEO) {
            pipeADT pipe = (pipeADT) buf;
            closePipe(pipe);
        }

    }else if ( signal == CTRLC){
        deleteFromScheduler(getForegroundPid());
    }
}


void unblockProcess(int pid){
    int i = searchProcessByPid(pid);
    
    if (i > 0 ){
        PCB[i]->state = READY;
        PCB[i]->priority = 1;
    }
    
}

void updateRunningPriority(unsigned lastTicks){

    if ( !PCB[lastSelected]->ticketsBeforeLoosingPrior){
        PCB[lastSelected]->priority = QUANTUM - lastTicks; 
        return;
    }
    
    PCB[lastSelected]->ticketsBeforeLoosingPrior--;
    return;
}

void updatePriority(int pid, int priority){
    //!  NO VA A IMPORTARRRR
    if (!pid){
            PCB[lastSelected]->ticketsBeforeLoosingPrior = TICKETS_BEFORE_LOOSING_PRIOR*(priority+1);
            PCB[lastSelected]->priority = priority;
            return;
    }

    int i = searchProcessByPid(pid);
    if (i>0){
        PCB[i]->priority = priority;
        PCB[i]->ticketsBeforeLoosingPrior = TICKETS_BEFORE_LOOSING_PRIOR*(priority+1);
        return;
        // aviso q info espera en struct
    }

}

void createNewPipe(char ** params1, char ** params2){
//int execve(void * ptrFunction, char isForeground, int argc, char ** argv )
// params {ptrFunction, isForeground, argc, argv..., ptrFunction, isForeground, argc, argv..., }
    void * ptrfunction1 = (void *) params1[0];
    char isForeground1 = (char) params1[1];
    int argc1 = (int) params1[2];
    char ** argv1 ;
//! argv1 y argv2 no tienen espacio reservado

//! este for es el q genera la invalid operation
// => no estoy pasando como argum argv1 ni 2 a execve
// pero en teoria con asignarles espacio con malloc va a funcionar
  /*
    for (int i = 0; i < argc1; i++){
        argv1[i] = params1[3+i];
    }
*/
    void * ptrfunction2 = params2[0];//3+argc1
    char isForeground2 = params2[1];
    int argc2 = params2[2];
    char ** argv2 ;
/*
    for (int i = 0; i < argc2; i++){
        argv2[i] = params2[i+3];
    }
*/
    pipeADT pipe = createPipe();
    PCB[1]->fds[1] = (void *) pipe;
    int pid1 = execve(ptrfunction1, isForeground1, argc1, 0x0);
    PCB[1]->fds[1] = BASEDIRVIDEO;
    PCB[1]->fds[0] = pipe;
    int pid2 = execve(ptrfunction2, isForeground2, argc2, 0X0);
    PCB[1]->fds[0] = &buffer;
    //pipe->writePid = pid1;
    //pipe->readPid = pid2;
}

void initializeScheduler(){
        nextPid = 0;
        lastSelected =0;
       
        int sizeEntry = 20 * 8; //sizeEntry = sizeof(pcbEntryCDT); //128 = 15 *8 cm antes
        
        PCB[0] = allocMemory( sizeEntry*MAX_SIZE_PCB );
        PCB[0]->state = TERMINATED;
        
        //chequeado que funciona
        for ( int i=1; i<MAX_SIZE_PCB; i++){
            PCB[i] = (char*) PCB[i-1] + sizeEntry;
            PCB[i]->state = TERMINATED;
            PCB[i]->fds[0] = &buffer;
            PCB[i]->fds[1] = BASEDIRVIDEO;
        }
}

void * scheduler(void * stackPointer, unsigned lastTicks ){
    
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
    if ( !halt ){
        PCB[lastSelected]->stackPointer = stackPointer;
        updateRunningPriority(lastTicks);
    }
    int i, found = 0;

    for ( i=lastWithPrior+1; i!=lastWithPrior; i++){
        if ( i==MAX_SIZE_PCB){
            i=1;
            if ( i==lastWithPrior) 
                break; 
        }
        if ( PCB[i]->priority && PCB[i]->state<=READY){
            found = 1;
            lastWithPrior = i;
            break;
        }
    }
    // i = lasWithPrior si !found
    /* 1. Si, found => sigo */
    /* 2. check si ultimo ready */

    if ( !found ) {
        // se mantiene lastWithPrior
        if ( PCB[lastWithPrior]->priority && PCB[lastWithPrior]->state <= READY )
            found = 1; 
        else {
    //if (  lastSelected==i && (PCB[i]->state>READY || !PCB[i]->priority) )//&& || !PCB[lastSelected]->priority ) lo saco xq no se si tiene inanicion
            for ( i=lastNormal+1; i!=lastNormal; i++ ){
                if ( i==MAX_SIZE_PCB){
                    i=1;
                    if ( i==lastNormal)
                        break;
                }
                if ( PCB[i]->state<=READY ){ // si pongo priority==0 => va a ser 1 comparacion +
                                            // => lo evito total si ready y de prior 1, lo agarraria =
                    lastNormal = i; 
                    found = 1; 
                    break;
                }
            }
        // si llegue a lastNormal==i => se mantiene lastNormal, no hay q actualizarlo

        // sale <=> found || i==lastNormal

            // como antes, no encontre nada 
            if ( !found && PCB[lastNormal]->state > READY ) {
            //if (  (i==lastSelected && PCB[lastSelected]->state > READY) || (i == lastNormal && PCB[lastNormal]->state > READY)) {
            //if ( i==lastSelected && (PCB[lastSelected]->state == TERMINATED || PCB[lastSelected]->state == BLOCKED) ){
                    halt = 1; 
                    return PCB[0]->stackPointer; 
            }
        }
    }
    // Si es el =, se v an a pisar => evi comparacion 
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
    int parent = searchProcessByPid( PCB[index]->parentPid );
    if ( parent<0 )
        return;
    PCB[parent]->countChildren--;
    if ( PCB[parent]->state == BLOCKED && ( PCB[parent]->blockedReasonCDT.blockReason== BLOCKBYWAITCHILDREN && PCB[parent]->countChildren==0) || 
    ( PCB[parent]->blockedReasonCDT.blockReason==BLOCKBYWAITCHILD && PCB[parent]->blockedReasonCDT.size == PCB[lastSelected]->pid ) )
        PCB[parent]->state = READY;
    // no fuerzo interrupt pues el hijo al morir la genera
}


//flujo de estados:
// INIT (PID = PARENTPID = 1)
// Arranca en running pero despues siempre
//* RUNNING asi se evita syscall getpid 
int deleteFromScheduler(uint16_t pid){
    PCB[lastSelected]->priority = 1;

    if ( pid == lastSelected || pid == RUNNING){
            /* evito que scheduler lo agarre si no estaba corriendo */
            if ( pid != RUNNING)
                PCB[lastSelected]->state = TERMINATED;
            
            /* aviso al padre */
            deadChild(lastSelected);

            // PCB[lastSelected]->state = TERMINATED;
            // freeMemory(PCB[lastSelected]->topMemAllocated);
            
            if (PCB[lastSelected]->fds[0] != &buffer){
                closePipe(PCB[lastSelected]->fds[0]);
            }
            if (PCB[lastSelected]->fds[1] != BASEDIRVIDEO){
                closePipe(PCB[lastSelected]->fds[1]);
            }
            /* cierro semaforos */
            for ( int j=0; j<PCB[lastSelected]->lastSemOpen; j++)
                if ( PCB[lastSelected]->sems[j] != NULL)
                    closeSem( PCB[lastSelected]->sems[j] ); 
            
            /* lo dejo al final por si hay una interrupcion => lo vuelve a agarrar */
            PCB[lastSelected]->state = TERMINATED;
            freeMemory(PCB[lastSelected]->topMemAllocated);
            forceTimerInt();
            return 0;
    }

    int i = searchProcessByPid(pid);
        if (i > 0){
            PCB[i]->state = TERMINATED;
            deadChild(i);
            if (PCB[i]->fds[0] != &buffer){
                closePipe(PCB[i]->fds[0]);
            }
            if (PCB[i]->fds[1] != BASEDIRVIDEO){
                closePipe(PCB[i]->fds[1]);
            }
            /* cierro semaforos */
            for ( int j=0; j<PCB[i]->lastSemOpen; j++)
                if ( PCB[i]->sems[j] != NULL)
                    closeSem( PCB[i]->sems[j] );
            
            freeMemory(PCB[i]->topMemAllocated);
            return 0;
        }
    
    return -1;
}

void copyFdFromParent(int index){
    for (int i = 0; i < MAX_FD_PER_PROCESS; i++){
        PCB[index]->fds[i] = PCB[getPCBIndex(PCB[index]->parentPid)]->fds[i];
    }
}

void changeFd(int index, int fd, void * buffer){
    PCB[index]->fds[fd] = buffer;
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
            PCB[i]->ticketsBeforeLoosingPrior = 0;
            PCB[i]->stackPointer = stackPointer;
            PCB[i]->blockedReasonCDT.blockReason = 0;
            copyFdFromParent(i);
            for (int j = 0; j < MAX_SEM_PER_PROCESS; j++){
                PCB[i]->sems[j] = NULL;
            }
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
    return NULL;
}

int getPriority(int pid){
    for (int i = 1; i < MAX_SIZE_PCB; i++){
        if (PCB[i]->pid == pid){
            return PCB[i]->priority;
        }
    }
    return -1;
}

int getAllProcessInfo(stat arrayStats){
    // no incluimos al halt
    int j=0,i;
    
    PCB[lastSelected]->priority = 2;

    for ( i=1; i<MAX_SIZE_PCB; i++){
        if ( PCB[i]->state != TERMINATED ) {
            arrayStats[j].name = PCB[i]->name;
            arrayStats[j].pid = PCB[i]->pid;
            arrayStats[j].priority = PCB[i]->priority;
            arrayStats[j].state = PCB[i]->state;
            arrayStats[j].stackPointer = PCB[i]->stackPointer;
            arrayStats[j].basePointer = PCB[i]->basePointer;
            arrayStats[j++].isForeground = PCB[i]->isForeground;
        }
    }
    
    return j; 
}