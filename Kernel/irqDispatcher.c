#include <stdint.h>
#include <time.h>
#include <keyboardDriver.h>
#include <clock.h>

static void * int_20(void * stackPointer);
static void int_21();
static void int_28();

/**
 * @brief Variable global sobre la cual se actualiza la tecla presionada sobre el teclado por el usuario.
 */
char buffer;

/**
 * @brief Llama a los distintos Handlers dependiendo de la interrupción ocurrida.
 * 
 * @param irq Número de la interrupción ocurrida.
 */
void * irqDispatcher(uint64_t irq, void * stackPointer) {
	switch (irq) {
		case 0:
			return int_20(stackPointer);
        case 1:
            int_21();
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
	// 1 pues sys no bloquante
	// ya sys fue llamada con int => = interrup
	// => stack tmb cargado 
	return timer_handler(stackPointer);
}

/**
 * @brief Función correspondiente a la interrupción número 21h, sobre el teclado.
 */
void int_21() {
	buffer = keyboard_handler();
}

/**
 * @brief Función correspondiente a la interrupción número 28h, sobre el módulo de RTC.
 */
void int_28() {
	rtc_handler();
}
