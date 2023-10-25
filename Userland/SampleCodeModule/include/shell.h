#ifndef _SHELL_H_
#define _SHELL_H_

#define TOTAL_MODULES 3
#define COMMAND_MIN_SIZE 4
#define TIME_BUFF_SIZE 9

void startShell();
void loadModule(char * name, char * description, void (*function)(void));
void loadAllModules();
void runModule(const char * input);
void printHelp();
void clear();

#endif