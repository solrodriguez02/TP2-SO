// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <speakerDriver.h>
#include <interrupts.h>

/**
 * @brief Reproduce un sonido con una frecuencia dada durante cierto tiempo.
 * 
 * @param freq Frecuencia deseada del sonido.
 * @param time Duración del sonido.
 */
void beep(uint32_t freq, uint32_t time) {
    uint32_t oldfreq = getOldFreq();
    playSound(freq);
    for (int i=0;i<time;i++) {
        _hlt();
    }
    noSound();
    restoreOldFreq(oldfreq);
}