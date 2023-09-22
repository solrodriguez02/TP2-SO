#ifndef _KEYBOARDDRIVER_H_
#define _KEYBOARDDRIVER_H_

#define TOTAL_SCANCODES 84
#define SHFT_DOWN 42
#define RSHFT_DOWN 54
#define SHFT_UP 170
#define RSHFT_UP 182
#define CAPSLOCK_DOWN 58
#define PONG_KEYS 5
#define UPR_KEY 0x18//24 o
#define UPL_KEY 0x11//17 w
#define DOWNR_KEY 0x25//36 k
#define DOWNL_KEY 0X1F//31 s
#define ESC_KEY 0X1
#define ALT_KEY 0x38
#define CTRL_KEY 0x1D
#define BREAK_CODE 128 
#define BREAK_UPR_KEY UPR_KEY + BREAK_CODE  
#define BREAK_UPL_KEY UPL_KEY + BREAK_CODE 
#define BREAK_DOWNR_KEY DOWNR_KEY + BREAK_CODE  
#define BREAK_DOWNL_KEY DOWNL_KEY + BREAK_CODE  

extern char readScanCode();
char keyboard_handler();
unsigned char scanCodeToASCII(unsigned char scanCode);
void checkConditions(unsigned char scanCode);
int isLetter(unsigned char scanCode);
int getShift();
int getCapslock();
char getAltTouched();
void resetAltTouched();
void getKeyboardState(char * up) ;

#endif