#include <scheduler.h>
#include <MemoryManager.h>
#include <manageProcess.h>
#include <interrupts.h>
#include <lib.h>

#define GPR 15 
#define SIZE_ADRESS 8   // 8bytes
#define INT_PUSHED_R 5
#define SS 0x0
#define RFLAGS 0x202
#define CS 0x8

// SIZES estan en bits
#define SIZE_GPT SIZE_ADRESS * GPR 
#define SIZE_INT_PUSHED_R SIZE_ADRESS * INT_PUSHED_R
#define SIZE_INITIAL_STACK  SIZE_GPT + SIZE_INT_PUSHED_R

#define PROCESS_STACK_SIZE 4096 //4kb
#define POSTION_REL_STRUCT PROCESS_STACK_SIZE - SIZE_INT_PUSHED_R

#define RDI_REL_POSITION 6 * SIZE_ADRESS // (QUEDO ARRIBA)
#define RSI_REL_POSITION RDI_REL_POSITION - SIZE_ADRESS

#define SIZE_ARRAY_SEMS MAX_SEM_PER_PROCESS * MAX_SIZE_PCB
#define FREE 0x0

typedef unsigned long long u_int64_t;

typedef struct processStackCDT{
   //15 reg 
    void *  rip;
    void *  cs;
    void *  rflags;
    void *  rsp;
    void *  ss;
    //align
} processStackCDT;

int lastSemCreated = -1;

sem_ptr sems[SIZE_ARRAY_SEMS];

// le paso el puntero a funcion asm para que pushee al stack
typedef processStackCDT * processStackADT;

// q mantenga pipes
int execve(void * ptrFunction, char isForeground, int argc, char ** argv ){
    // pido espacio 
    void * topMem = allocMemory(PROCESS_STACK_SIZE);
	
    // si no hay mas espa => ret -1
    if ( topMem == NULL )
        return -1;
    
    void * memForStack = topMem + POSTION_REL_STRUCT;
    processStackADT p = (processStackADT) memForStack; 
    
    p->ss = (void *) SS;
    // + SIZE_ADRESS para asegurar que no pisa los gpr
    p->rsp = (void *) p - SIZE_ADRESS*GPR; 

    int * rdi = (char *)memForStack - RDI_REL_POSITION ;
    char ** rsi = (char *)memForStack - RSI_REL_POSITION;
    
    *rdi = argc;
    *rsi = argv; 

    p->rflags = (void *) RFLAGS;
    p->rip = ptrFunction; 
    p->cs = (void *) CS; 

    return addToScheduler( p->rsp, argv[0], topMem, (char * )topMem + PROCESS_STACK_SIZE - 1, isForeground );
}

void haltProcess(){
    
    while(1){
        _hlt();
    }
}

/* -------------------------------- SEMAPHORES -------------------------------- */

sem_ptr getSemByName(char * name){
    int i;
    for (i = 0; i < SIZE_ARRAY_SEMS; i++){
        if ( sems[i]!=FREE && strCmp(name, getSemName(sems[i]))){
            return sems[i];
        }
    }
    return NULL;
}

int getSemIndex(sem_ptr sem){
    for (int i = 0; i < SIZE_ARRAY_SEMS; i++){
        if ( sems[i] == sem ){
            return i;
        }
    }
    return -1;    
}

void initializeSems(){
    int i;
    for (i = 0; i < SIZE_ARRAY_SEMS ; i++){
        sems[i] = FREE;
    }
}


int openSem(char * name, int value){
    int i;
    sem_ptr sem = getSemByName(name);
    if ( sem == NULL){
        for ( i=lastSemCreated+1; i!=lastSemCreated; i++ ){
            if ( i == SIZE_ARRAY_SEMS ){
                i=0;
                if ( i==lastSemCreated)
                    break;
            }
            if ( sems[i]==FREE )
                break;
        }
        /* si no hay mas espacio disponible */
        if ( i==lastSemCreated)
            return -1; 
        lastSemCreated = i;
        sems[i] = createSem(name, value);
        sem = sems[i];
    }
    
    if ( addSemToPCB(name, RUNNING) == 0){
        processConnected(sem);
        return 0;
    }
    return -1;
}

int closeSemSyscall(char * name){
    sem_ptr sem = getSemByName(name);
    if (sem){
        if ( deleteSemFromPCB(name, RUNNING) && disconnectProcess(sem) == 0 ){
                destroySem(sem);
                sems[ getSemIndex(sem) ] = FREE;           
        }
        return 0;
    }
    return -1;
}

int closeSem(char * name){
    sem_ptr sem = getSemByName(name);
    if (sem){
        if ( disconnectProcess(sem) == 0 ){
                destroySem(sem);
                sems[ getSemIndex(sem) ] = FREE;           
        }
        return 0;
    }
    return -1;
}
