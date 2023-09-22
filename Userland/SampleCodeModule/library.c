#include <stdarg.h>
#include <library.h> 
#include <time.h>

#define FBCOLOR_BLACK 0x000000
#define FGCOLOR_WHITE 0xFFFFFF

// ------------------------------------- LECTURA ------------------------------------- //

/**
 * @brief Retorna el valor ASCII de lectura del teclado.
 * 
 * @return Valor ASCII de lecuta del teclado.
 */
int getChar(){
    return syscall_read();
}

/**
 * @brief Escribe sobre la cadena de caracteres recibida como parámetro las teclas ingresadas por el usuario
 * hasta un máximo de COMMAND_MAX_SIZE caracteres o hasta recibir el caracter de salto de línea.
 * Además, coloca en la pantalla las teclas ingresadas.
 * 
 * @param buff Cadena de caracteres a modificar por el usuario.
 */
void getInput(char * buff){
    char c;
    int buffidx=0, screenptr=0;
    while((c=getChar())!='\n') {
        if (c == 127 /* DEL ASCII */ ){
            if ( buffidx!=0 ){
                putChar(c, FGCOLOR_WHITE);
                if ( buffidx >= screenptr-- )
                    buffidx--;
            }
        } else {
            putChar(c, FGCOLOR_WHITE);            
            if (buffidx < COMMAND_MAX_SIZE) {
                buff[buffidx++] = c;
            }
            screenptr++;
        }
    }  
    buff[buffidx] = '\0';
}

/**
 * @brief Retorna el vector de registros.
 * 
 * @return Vector que contiene los valores de los registros.
 */
long int * getRegisters() {
    return syscall_getRegisters();
}

// ------------------------------------- ESCRITURA ------------------------------------- //

/**
 * @brief Escribe sobre la pantalla el caracter recibido como parámetro en la posición horizontal y 
 * vertical de la pantalla recibida como parámetro.
 * 
 * @param c Caracterer que se desea escribir sobre la pantalla.
 * @param x Posición horizontal donde se desea colocar el caracter.
 * @param y Posición vertical donde se desea colocar el caracter.
 * @return Valor entero 0.
 */
int putCharAt(char c, int x, int y){
    syscall_writeAt(c, x, y, FGCOLOR_WHITE, FBCOLOR_BLACK);
    return 0;
}

/**
 * @brief Escribe sobre la pantalla el caracter recibido como parámetro con el color deseado.
 * 
 * @param c Caracter al cual se desea escribir.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @return Valor entero 0. 
 */
int putChar(char c, int FGColor){
    syscall_write(c, FGColor, FBCOLOR_BLACK);
    return 0;
}

/**
 * @brief Escribe en pantalla una cadena de caracteres recibida como parámetro, en la posición deseada.
 * 
 * @param string Cadena de caracteres recibida como parámetro
 * @param x Posición horizontal donde se desea colocar el caracter.
 * @param y Posición vertical donde se desea colocar el caracter.
 * @return Valor entero 0.
 */
int printAt(char * string, int x, int y) {
    int i = 0;
    while (string[i] != '\0'){
        putCharAt(string[i], x, y);
        x+=15;
        i++;
    }
    return 0;
}

/**
 * @brief Imprime en pantalla una cadena de caracteres sin formato con un color recibido como parámetro.
 * 
 * @param string Cadena de caracteres que se desea imprimir.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @return Valor entero 0.
 */
int print(char * string, int FGColor){
    int i = 0;
    while (string[i] != '\0'){
        putChar(string[i++], FGColor);
    }
    return 0;
}

/**
 * @brief Escribe sobre la pantalla una cadena de caracteres formada con cierto formato, reemplazando
 * ciertos caracteres designados por el valor de las argumentos recibidos de izquierda a derecha en la
 * función.
 * Los caracteres designados son:
 * %c -> char
 * %d -> signed decimal
 * %s -> string
 * %x -> unsigned hexadecimal integer
 * %% -> escapa el caracter '%'
 * \\n -> nuevo salto de línea
 * \\a -> beep
 * 
 * @param format Cadena de caracteres constante, con el formato dado.
 * @param ... Argumentos variables dependiendo de la cantidad de caracteres designados en el format.
 * @return Valor entero 0.
 */
int printf(const char * format, ...){
    /*
    %c -> char
    %d -> signed decimal
    %s -> string
    %x -> unsigned hexadecimal integer
    */
    va_list args;
    va_start(args, format);
    for (int i = 0; format[i]; i++){
        //va_start recibe una instancia de va_list () y el argumento que viene antes de los argumentos variables en la funcion, que en este caso es format
        
        char buffer[12];
        if (format[i] != '%' && format[i] != '\n' && format[i] != '\a') {
            putChar(format[i], FGCOLOR_WHITE);
        } else {
            if (format[i] == '%'){
                i++;
                if(format[i]=='c') { 
                    int ch = va_arg(args,int); 
                    putChar((char)ch, FGCOLOR_WHITE);            
                } else if (format[i] == 'd') {
                    int dec = va_arg(args, int);
                    numToStr(dec, 10, buffer);
                    print(buffer, FGCOLOR_WHITE);
                } else if (format[i] == 's') {
                    char * str = va_arg(args, char *);
                    print(str, FGCOLOR_WHITE);
                }  else if (format[i] == 'f') {
                    /* unused */
                }  else if (format[i] == 'x') {
                    int hex = va_arg(args,int);
                    numToStr(hex,16,buffer);
                    print(buffer, FGCOLOR_WHITE);
                } else if (format[i] == '%'){ 
                    putChar('%', FGCOLOR_WHITE);
                } /* else invalid literal after %*/

            } else { //format[i] == '//' -> barra de escape
                if (format[i]=='\n'){
                    syscall_nextLine();
                } else if (format[i] == '\a') {
                    syscall_beep(1000,10);
                } /* other options ?*/
            }
        }
    }
    va_end(args);
    return 0;
}

