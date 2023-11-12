#include <stdarg.h>
#include <library.h> 
#include <time.h>
#include <shell.h>
#include "syscall.h"

#define FBCOLOR_BLACK 0x000000
#define FGCOLOR_WHITE 0xFFFFFF

// ------------------------------------- LECTURA ------------------------------------- //

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
    while((c=getChar())!='\n' && c != EOFILE) {
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
    if (c == EOFILE){
        exit();
    }
    buff[buffidx] = '\0';
}

// ------------------------------------- ESCRITURA ------------------------------------- //

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
                    putChar('\n', FGCOLOR_WHITE);
                } else if (format[i] == '\a') {
                    syscall_beep(1000,10);
                } /* other options ?*/
            }
        }
    }
    va_end(args);
    return 0;
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
    while (stringNum[i] && stringNum[i] != '\n'){
        n = n*10 + (stringNum[i] - '0');
        i++;
    }
    n*=notNeg;
    return n;
}

int strlen(const char * str){
    int i=0;
    while(str[i]!='\0') {
        i++;
    }
    return i;
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

// ------------------------------------- MODIFICACIONES STRINGS ------------------------------------- //

//se devuelve la cant maxima de strings que se parsearon
int strtok(char * s, char delim, char * array[], int arraySize){
    int arrayIndex = 0;
    //primero se guarda el puntero en el array desde el principio
    //entonces por cada vez que encuentre el delimitador tengo que dejar el puntero al char de inicio
    if(*s != delim && *s != '\0')
        array[arrayIndex++] = s;
    while( *s != '\0' ){
        if(*s == delim){
            *s = 0;
            //si el siguiente no es tambien un delimitador ni el final del string
            //entonces mientras todavia tenga espacio en el array -> me guardo el puntero al siguiente
            if( *(s+1) != delim &&  (*(s+1) != '\0')){
                if(arrayIndex >= arraySize)
                    return arrayIndex;
                array[arrayIndex++] = s + 1;
            }
        }
        s++;
    }
    return arrayIndex;
}

/**
 * @brief Copia la cantidad de caracteres deseados desde un puntero fuente sobre un puntero destino.
 * 
 * @param destination 
 * @param source 
 * @param length 
 * @return void* 
 */
void * memcpy(void * destination, const void * source, uint64_t length) {
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

// ------------------------------------- PARA TESTEOS DE LA CATEDRA ------------------------------------- //

// Random
static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint() {
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max) {
  uint32_t u = GetUint();
  return (u + 1.0) * 2.328306435454494e-10 * max;
}

// Memory
uint8_t memcheck(void *start, uint8_t value, uint32_t size) {
  uint8_t *p = (uint8_t *)start;
  uint32_t i;

  for (i = 0; i < size; i++, p++)
    if (*p != value)
      return 0;

  return 1;
}

// Parameters
int64_t satoi(char *str) {
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str)
    return 0;

  if (str[i] == '-') {
    i++;
    sign = -1;
  }

  for (; str[i] != '\0'; ++i) {
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}


int64_t my_sem_open(char *sem_id, uint64_t initialValue){
    return syscall_openSem(sem_id, initialValue);

}
int64_t my_sem_wait(char *sem_id){
    return syscall_waitSem(sem_id);

}
int64_t my_sem_post(char *sem_id){
    return syscall_postSem(sem_id);
}
int64_t my_sem_close(char *sem_id){
    return syscall_closeSem(sem_id);
}
int64_t my_yield(){
    syscall_yield();
    return 0;
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
  syscall_update_priority(pid, newPrio);
  return 0;
}

int64_t my_create_process(char *name, uint64_t argc, char *argv[]){
    return execve(getFunctionPointer(getIndexModule(name)), 0, argc, argv);
}

int64_t my_wait(int64_t pid){
    syscall_waitChild(pid);
    return 0;
}

int64_t my_getpid(){
    return syscall_getPid();
}

int64_t my_kill(uint64_t pid) {
    return syscall_kill(pid);
}

int64_t my_block(uint64_t pid){
    syscall_block(pid);
    return 0;
}

int64_t my_unblock(uint64_t pid){
    syscall_block(pid);
    return 0;
}
