// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "philosophers.h"
#include <library.h>

int n;                          /* numero de filosofos */
int nMax;
int state[MAX_NUM_PHILO];
char ** semPhi;
int pids[MAX_NUM_PHILO];
char * argvsToFree[MAX_NUM_PHILO];

#define LEFT(i,n) (i+n-1)%n
#define RIGHT(i,n) (i+1)%n 

void printInstructions(){
    printf("-------------------------------------\n");
    printf("Press \'a\' to add a philosopher\n");
    printf("Press \'r\' to remove a philosopher\n");
    printf("Press \'Ctrl d\' to end test\n");
    printf("-------------------------------------\n");
}

void initializePhilo(){
    
    printInstructions();
    printf("Preparing default philosophers..\n");

    if (my_sem_open(SEM_MUTEX_ID, 1) == -1) {
      printf("initialize: ERROR opening semaphore\n");
      return;
    }
    
    char * names[MAX_NUM_PHILO];
    char * space = malloc( MAX_NUM_PHILO*SIZE_NAME );
    for ( int i=0; i<MAX_NUM_PHILO; i++ ){
        space += SIZE_NAME*i;
        numToStr(i,10, space );
        names[i] = space;
    }
    semPhi = names; 

    n = DEFAULT_NUM_PHILO; 
    nMax = n;
    for (int i=0; i<n; i++){
        char ** argv = malloc( ARGV_SIZE );
        argv[0] = PHILO_NAME;
        argv[1] = names[i];
        pids[i] = execve(&philo, 1, 2, argv );
        argvsToFree[i] = argv;
    }
    
    char c; 
    while ( (c=getChar()) != EOFILE){
        if ( c == 'a')
            addPhilo();
        else if ( c == 'r')
            removePhilo();
    }

    my_sem_wait(SEM_MUTEX_ID);
    for ( int i=0; i<nMax; i++ ){
        /* sem se cierran con el kill */
        if ( i<n)
            kill(pids[i]);
        free( argvsToFree[i] );    
        my_sem_destroy(semPhi[i]);
    }
    my_sem_post(SEM_MUTEX_ID);

    my_sem_close(SEM_MUTEX_ID);
    my_sem_destroy(SEM_MUTEX_ID);
    free(space);

    printf("\nFin\n");
}

void removePhilo(){
    if ( n == 2 ){
        printf("\nError: There should be at least 2 philosophers\n");
        return;     
    }
    
    my_sem_wait(SEM_MUTEX_ID);
    state[--n] = THINKING;
    kill(pids[n]);
    my_sem_post(SEM_MUTEX_ID);
    
    free(argvsToFree[n]); 
}

void addPhilo(){
    if ( n==MAX_NUM_PHILO ){
        printf("\nMax amount of philosophers reached\n");    
        return;
    }
    char ** argv = malloc( ARGV_SIZE );
    argvsToFree[n] = argv;
    
    argv[0] = PHILO_NAME;
    argv[1] = semPhi[n];
    my_sem_wait(SEM_MUTEX_ID);
    n++;
    if ( n>nMax)
        nMax = n;
    my_sem_post(SEM_MUTEX_ID);
    pids[n-1] = execve(&philo, 1, 2, argv );
    
}


void philo(int argc, char ** argv){
    int i = satoi(argv[1]);
    printf("I am the new philosopher %d\n", i+1);

    /* añado mi sem */
    if (my_sem_open(semPhi[i], 0) == -1) {
      printf("philo %d: ERROR opening semaphore\n", i+1);
      exit();
    }

    /* abro solo los sem que le importan */   
    if (my_sem_open(SEM_MUTEX_ID, 1) == -1) {
      printf("philo %d: ERROR opening semaphore\n", i+1);
      exit();
    }

    if (my_sem_open(semPhi[LEFT(i,n)], 0) == -1) {
      printf("philo %d: ERROR opening semaphore\n", i+1);
      exit();
    }
    
    if (my_sem_open(semPhi[RIGHT(i,n)], 0) == -1) {
      printf("philo %d: ERROR opening semaphore\n", i+1);
      exit();
    }
    yield();
    while ( 1 ){
        /* thinks */
        takeForks(i);
        eat();
        putForks(i);
        bussy_wait(WAITTIME*(GetUint()%10+3 ));
    }
}

void eat(){

    my_sem_wait(SEM_MUTEX_ID);
    for ( int i=0; i<n; i++){
        if ( state[i]==EATING)
            printf("E");
        else
            printf(".");
    }
    printf("\n");
    my_sem_post(SEM_MUTEX_ID);   
    
    bussy_wait(WAITTIME*3 );
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
    yield();
}

/* estoy en mutex */
void test(int i){
    if ( state[i]==HUNGRY && state[LEFT(i,n)]!=EATING && state[RIGHT(i,n)]!=EATING){
        state[i] = EATING; 
        my_sem_post(semPhi[i]);
    }
}