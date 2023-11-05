#include <semaphore.h>

int execve(void * ptrFunction, char isForeground, int argc, char ** argv );

void haltProcess();

void initializeSems();
sem_ptr getSemByName(char * name);
int openSem(char * name, int value);
int closeSem(char * name);