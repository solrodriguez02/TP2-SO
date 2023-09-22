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
static char altTouched = 0; 
static unsigned char character;


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
    return ascii[scanCode][getShift()];
}

/**
 * @brief Cambia los valores booleanos acerca de si alguna de las teclas está combinada.
 * 
 * @param scanCode Valor leído del mapa de entrada y salida.
 */
void checkConditions(unsigned char scanCode) {
    if (scanCode == ALT_KEY){
        altTouched = 1; 
    } 
    if(scanCode == SHFT_DOWN || scanCode == RSHFT_DOWN) {
        shiftActivated = 1;
    }
    else if(scanCode == SHFT_UP||scanCode == RSHFT_UP) {
        shiftActivated = 0;
    }
    else if(scanCode == CAPSLOCK_DOWN) {
        capslockActivated = !capslockActivated;
    }
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

/**
 * @brief Retorna un valor booleano si la tecla ALT fue usada recientemente.
 * 
 * @return Valor booleano (1 o 0) si la tecla fue usada recientemente.
 */
char getAltTouched(){ return altTouched;}

/**
 * @brief Reinicia al valor falso a la variable booleana de si la tecla ALT fue usada recientemente.
 * 
 */
void resetAltTouched(){ altTouched = 0;}

/**
 * @brief Actualiza los valores del vector de teclas utilizada para el minijuego.
 * 
 * @param up Vector de caracteres para las teclas utilizadas para el minijuego.
 */
void getKeyboardState(char * up) {
   int c = character;
    switch (c)
    {
        case UPL_KEY:
            up[1] = 1;
            break;
        case DOWNL_KEY:
            up[2] = 1;
            break;
        case UPR_KEY:
            up[3] = 1;
            break;
        case DOWNR_KEY:
            up[4] = 1;
            break;
        case BREAK_UPL_KEY:
            up[1] = 0;
            break;
        case BREAK_DOWNL_KEY:
            up[2] = 0;
            break;
        case BREAK_UPR_KEY:
            up[3] = 0;
            break;
        case BREAK_DOWNR_KEY:
            up[4] = 0;
            break;
        case ESC_KEY:
            up[0] = 1;
            break;
    }
}