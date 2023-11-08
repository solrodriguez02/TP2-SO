#include <time.h>
#include <scheduler.h>

static unsigned ticks = 0;
static char forced = 0;
static unsigned lastTicks =0;
/**
 * @brief Handler para la interrupción del Timer Tick. Incrementa constantemente el valor de ticks.
 */
void * timer_handler(void * stackPointer) {
	//updateTicks(0, ticks_before_quantum());
	
	//updateTicks(0, ticks );
	//mod = ticks% QUANTUM; 
	if ( ticks == QUANTUM || forced ) {
		//updateRunningPriority(0,ticks);
		lastTicks = ticks;
		/*
		mod = ticks% QUANTUM; 
		if ( mod == 0 || forced ) { 
			...
		if ( mod )
			ticks += ticks % QUANTUM; 
		ticks++;
		*/
		ticks = 0; 
		forced = 0;
		return scheduler( stackPointer, lastTicks );
	}
	ticks++;

	return stackPointer;
}

/**
 * @brief Retorna la cantidad de ticks transcurridos.
 * 
 * @return Cantidad de ticks transcurridos.
 */
int ticks_elapsed() {
	return ticks;
}

void restartTicks(){
	//ticks =0; se hace solo xq entra al if x el forced
	forced = 1;
	//ticks += ticks % QUANTUM; 
}


int ticks_before_quantum(){
	return ticks_elapsed()%QUANTUM;
}
/**
 * @brief Retorna la cantidad de segundos transcurridos a partir de los ticks.
 * 
 * @return Cantidad de segundos transcurridos.
 */
int seconds_elapsed() {
	return ticks / 18;
}
