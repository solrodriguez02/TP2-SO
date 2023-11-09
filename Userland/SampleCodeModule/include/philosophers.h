#ifndef _PHILO_H
#define _PHILO_H

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define DEFAULT_NUM_PHILO 3
#define MORE_MEM_SPACE 20*8
#define ARGV_SIZE 15

#define SEM_MUTEX_ID "mutexPhilo"


/* builtin shell */
/* => no se corre en bg */
void initializePhilo();
void philo(int argc, char ** argv);
void test(int i);
void putForks(int i);
void takeForks(int i);
void eat();
void addPhilo();
void removePhilo();

#endif