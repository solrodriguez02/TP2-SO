#ifndef _SYSCALLSH
#define _SYSCALLSH

#include <stdint.h>
#include <shellCommands.h>

extern unsigned char syscall_read(char * placeholder, unsigned char count);
extern void syscall_writeAt(unsigned char c, int x, int y, int FGColor, int BGColor);
extern void syscall_write(unsigned char c, int FGColor, int BGColor);
extern void syscall_nextLine();
extern void syscall_wait(int seconds);
extern void syscall_beep(uint32_t freq, uint32_t time);
extern void syscall_loadScreen();
extern void syscall_enableDoubleBuffer(int enable);
extern void syscall_time(char * timeStr);
extern void * syscall_malloc(unsigned int size);
extern void syscall_free(void * ptr);
extern void syscall_yield();
extern void syscall_waitChildren();
extern int syscall_getAllProcessInfo(stat stat);
extern int syscall_kill(int pid);
extern void syscall_waitChild(int pid);
extern int syscall_getPid();
extern int syscall_getStatus(int pid);
extern int syscall_kill(int pid);
extern int syscall_execve(void * pointer, int isForeground, int argc, char ** argv);
extern void syscall_block(int pid);
extern int syscall_update_priority(int pid, int priority);
extern int syscall_get_priority(int pid);
extern int syscall_getAllProcessInfo();
extern int syscall_createPipe(char **params1, char ** params2);
extern int syscall_openSem(char * name, int value);
extern int syscall_waitSem(char * name);
extern int syscall_postSem(char * name);
extern int syscall_closeSem(char * name);
extern int syscall_mem(char * buffer);
extern int syscall_getTotalMemory();
extern int syscall_destroySem(char * name);
extern void * syscall_openShm(char * name, int size);
#endif