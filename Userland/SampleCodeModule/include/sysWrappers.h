#include <syscalls.h>
#include <stdint.h>

#define STDIN 0
#define STDOUT 1

#define FREE 1
#define BOUNDRY 3

int getChar();

int putCharAt(char c, int x, int y);
int putChar(char c, int FGColor);

void loadScreen();
void enableDoubleBuffer(int enable);

void getTime(char * timeStr);

void * malloc(unsigned int size); 
void free(void * ptr);
unsigned getMemStatus(char * state);
unsigned long getTotalMemory();

int getPid();
int getStatus(int pid);
int kill(int pid);
int execve(void* pointer, char isForeground, int argc, char ** argv);
void block(int pid);
void exit();
void yield();
int updatePriority(int pid, int priority );
int getPriority(int pid);
void waitChildren();
void waitChild(int pid);
int64_t my_getpid();
int64_t my_create_process(char *name, uint64_t argc, char *argv[]);
int64_t my_nice(uint64_t pid, uint64_t newPrio);
int64_t my_kill(uint64_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
int64_t my_sem_open(char *sem_id, uint64_t initialValue);
int64_t my_sem_wait(char *sem_id);
int64_t my_sem_post(char *sem_id);
int64_t my_sem_close(char *sem_id);
int my_sem_destroy(char *sem_id);
int64_t my_yield();
int64_t my_wait(int64_t pid);
