#ifndef _SHELL_H_
#define _SHELL_H_

#define TOTAL_MODULES 38
#define COMMAND_MIN_SIZE 4
#define TIME_BUFF_SIZE 9
#define MAX_NUM_ARGUMENTS 12
#define EOFILE '\377'

void startShell();
void loadModule(char * name, char * description, void (*function)(char** params), int numParams);
void loadAllModules();
void runModule(const char * input[]);
void printHelp();
void clear();
int getIndexModule(char * name);

#endif