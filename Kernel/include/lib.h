#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
void numToStr(int64_t num, int base, char * buffer);
int strCmp(const char *s1, const char *s2);
int strlen(const char * str);
char *cpuVendor(char *result);

#endif