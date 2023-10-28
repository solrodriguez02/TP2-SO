#ifndef _SHELL_H_
#define _SHELL_H_

#define TOTAL_MODULES 10
#define COMMAND_MIN_SIZE 4
#define TIME_BUFF_SIZE 9
#define MAX_NUM_ARGUMENTS 4

void startShell();
void loadModule(char * name, char * description, void (*function)(void));
void loadAllModules();
void runModule(const char * input[]);
void printHelp(int none);
void clear(int none);

#endif