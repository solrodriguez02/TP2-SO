// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <time.h>
#include <scheduler.h>

static unsigned ticks = 0;
static char forced = 0;
static unsigned lastTicks =0;
/**
 * @brief Handler para la interrupción del Timer Tick. Incrementa constantemente el valor de ticks.
 */
void * timer_handler(void * stackPointer) {

	if ( ticks == QUANTUM || forced ) {
 
		lastTicks = ticks;
		ticks = 0; 
		forced = 0;
		return scheduler( stackPointer, lastTicks );
	}
	ticks++;

	return stackPointer;
}


void forceScheduler(){
	forced = 1;
}

void completeQuantum(){
	ticks = QUANTUM; 
}
