#ifndef TIME_H
#define TIME_H

#define QUANTUM 2

void * timer_handler(void * stackPointer);

int ticks_elapsed();

int seconds_elapsed();

void restartTicks();

void completeQuantum();

int ticks_before_quantum();

#endif