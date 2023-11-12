#include <stdint.h>
#include <interrupts.h>
#include <keyboardDriver.h>

int read(char * placeholder, int count);

void write(unsigned char c, int FGColor, int BGColor);

void waitChildren();

void waitChild(int pid);
