#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include "sysWrappers.h"


#define WHITE 0xFF
#define BACKGROUNG_COLOR 0x00
#define GREEN 0x00FFAA
#define ORANGE 0xFFB366
#define BLUE 0x4D4DFF
#define MID_SCREEN_X 512
#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1018
#define COMMAND_MAX_SIZE 15

void getInput(char * buffer);

int printAt(char * string, int x, int y);
int print(char * string, int FGColor);
int printf(const char * format, ...);


void numToStr(long int num, int base, char * buffer);
int strToNum(char* num);

int strcmp (const char* s1, const char* s2);
int strtok(char * s, char delim, char * array[], int arraySize);

#endif