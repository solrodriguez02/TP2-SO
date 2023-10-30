#include "syscalls.h"
#include <stdio.h>
#define STDIN 0
#define STDOUT 1
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
int execve(void* pointer, char isForeground);
void block(int pid);
void exit();
void yield();
void updatePriority(int pid);
int getPriority(int pid);
void waitChildren();