// ------------------------------------- SCREEN CONTROL ------------------------------------- //


/**
 * @brief Función que espera la cantidad de segundos deseada recibida como parámetro.
 * 
 * @param seconds Cantidad de segundos a esperar.
 */
void sleep(int seconds){
    syscall_wait(seconds);
}

/**
 * @brief Reproduce un sonido de frecuencia 1000Hz durante un corto instante de tiempo.
 */
void beep() {
    syscall_beep(1000,10);
}

/**
 * @brief Carga el segundo buffer guardado.
 */
void loadScreen() {
    syscall_loadScreen();
}

/**
 * @brief Activa y desactiva el segundo buffer.
 * 
 * @param enable Valor booleano de si activa el segundo buffer.
 */
void enableDoubleBuffer(int enable){
    syscall_enableDoubleBuffer(enable);
}

// ------------------------------------- CONVERSIÓN ------------------------------------- //

/**
 * @brief Convierte sobre la cadena de caracteres recibida como parámetro un número a su representación
 * ASCII en la base deseada.
 * 
 * @param num Número que se desea convertir a su representación ASCII.
 * @param base Base en la cual se desea convertir el número. Bases disponibles: decimal o hexadecimal.
 * @param buffer Cadena de caracteres en la cual se escribe la representación ASCII del número.
 */
void numToStr(long int num, int base, char * buffer) {
    /* Calculo cuantos dígitos tiene */
    /* BASE != 0 (hay que verificar?)*/
    int i=0;
    int isPositive = num >= 0;
    if (!isPositive) {
        buffer++; /* reservo un lugar */
        num*=-1;
    } else if (num==0){ /* caso especial de si el número es 0*/
        buffer[0]='0';
        buffer[1]='\0';
        return;
    }
        
    uint64_t aux = num;
    while(aux!=0) {
        i++;
        aux/=base;
    }
    
    buffer[i--] = '\0';

    int mod=0;
    while(i>=0){
        mod = num%base;
        if (mod<10) {
            buffer[i] = mod + '0';
        } else {
            /* hex */
            buffer[i] = mod - 10 + 'A';
        }
        num/=base;
        i--;
    }
    if (!isPositive) {
        buffer--;
        buffer[0]='-';
    }
        
}

/**
 * @brief Retorna el valor numérico de una cadena de caracteres que son sólo dígitos.
 * 
 * @param stringNum Cadena de caracteres de sólo ´dígitos.
 * @return Valor numérico de la cadena de caracteres.
 */
int strToNum(char * stringNum){
    int n = 0;
    int i = 0;
    int notNeg = 1;
    if (stringNum[i]=='-') {
        notNeg=-1;
        i++;
    }
    while (stringNum[i]){
        n = n*10 + (stringNum[i] - '0');
        i++;
    }
    n*=notNeg;
    return n;
}

/**
 * @brief Actualiza sobre la cadena de caracteres recibida el tiempo actual del sistema.
 * 
 * @param timeStr Cadena de caracteres que es modificada.
 */
void getTime(char * timeStr) {
    syscall_time(timeStr);
}

/**
 * @brief Actualiza los valores del vector de teclas utilizada para el minijuego.
 * 
 * @param keys Vector de caracteres para las teclas utilizadas para el minijuego.
 */
void getKeyboardState(char * keys) {
    return syscall_getKeyboardState(keys);
}

// ------------------------------------- BOOLEANOS ------------------------------------- //

/**
 * @brief Compara la igualdad entre dos cadenas de caracteres. Retorna 1 si ambos son iguales, y 0 en otro caso.
 * 
 * @param s1 Primera cadena de caracteres a comparar.
 * @param s2 Segunda cadena de caracteres a comparar.
 * @return Valor booleano de si ambas cadenas son iguales en representación ASCII.
 */
int strcmp(const char* s1, const char* s2){
    int i=0;
    while(s1[i]!='\0' && s2[i]!='\0') {
        if (s1[i]!=s2[i]) {
            return 0;
        }
        i++;
    }
    if(s1[i]!='\0' || s2[i]!='\0') {
        return 0;
    }
    return 1;
}

// ------------------------------------- FIGURES ------------------------------------- //

/**
 * @brief Dibuja sobre la pantalla un círculo centrado en un par ordenado (x,y) con cierto radio y color.
 * 
 * @param x_centre Posición horizontal en la cual el círculo está centrado.
 * @param y_centre Posición vertical en la cual el círculo está centrado.
 * @param r Radio del círculo.
 * @param color Color del círculo.
 */
void drawCircle(int x_centre, int y_centre, int r, uint8_t color) {
    syscall_drawCircle(x_centre, y_centre, r, color);
}

/**
 * @brief Dibuja sobre la pantalla un rectángulo que comienza en la esquina inferior izquierda, de cierta 
 * altura, ancho y color.
 * 
 * @param x Posición izquierda donde comienza el rectángulo.
 * @param y Posición inferior donde comienza el rectángulo.
 * @param h Altura del rectángulo.
 * @param w Ancho del rectángulo.
 * @param color Color del rectángulo.
 */
void drawRectangle(int x, int y, int h, int w, uint8_t color) {
    syscall_drawRectangle(x, y, h, w, color);
}