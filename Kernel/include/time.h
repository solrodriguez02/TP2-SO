#ifndef TIME_H
#define TIME_H

void * timer_handler(void * stackPointer);

int ticks_elapsed();

int seconds_elapsed();

void restartTicks();

#endif