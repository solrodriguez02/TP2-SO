#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include <sysWrappers.h>
#include <stdint.h>

#define WHITE 0xFF
#define BACKGROUNG_COLOR 0x00
#define GREEN 0x00FFAA
#define ORANGE 0xFFB366
#define BLUE 0x4D4DFF
#define MID_SCREEN_X 512
#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1018
#define COMMAND_MAX_SIZE 80
#define EOFILE '\377'


#define MINOR_WAIT 1000000 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 10000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

void getInput(char * buffer);

int printAt(char * string, int x, int y);
int print(char * string, int FGColor);
int printf(const char * format, ...);


void numToStr(long int num, int base, char * buffer);
int strToNum(char* num);
void * memcpy(void * destination, const void * source, uint64_t length);
int strcmp (const char* s1, const char* s2);
int strtok(char * s, char delim, char * array[], int arraySize);
uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
void endless_loop();
void endless_loop_print(uint64_t wait);

#endif