#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include <syscalls.h>

#define WHITE 0xFF
#define BACKGROUNG_COLOR 0x00
#define GREEN 0x00FFAA
#define ORANGE 0xFFB366
#define BLUE 0x4D4DFF
#define MID_SCREEN_X 512
#define SCREEN_HEIGHT 768
#define SCREEN_WIDTH 1018
#define COMMAND_MAX_SIZE 15

int getChar();
void getInput(char * buffer);
long int * getRegisters();

int putCharAt(char c, int x, int y);
int putChar(char c, int FGColor);
int printAt(char * string, int x, int y);
int print(char * string, int FGColor);
int printf(const char * format, ...);

void sleep(int seconds);
void beep();
void loadScreen();
void enableDoubleBuffer(int enable);

void numToStr(long int num, int base, char * buffer);
int strToNum(char* num);
void getTime(char * timeStr);
void getKeyboardState(char * keys);

int strcmp (const char* s1, const char* s2);

void drawCircle(int x, int y, int r, uint8_t color);
void drawRectangle(int x, int y, int h, int w, uint8_t color);

#endif