#include "sysWrappers.h"
#define FBCOLOR_BLACK 0x000000
#define FGCOLOR_WHITE 0xFFFFFF

// ------------------------------------- LECTURA ------------------------------------- //


static char buf;
/**
 * @brief Retorna el valor ASCII de lectura del teclado.
 * 
 * @return Valor ASCII de lecuta del teclado.
 */
int getChar(){
    syscall_read(STDIN, &buf, 1);
    return buf;
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
 * @brief Actualiza sobre la cadena de caracteres recibida el tiempo actual del sistema.
 * 
 * @param timeStr Cadena de caracteres que es modificada.
 */
void getTime(char * timeStr) {
    syscall_time(timeStr);
}

// -------------------------------------- MEMORY ------------------------------------- //

void * malloc(unsigned int size){
    void * ans; 
    // si me pase de lo asignado x el proceso
    ans = syscall_malloc(size);
    return ans;
}

void free(void * ptr){
    // conviene validar aca, evitar ir a kernel
    if ( ptr==NULL)
        return;
    syscall_free(ptr);
}

// -------------------------------------- PROCESSES ------------------------------------- //

int getPid(){
    return syscall_getPid();
} 

int getStatus(int pid){
    return syscall_getStatus(pid);
}

int kill(int pid){
    return syscall_kill(pid);
}

int execve(void * pointer, char isForeground, int argc, char ** argv){
    return syscall_execve(pointer, isForeground, argc, argv);
}

void block(int pid){
    syscall_block(pid);
}

void exit(){
    kill(0);
}

void yield(){
    syscall_yield();   
}
void updatePriority(int pid){
    syscall_update_priority(pid);
}

int getPriority(int pid){
    return syscall_get_priority(pid);
}

void waitChildren(){
    syscall_waitChildren();
}

void waitChild(int pid){
    syscall_waitChildren( pid );
}

int getAllProcessInfo(stat arrayStats){
    return syscall_getAllProcessInfo( arrayStats);
}
