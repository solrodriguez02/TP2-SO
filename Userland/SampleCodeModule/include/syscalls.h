#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>

extern unsigned char syscall_read();
extern long int * syscall_getRegisters();
extern void syscall_writeAt(unsigned char c, int x, int y, int FGColor, int BGColor);
extern void syscall_write(unsigned char c, int FGColor, int BGColor);
extern void syscall_nextLine();
extern void syscall_wait(int seconds);
extern void syscall_beep(uint32_t freq, uint32_t time);
extern void syscall_loadScreen();
extern void syscall_enableDoubleBuffer(int enable);
extern void syscall_time(char * timeStr);
extern void syscall_getKeyboardState(char * keys);
extern void syscall_drawCircle(int x_centre, int y_centre, int r, uint8_t color);
extern void syscall_drawRectangle(int x, int y, int h, int w, uint8_t color);


#endif