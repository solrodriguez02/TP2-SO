#include <semaphore.h>

int execve(void * ptrFunction, char isForeground, int argc, char ** argv );

void haltProcess();

void initializeSems();
int openSem(char * name, int value);
int closeSem(char * name);
sem_ptr getSemByName(char * name);
int getSemIndex(sem_ptr sem);