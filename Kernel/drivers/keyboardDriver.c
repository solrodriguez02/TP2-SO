#include <keyboardDriver.h>

const unsigned char ascii[TOTAL_SCANCODES][2] = {
	{  0, 0  }, { 27, 27 } , {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'},
	{'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {127, 127}, {  9, 9  },
	{'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'},
	{'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n','\n'}, { 0, 0  }, {'a', 'A'}, {'s', 'S'},
	{'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'},
	{'\'', '"'}, {'`', '~'}, { 0, 0 }, {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'},
	{'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, {  0, 0  }, {'*', '*'},
	{  0, 0  }, {' ', ' '}, {  0, 0  }, {  0, 0  }, {  0, 0  }, {  0, 0  }, {  0, 0  }, {  0, 0  },
	{  0, 0  }, {  0, 0  }, {  0, 0  }, {  0, 0  }, {  0, 0  }, {  0, 0  }, {  0, 0  }, {'7', '7'},
	{'8', '8'}, {'9', '9'}, {'-', '-'}, {'4', '4'}, {'5', '5'}, {'6', '6'}, {'+', '+'}, {'1', '1'},
	{'2', '2'}, {'3', '3'}, {'0', '0'}, {'.', '.'}			        
};


static int shiftActivated = 0;
static int capslockActivated = 0;
static unsigned char character;
static char ctrlActivated = 0;

/**
 * @brief Handler de la interrupción de teclado. Retorna el valor ASCII obtenido del buffer en el mapa de
 * entrada y salida.
 * 
 * @return Código ASCII correspondiente al make-code leído del mapa de entrada y salida.
 */
char keyboard_handler() {
	character = readScanCode();
	checkConditions(character);
	return scanCodeToASCII(character);
}

/**
 * @brief Convierte el valor leído del mapa de entrada y salida al ASCII correspondiente.
 * 
 * @param scanCode Valor leído del mapa de entrada y salida.
 * @return Valor ASCII correspondiente.
 */
unsigned char scanCodeToASCII(unsigned char scanCode) {
	if(scanCode & 0x80)
		return 0;
    if(getCapslock()) {
        if(isLetter(scanCode)) {
            return ascii[scanCode][!getShift()];
        }
    }
    /*if( scancode de D & ctrlActivated)
        excep
    */
    

    return ascii[scanCode][getShift()];
}

/**
 * @brief Cambia los valores booleanos acerca de si alguna de las teclas está combinada.
 * 
 * @param scanCode Valor leído del mapa de entrada y salida.
 */
void checkConditions(unsigned char scanCode) {
    if(scanCode == SHFT_DOWN || scanCode == RSHFT_DOWN) {
        shiftActivated = 1;
    }
    else if(scanCode == SHFT_UP||scanCode == RSHFT_UP) {
        shiftActivated = 0;
    }
    else if(scanCode == CAPSLOCK_DOWN) {
        capslockActivated = !capslockActivated;
    } 
    //PARA CTRL+D y CTRL+C
    else if ( scanCode== CTRL_KEY_DOWN ) 
        ctrlActivated = 1;
    else if ( scanCode== CTRL_KEY_UP ) 
        ctrlActivated = 0;
}

/**
 * @brief Retorna el valor booleano si el caracter leído es una letra minúscula.
 * 
 * @param scanCode Valor leído del mapa de entrada y salida.
 * @return Valor booleano de si el caracter leído es una letra minúscula.
 */
int isLetter(unsigned char scanCode) {
    unsigned char character = ascii[scanCode][0];
    return character >= 'a' && character <= 'z';
}

/**
 * @brief Retorna el valor booleano si la tecla SHIFT está activa.
 * 
 * @return Valor booleano si la tecla SHIFT está activa.
 */
int getShift() {
    return shiftActivated;
}

/**
 * @brief Retorna el valor booleano si la tecla CAPS_LOCK está activa.
 * 
 * @return Valor booleano si la tecla CAPS_LOCK está activa.
 */
int getCapslock() {
    return capslockActivated;
}
