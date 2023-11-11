#ifndef _PHILO_H
#define _PHILO_H

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define DEFAULT_NUM_PHILO 5
#define MAX_NUM_PHILO 12
#define ARGV_SIZE 15

#define SEM_MUTEX_ID "mutexPhilo"
#define PHILO_NAME "philo"
#define SIZE_NAME 5

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