#include "scheduler.h"
#include "MemoryManager.h"
#include <interrupts.h>

#define GPR 15 
#define SIZE_ADRESS 8   // 8bytes
#define INT_PUSHED_R 5
#define SS 0x0
#define RFLAGS 0x202
#define CS 0x8
#define SIZE_GPT SIZE_ADRESS * GPR 
#define SIZE_INT_PUSHED_R SIZE_ADRESS * INT_PUSHED_R
#define SIZE_INITIAL_STACK  SIZE_GPT + SIZE_INT_PUSHED_R

#define PROCESS_STACK_SIZE 4096 //4kb
#define POSTION_REL_STRUCT PROCESS_STACK_SIZE - SIZE_INT_PUSHED_R

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

// le paso el puntero a funcion asm para que pushee al stack
typedef processStackCDT * processStackADT;

// q mantenga pipes
void execve(void * ptrFunction ){
    // pido espacio 
    void * topMem = allocMemory(PROCESS_STACK_SIZE);
	
    // si no hay mas espa => ret -1
    if ( topMem == NULL )
        return;
    
    void * memForStack = topMem + POSTION_REL_STRUCT;
    processStackADT p = (processStackADT) memForStack; 
    p->ss = (void *) SS;
    // + SIZE_ADRESS para asegurar que no pisa los gpr
    p->rsp = (void *) p + SIZE_ADRESS*GPR + SIZE_ADRESS; 
    p->rflags = (void *) RFLAGS;
    p->rip = ptrFunction; 
    p->cs = (void *) CS; 

    addToScheduler( p->rsp );
}