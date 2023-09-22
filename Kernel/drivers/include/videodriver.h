#ifndef _VIDEODRIVER_H_
#define _VIDEODRIVER_H_

#include <stdint.h>

#define RED 0xFF0000
#define BLUE 0x0000FF
#define GREEN 0x00FF00
#define WHITE 0xFFFFFF
#define BACKGROUNG_COLOR 0x080808

// ----------------------------------- FONT ----------------------------------- //
unsigned char* getBitmapArrayEntry(unsigned char c);
uint32_t getFontSize();

// ----------------------------------- SCREEN CONTROL ----------------------------------- //
void setScreenBuffer(int directly);
void loadScreen(); 
void enableDoubleBuffer(int enable);
void scrollUp();
unsigned int getLine();
void drawNextLine();
void moveWritePointer();

// ----------------------------------- DRAWABLE ----------------------------------- //
void putPixel(uint8_t r, uint8_t g, uint8_t b, uint32_t x, uint32_t y);
void drawCharAt(unsigned char c, int FGColor, int BGColor, uint64_t horizontalOffset, uint64_t verticalOffset);
void drawChar(unsigned char c, int FGColor, int BGColor);
void drawBlank();
void deleteChar();
void drawString(char * str, int FGColor, int BGColor);
void drawTopString(char * str, uint64_t topOffset, int FGColor, int BGColor);
void drawDate(char * date);
void drawTopLine();

// ----------------------------------- FIGURES ----------------------------------- //
void drawLine(char r, char g, char b, int y);
void drawRectangle ( int x_left, int y_top,int h,int w, uint8_t color);
void drawCircle (int x_centre, int y_centre, int r, uint8_t color);

#endif