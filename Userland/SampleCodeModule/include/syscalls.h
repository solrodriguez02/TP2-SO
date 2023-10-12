#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

extern unsigned char syscall_read();
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
#endif