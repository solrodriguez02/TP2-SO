#include <stdint.h>
#include <interrupts.h>
#include <scheduler.h>
#include <keyboardDriver.h>

int readNuevo(int fd, char * placeholder, int count);

// NO LA DEF TDV xq genera conflicto con la vieja
//int write( int fd, void * placeholder, size_t count);