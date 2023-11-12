// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <videodriver.h>
#include <keyboardDriver.h>
#include <lib.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPERATION_ID 6
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define RED 0xFF0000
#define LIGHT_RED 0xFF6666


/**
 * @brief Escribe sobre la pantalla el mensaje de error para la excepción de divisón por cero.
 */
static void zero_division() {
	char* msg = "Error: invalid parameters: divisor cannot be 0";
	drawString(msg, RED, BLACK);
}

/**
 * @brief Escribe sobre la pantalla el mensaje de error para la excepción de operación inválida.
 */
static void invalid_operation() {
	char* msg = "Error: invalid operation";
	drawString(msg, RED, BLACK);
}

/**
 * @brief Llama a los distintos Handlers de las excepciones dependiendo del parámetro recibido:
 * división por cero o operación inválida. Además, imprime en pantalla el estado de los registros
 * en el momento que se produjo la excepción.
 * 
 * @param exception Número de excepción recibido.
 */
void exceptionDispatcher(int exception) {
	drawNextLine();
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
	else if (exception == INVALID_OPERATION_ID){
		invalid_operation();
	}
	drawNextLine();
}

