#include "syscalls.h"
#include <stdio.h>

int getChar();

int putCharAt(char c, int x, int y);
int putChar(char c, int FGColor);

void sleep(int seconds);
void loadScreen();
void enableDoubleBuffer(int enable);

void getTime(char * timeStr);

void * malloc(unsigned int size); 
void free(void * ptr);
