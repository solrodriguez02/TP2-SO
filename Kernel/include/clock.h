#ifndef CLOCK_H
#define CLOCK_H

#define MAX_DATE_LENGTH 29

extern unsigned char getTime( unsigned char mode);
extern void drawDate(char * date);

void rtc_handler();
unsigned int decode(unsigned char time);
unsigned int seconds();
unsigned int minutes();
unsigned int hours();
unsigned int day();
unsigned int weekday();
unsigned int month();
unsigned int year();
void timeToStr(char * dest);
void dateToStr(char * dest);
void fulldate(char * dateStr);

#endif