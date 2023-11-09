#include "philosophers.h"
#include <library.h>

//! EOF deberia def en library
int n=DEFAULT_NUM_PHILO;      /* numero de filosofos */
int * state;
char * semPhi;
int pids[DEFAULT_NUM_PHILO*8];
#define LEFT(i,n) (i+n-1)%n
#define RIGHT(i,n) (i+1)%n 

#define PHILO_NAME "philo"
#define POS_NUM_PHILO 5
//! ojo con n, hay q protejerlo 
//! solo shell lo altera, pero el resto lo usa para calcular LEFT, RIGHT 
//! => lo debo alterar en mutex

/* builtin shell */
/* => no se corre en bg */
void initializePhilo(){
    printf("Preparo filosofos default\n");
    state = malloc(DEFAULT_NUM_PHILO*MORE_MEM_SPACE);
    if (my_sem_open(SEM_MUTEX_ID, 1) == -1) {
      printf("test_sync: ERROR opening semaphore\n");
      exit();
    }
    for (int i=0; i<n; i++){
        char ** argv = malloc( ARGV_SIZE );
        argv[0] = PHILO_NAME;

        numToStr(i,10,argv+10);    //!no se si lo va a tomar
                                 // si lo desreferencio podria obtener i 
        argv[1] = argv+10;
        pids[i] = execve(&philo, 1, 2, argv );
    }
    
    char c; 
    while ( (c=getChar()) != EOF){
        if ( c == 'a')
            addPhilo();
        if ( c == 'r')
            removePhilo();
    }

    //todo: matar a todos los hijos 
}

void removePhilo(){
    int aux;
    my_sem_wait(SEM_MUTEX_ID);
    aux = --n;
    my_sem_post(SEM_MUTEX_ID);
    kill(pids[aux]);
}

void addPhilo(){
    if ( DEFAULT_NUM_PHILO*MORE_MEM_SPACE == n*MORE_MEM_SPACE){
        int * aux = malloc(DEFAULT_NUM_PHILO*MORE_MEM_SPACE*DEFAULT_NUM_PHILO);
        memcpy(aux,state,DEFAULT_NUM_PHILO*MORE_MEM_SPACE);
        free(state);
        state = aux; 
        char * aux2 = malloc(DEFAULT_NUM_PHILO*MORE_MEM_SPACE*DEFAULT_NUM_PHILO);
        memcpy(aux,semPhi,DEFAULT_NUM_PHILO*MORE_MEM_SPACE);
        free(semPhi);
        semPhi = aux2;
        
    }
    char ** argv = malloc( ARGV_SIZE );
    argv[0] = "philo";
    //!no se si lo va a tomar
    argv[1] = (char) n++;
    pids[n-1] = execve(&philo, 1, 2, argv );
}


void philo(int argc, char ** argv){
    int i = satoi(argv[1]);
    printf("Soy el filo %d", i);
    /* añado mi sem */
    char name[3]; 
    numToStr(i, 10, name);
    semPhi[i] = name;
    if (my_sem_open(semPhi[i], 0) == -1) {
      printf("test_sync: ERROR opening semaphore\n");
      exit();
    }
    yield();
    yield();
    yield();
    yield();
    /* incorpora los sem */
    /* abro solo los sem q me importan */   
    /* o podria hacerlo siempre q vea los costados = muy ineficiente */
    if (my_sem_open(semPhi[LEFT(i,n)], 0) == -1) {
      printf("test_sync: ERROR opening semaphore\n");
      exit();
    }
    if (my_sem_open(semPhi[RIGHT(i,n)], 0) == -1) {
      printf("test_sync: ERROR opening semaphore\n");
      exit();
    }
    
    if (my_sem_open(SEM_MUTEX_ID, 1) == -1) {
      printf("test_sync: ERROR opening semaphore\n");
      exit();
    }
    
    while ( 1 ){
        // thinks
        takeForks(i);
        // eat
        eat();
        putForks(i);

    }
}

/* para poder imprimir */
/* si o si deberia haber 1 comiendo */
void eat(){
    yield();
    my_sem_wait(SEM_MUTEX_ID);
    for ( int i=0; i<n; i++){
        if ( state[i]==EATING)
            printf("E");
        else
            printf(".");
    }
    printf("\n");
    my_sem_post(SEM_MUTEX_ID);   

}

void takeForks(int i){
    my_sem_wait(SEM_MUTEX_ID);
    state[i] = HUNGRY; 
    test(i);
    my_sem_post(SEM_MUTEX_ID);   
    my_sem_wait(semPhi[i]);
}

void putForks(int i){
    my_sem_wait(SEM_MUTEX_ID);
    state[i] = THINKING; 
    test(LEFT(i,n));
    test(RIGHT(i,n));
    my_sem_post(SEM_MUTEX_ID);   
}

/* estoy en mutex */
void test(int i){
    if ( state[i]==HUNGRY && state[LEFT(i,n)]!=EATING && state[RIGHT(i,n)]!=EATING){
        state[i] = EATING; 
        my_sem_post(semPhi[i]);
    }
}