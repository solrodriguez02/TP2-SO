// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <scheduler.h>
#include <MemoryManager.h>
#include <manageProcess.h>
#include <interrupts.h>
#include <lib.h>

#define GPR 15 
#define SIZE_ADRESS 8   /* 8 bytes */
#define INT_PUSHED_R 5
#define SS 0x0
#define RFLAGS 0x202
#define CS 0x8

#define SIZE_GPT SIZE_ADRESS * GPR
#define SIZE_INT_PUSHED_R SIZE_ADRESS * INT_PUSHED_R
#define SIZE_INITIAL_STACK  SIZE_GPT + SIZE_INT_PUSHED_R

#define PROCESS_STACK_SIZE 4096  /* 4kb */
#define POSTION_REL_STRUCT PROCESS_STACK_SIZE - SIZE_INT_PUSHED_R

#define RDI_REL_POSITION 6 * SIZE_ADRESS
#define RSI_REL_POSITION RDI_REL_POSITION - SIZE_ADRESS

#define SIZE_ARRAY_SEMS MAX_SEM_PER_PROCESS * MAX_SIZE_PCB


typedef struct processStackCDT{
    void *  rip;
    void *  cs;
    void *  rflags;
    void *  rsp;
    void *  ss;
} processStackCDT;

typedef processStackCDT * processStackADT;

int lastSemCreated = -1;
sem_ptr sems[SIZE_ARRAY_SEMS];


int execve(void * ptrFunction, char isForeground, int argc, char ** argv ){
    
    void * topMem = allocMemory(PROCESS_STACK_SIZE);
	
    if ( topMem == NULL )
        return -1;
    
    void * memForStack = topMem + POSTION_REL_STRUCT;
    processStackADT p = (processStackADT) memForStack; 
    
    p->ss = (void *) SS;

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
        if ( sems[i]!=NULL && strCmp(name, getSemName(sems[i]))){
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
        sems[i] = NULL;
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
            if ( sems[i]==NULL )
                break;
        }
        /* si no hay mas espacio disponible */
        if ( i==lastSemCreated && sems[i]!=NULL)
            return -1; 
        lastSemCreated = i;
        sems[i] = createSem(name, value);
        sem = sems[i];
    }
    
    if ( (i=addSemToPCB(name, RUNNING)) >= 0){
        return 0; 
    }
    return -1;
}

int closeSemSyscall(char * name){
    sem_ptr sem = getSemByName(name);
    if (sem && deleteSemFromPCB(name, RUNNING)==0 ){
        return 0;
    }
    return -1;
}

int destroySemSyscall(char * name){
    sem_ptr sem = getSemByName(name);

    if ( sem ){
        sems[ getSemIndex(sem) ] = NULL;           
        destroySem(sem);
        return 0; 
    }       
    return -1;
}