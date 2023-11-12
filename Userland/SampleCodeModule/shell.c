// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#include <library.h>
#include <stdint.h>
#include <philosophers.h>
#include <mandatoryTests.h>
#include <shellCommands.h>
#include <programs.h>

module modules[TOTAL_MODULES];
int modulesCount = 0;
int maxCantArg;
char input[COMMAND_MAX_SIZE];
char * command[MAX_NUM_ARGUMENTS];

void startShell() {
    loadAllModules();
    printf("Welcome to the shell\n");
    //modules[0].function(0);
    while(1){
        printf("\n");
        print("$ ", BLUE);
        getInput(input);
        maxCantArg = strtok(input,' ', command, MAX_NUM_ARGUMENTS);
        runModule(command);
        clearInput();
    }
}

void loadModule(char * name, char * description, void (*function)(char** params), int numparams) {
    modules[modulesCount].name = name;
    modules[modulesCount].description = description;
    modules[modulesCount].function = function;
    modules[modulesCount].numParams = numparams;
    modulesCount++;
}

void clearInput(){
    for (int i = 0; i < COMMAND_MAX_SIZE; i++){
        input[i] = '\0';    
    }
} 

void loadAllModules() {
    loadAllCommands();
    loadAllPrograms();
    loadAllTests();
}

/**
 * @brief Función que llama al módulo correspondiente dependiendo del parámetro ingresado. En caso de
 * no existir, imprime un mensaje de error. Se manejan los caracteres especiales como & y |.
 * 
 * @param input Nombre del módulo y parametros recibidos desde la consola.
 */
void runModule(char * input[]){
    printf("\n");    
    for(int i=0;i<TOTAL_MODULES;i++){
        if (strcmp(modules[i].name,input[0])){
            if (!input[1]){
                modules[i].function((char **) 1);
                return;
            }
            int numParams1 = modules[i].numParams;
            if (maxCantArg >= numParams1 + 1){
                if (i < 8){
                    print("Invalid command: commands cannot be piped or run in bg", RED);
                    printf("\n");
                    return;
                }
                int numParams2;
                if (strcmp(input[numParams1 + 1],"|")){
                            char ** params1;
                            params1 = malloc((4 + numParams1) * sizeof(char *));
                            params1[0] = (char *) modules[i+1].function;
                            params1[1] = (char *) 1;
                            params1[2] = numParams1 + 1;
                            params1[3] = modules[i].name;
                            for (int j = 0; j < numParams1; j++){
                                params1[j+4] = input[j+1];
                            }
                    for(int i=0;i<TOTAL_MODULES;i++){
                        if (strcmp(modules[i].name,input[numParams1 + 2])){
                            char ** params2;
                            numParams2 = modules[i].numParams;
                            params2 = malloc((4 + numParams2) * sizeof(char *));
                            params2[0] = (char *) modules[i+1].function;
                            params2[1] = (char *) 1;
                            params2[2] = numParams2 + 1;
                            params2[3] = modules[i].name;
                            for (int j = 0; j < numParams2; j++){
                                params2[j+4] = input[j+numParams1+2];
                            }
                            syscall_createPipe(params1, params2);
     
                            if (!strcmp(input[numParams1 + 3], "&")){
                                waitChildren();
                            }
                            return;
                        }
                    }
                    printf("Invalid command: for a list of available functionalities type 'help'");
                    return;
                }
            }
            modules[i].function(&input[1]);
            return;
        }
    }
    printf("Invalid functionality: for a list of available functionalities type 'help'");
    return;
}

int getIndexModule(char * name){
    for(int i=0; i<TOTAL_MODULES; i++) {
        if (strcmp(modules[i].name,name)){
            return i;
        }
    }
    return -1;
}

void * getFunctionPointer(int index){
    return modules[index].function;
}

char * getFunctionName(int index){
    return modules[index].name;
}

char * getFunctionDescription(int index){
    return modules[index].description;
}

int getFunctionNumParams(int index){
    return modules[index].numParams;
}