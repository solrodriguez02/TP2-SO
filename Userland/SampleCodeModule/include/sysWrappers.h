#include "syscalls.h"
#include <stdio.h>
#define STDIN 0
#define STDOUT 1

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

typedef struct statProcess * stat;

int getChar();

int putCharAt(char c, int x, int y);
int putChar(char c, int FGColor);

void sleep(int seconds);
void loadScreen();
void enableDoubleBuffer(int enable);

void getTime(char * timeStr);

void * malloc(unsigned int size); 
void free(void * ptr);

extern int syscall_getpid();
extern int syscall_getstatus(int pid);
int getPid();
int getStatus(int pid);
int kill(int pid);
int execve(void* pointer, char isForeground, int argc, char ** argv);
void block(int pid);
void exit();
void yield();
void updatePriority(int pid);
int getPriority(int pid);
void waitChildren();
int getAllProcessInfo(stat * arrayStats);