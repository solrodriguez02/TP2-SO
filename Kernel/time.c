#include <time.h>
#include "scheduler.h"

static unsigned long ticks = 0;

/**
 * @brief Handler para la interrupción del Timer Tick. Incrementa constantemente el valor de ticks.
 *! wasForced para sys bloq? 
 */
void * timer_handler(void * stackPointer) {
	if ( ticks%100 == 0 ) 
		return scheduler( stackPointer );
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
	ticks =0; 
}

/**
 * @brief Retorna la cantidad de segundos transcurridos a partir de los ticks.
 * 
 * @return Cantidad de segundos transcurridos.
 */
int seconds_elapsed() {
	return ticks / 18;
}
