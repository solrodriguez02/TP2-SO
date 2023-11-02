//implementacion para un semaforo naive
typedef struct sem_t{
    int value;
} sem_t;

typedef sem_t * sem_ptr;

void * createSem();
void waitSem(sem_ptr sem);
void postSem(sem_ptr sem);