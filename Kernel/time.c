#include <time.h>
#include <scheduler.h>

static unsigned long ticks = 0;
static char forced = 0;
static unsigned mod =0;
/**
 * @brief Handler para la interrupción del Timer Tick. Incrementa constantemente el valor de ticks.
 */
void * timer_handler(void * stackPointer) {
	//updateTicks(0, ticks_before_quantum());
	
	//updateTicks(0, ticks );
	mod = ticks% QUANTUM; 
	if ( mod == 0 || forced ) {
		updatePriority(0,mod);
		ticks++;
		forced = 0;
		return scheduler( stackPointer );
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
	//ticks =0; 
	forced = 1;
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
