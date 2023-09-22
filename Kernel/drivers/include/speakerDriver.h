#ifndef _SPEAKERDRIVER_H_
#define _SPEAKERDRIVER_H_
#include <stdint.h>

extern void playSound(uint32_t freq); 
extern void noSound();
extern uint32_t getOldFreq();
extern void restoreOldFreq(uint32_t old);
void beep(uint32_t freq, uint32_t time);


#endif