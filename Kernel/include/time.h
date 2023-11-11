#ifndef TIME_H
#define TIME_H

#define QUANTUM 2

void * timer_handler(void * stackPointer);


void completeQuantum();

void forceScheduler();

#endif