// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <time.h>
#include <keyboardDriver.h>
#include <clock.h>
#include <scheduler.h>

static void * int_20(void * stackPointer);
static void int_21(char character);
static void int_28();

/**
 * @brief Llama a los distintos Handlers dependiendo de la interrupción ocurrida.
 * 
 * @param irq Número de la interrupción ocurrida.
 */
void * irqDispatcher(uint64_t irq, uint64_t param1) {
	switch (irq) {
		case 0:
			return int_20((void *) param1);
        case 1:
            int_21( param1 );
            break;
		case 8:
			int_28();
			break;
	}
	return 0;
}

/**
 * @brief Función correspondiente a la interrupción número 20h, sobre el Timer Tick.
 */
void * int_20(void * stackPointer) {
	return timer_handler(stackPointer);
}

/**
 * @brief Función correspondiente a la interrupción número 21h, sobre el teclado.
 */
void int_21(char character) {
	if ( keyboard_handler(character)== 0)
		return;
	
	tryToUnlockRead(1);

}

/**
 * @brief Función correspondiente a la interrupción número 28h, sobre el módulo de RTC.
 */
void int_28() {
	rtc_handler();
}
