#include <stdint.h>
#include <interrupts.h>
#include <videodriver.h>
#include <speakerDriver.h>
#include <keyboardDriver.h>
#include <time.h>
#include <clock.h>

extern char buffer;
extern long int registers_space[];

/**
 * @brief Retorna el valor ASCII guardado en la variable buffer que se modifica con la interrupción 21h.
 * 
 * @return Valor ASCII guardado en la variable buffer.
 */
static unsigned char read() {
    _sti();
    buffer = 0; 
    while ( buffer == 0);
    return buffer;
}

/**
 * @brief Escribe sobre la pantalla un caracter con su color deseado.
 * 
 * @param c Caracter al cual se quiere escribir sobre la pantalla.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param BGColor Color de fondo del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 */
static void write(unsigned char c, int FGColor, int BGColor) { drawChar(c, FGColor, BGColor);}

/**
 * @brief Función que espera al transcurso de los segundos deseados que se reciben como parámetro.
 * 
 * @param seconds Segundos que se desean esperar.
 */
void wait(int seconds){
    int initial = ticks_elapsed();
	while ( (double) (ticks_elapsed()-initial)/18.0 < 2 );
}

/**
 * @brief Llama a los distintos Handlers para cada interrupción de software.
 * 
 * @param syscall Número de la interrupción de software
 * @param param1 Primer parámetro recibido. Corresponde al registro RSI.
 * @param param2 Segundo parámetro recibido. Corresponde al registro RDX
 * @param param3 Tercer parámetro recibido. Corresponde al registro R10.
 * @param param4 Cuarto parámetro recibido. Corresponde al registro R8.
 * @param param5 Quinto parámetro recibido. Corresponde al registro R9.
 * @return Retorna en el caso de ser necesario, el valor de retorno de la interrupción llamada. En otro caso
 * retorna el valor 0.
 */
long int syscallsDispatcher (uint64_t syscall, uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5) {
    switch (syscall) {
		case 0:
			return read();
        case 1:
            if (param1==127)    
                deleteChar();
            else
                write(param1, param2, param3);
            break;
        case 2:
            drawNextLine();
            break;
        case 3:
        	_sti();
            wait(param1);
            break;
        case 4:
            timeToStr((char*)param1);
            break;     
        case 5: 
            if (getAltTouched()){
                return (intptr_t)registers_space;      
            }
            return 0;
        case 6:
            beep(param1,param2);
            break;
        case 7:
            drawCircle(param1, param2, param3, param4);
            break;
        case 8:
            drawRectangle(param1, param2, param3, param4, param5);
            break;
        case 9:
            loadScreen();         
            break;
        case 10:
            enableDoubleBuffer(param1);            
            break;
        case 11:
            getKeyboardState((char*)param1);
            break;
        case 12:
            drawCharAt(param1, param4, param5, param2, param3);            
            break;
	}
	return 0;
}
