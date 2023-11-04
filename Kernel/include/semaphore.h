//implementacion para un semaforo naive

typedef struct sem_t * sem_ptr;

sem_ptr createSem(char * name, int value);
void waitSem(sem_ptr sem);
void postSem(sem_ptr sem);
void destroySem(sem_ptr sem);
int getSemValue(sem_ptr sem);
char * getSemName(sem_ptr sem);	