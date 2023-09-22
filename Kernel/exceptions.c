#include <videodriver.h>
#include <keyboardDriver.h>
#include <lib.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPERATION_ID 6
#define REGISTERS_COUNT 17
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define RED 0xFF0000
#define LIGHT_RED 0xFF6666

extern long int registers_space[];

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
	drawString("The registers and RIP values in the moment the error occured are:", RED, BLACK);
	char * register_names[REGISTERS_COUNT] = {"RAX", "RBX", "RCX", "RDX", "RDI", "RSI", 
    "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP"};
	//uint64_t* register_values = getRegistersSpace();
	char buffer[20];
	int i;
    for (i=0; i<REGISTERS_COUNT; i++) {
        drawNextLine();
        drawString(register_names[i], LIGHT_RED, BLACK);
        drawString(" = ", WHITE, BLACK);
        numToStr(registers_space[i], 16, buffer);
        drawString(buffer, WHITE, BLACK);
        drawChar('h', WHITE, BLACK);
    }

	drawNextLine();
	drawString("Press 'Enter' to reload the Shell.", WHITE, BLACK);
	waitForEnter();
	drawNextLine();
	resetAltTouched();
}

