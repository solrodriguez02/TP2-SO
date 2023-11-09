#ifndef _KEYBOARDDRIVER_H_
#define _KEYBOARDDRIVER_H_

#define TOTAL_SCANCODES 84
#define SHFT_DOWN 42
#define RSHFT_DOWN 54
#define SHFT_UP 170
#define RSHFT_UP 182
#define CAPSLOCK_DOWN 58
#define ESC_KEY 0X1
#define ALT_KEY 0x38
#define CTRL_KEY_DOWN 0x1D
#define CTRL_KEY_UP CTRL_KEY_DOWN + BREAK_CODE
#define BREAK_CODE 128 
#define MAX_SIZE_BUF 40


extern char readScanCode();
char keyboard_handler(char character);
unsigned char scanCodeToASCII(unsigned char scanCode);
void checkConditions(unsigned char scanCode);
int isLetter(unsigned char scanCode);
int getShift();
int getCapslock();
char consumeKeyFromBuffer();
void clearKeyboardBuffer();
#endif