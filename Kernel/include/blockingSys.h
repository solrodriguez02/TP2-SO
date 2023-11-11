#include <stdint.h>
#include <interrupts.h>
#include <keyboardDriver.h>

int read(int fd, char * placeholder, int count);

void waitChildren();

void waitChild(int pid);
