#ifndef _SHELL_H_
#define _SHELL_H_

#define TOTAL_MODULES 28
#define COMMAND_MIN_SIZE 4
#define TIME_BUFF_SIZE 9
#define MAX_NUM_ARGUMENTS 12

#define RUNNING 0
#define READY 1 
#define BLOCKED 2
#define TERMINATED 3

/**
 * @brief Estructura que contiene el nombre, la descripción y la dirección de la función correspondinte al módulo.
 */
typedef struct {
    char * name;
    char * description;
    void (*function)(char ** params);
    int numParams;
} module;


void startShell();
void loadModule(char * name, char * description, void (*function)(char** params), int numParams);
void * getFunctionPointer(int index);
char * getFunctionName(int index);
char * getFunctionDescription(int index);
int getFunctionNumParams(int index);
void loadAllModules();
void runModule(char * input[]);
void printHelp();
void clear();
int getIndexModule(char * name);
void clearInput();
#endif