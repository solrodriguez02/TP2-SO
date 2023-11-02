#include <shell.h>
#include <library.h>
#include <stdint.h>


#define RUNNING 2
#define READY 1 
#define BLOCKED 0
#define TERMINATED 3

/**
 * @brief Estructura que contiene el nombre, la descripción y la dirección de la función correspondinte al módulo.
 */
typedef struct {
    char * name;
    char * description;
    void (*function)(int param1);
    int numParams;
} module;

/**
 * @brief Vector de módulos
 */
module modules[TOTAL_MODULES];
int modulesCount = 0;

/**
 * @brief Función que inicializa la Shell, y consulta constantemente acerca de qué módulo se desea correr.
 */
void startShell() {
    loadAllModules();
    printf("Welcome to the shell\n");
    //modules[0].function(0);
    char input[COMMAND_MAX_SIZE];
    char * command[MAX_NUM_ARGUMENTS];
    while(1){
        printf("\n");
        print("$ ", BLUE);
        getInput(input);
        strtok(input,' ', command, MAX_NUM_ARGUMENTS);
        runModule(command);
    }
}

/**
 * @brief Para cada módulo, almacena su nombre, una breve descripción de lo que hace, y un puntero a su función
 * correspondiente.
 * 
 * @param name Nombre del módulo a almacenar.
 * @param description Breve descripción del módulo.
 * @param function Puntero a la función correspondiente.
 */
void loadModule(char * name, char * description, void (*function)(void), int numparams) {
    modules[modulesCount].name = name;
    modules[modulesCount].description = description;
    modules[modulesCount].function = function;
    modules[modulesCount].numParams = numparams;
    modulesCount++;
}


void enter(){
    while(1){
        printf("--------------------------------------xxxxxxx------------");
    }
    exit();
}

void getDefinedStatus(int pid){
        int status =  getStatus(pid);
        switch(status){
            case RUNNING:
                printf("proceso running\n");
                break;
            case READY:
                printf("proceso ready\n");
                break;
            case BLOCKED:
                printf("proceso bloqueado\n");
                break;
            case TERMINATED:
                printf("proceso terminado\n");
                break;
        }
}

void getCurrentPid(int none){
    printf("El pid: %d\n", getPid());
}

void killProcess(int pid){
    if ( pid==0)
        printf("Invalid pid\n");
    else
        kill(pid);
}

void getProcessPriority(int pid){
    int priority = getPriority(pid);
    printf("El proceso %d tiene prioridad %d\n", pid, priority);
}

void updateProcessPriority(int pid){
    updatePriority(pid);
}

void enterBg(){
    execveNew(3, 0);
}

void execveNew(int functionIndex, char isForeground ){
    if ( functionIndex < 1 || functionIndex > 3 ){
        printf("Invalid module");
        return;
    }
    if (isForeground)
        printf("Is in fg!");
    int pid = execve(modules[functionIndex-1].function, isForeground);
    waitChildren();
    if (pid != -1){
        printf("\nproceso enter creado con pid: %d", pid);
    }else{
        printf("\ncreacion de proceso fallido");
    }
}

void getInputAndPrint(){
    printf("estoy en el proceso que lee del pipe e imprime");
    while (1){
        char read;
        syscall_read(0,&read,1);
        printf("proceso get&print: %c", read);
    }
}

void runWithPipe(){
    execveNew(3, 0);
    block(2);
    execveNew(4, 0);
    block(3);
    syscall_createPipe();
    block(2);
    block(3);
}

void blockProcess(int pid){
    block(pid);
}

/**
 * @brief Carga todas los módulos/funcionalidades de la Shell disponibles para el usuario.
 * faltan: 
 * ps - 0 param - syscall needed -lista de procesos con sus estados (nombre, ID, prioridad, stack y base pointer, foreground)
 * loop -  Imprime su ID con un saludo cada una determinada cantidad de segundos.
 * cat: Imprime el stdin tal como lo recibe.
 * wc: Cuenta la cantidad de líneas del input.
 * filter: Filtra las vocales del input.
 * phylo: Implementa el problema de los filósofos comensales. DEBERÁ permitir cambiar la cantidad de filósofos en runtime con las teclas “a” (add 1) y “r” (remove 1).
 */
void loadAllModules() {
    loadModule("help", "Prints name and description for all the functionalities available for the user", &printHelp, 0);
    loadModule("clear", "Clears the screen of the shell", &clear, 0);
    loadModule("enter", "Prueba enters",&enter, 0);
    loadModule("getInputAndPrint", "imprime lo que recibe por fd = 0",&getInputAndPrint, 0);
    loadModule("getPid", "Returns current process PID", &getCurrentPid, 0);
    loadModule("getstatus", "get status from process", &getDefinedStatus, 1);
    loadModule("kill", "kill a process", &killProcess, 1);
    loadModule("fork", "executes fork+execve for a given process", &execveNew, 2);
    loadModule("bgEnter", "crea proceso en bg", &enterBg, 0);
    loadModule("block", "block specific process", &blockProcess, 1);
    loadModule("getPriority", "get priority from process", &getProcessPriority, 1);
    loadModule("updatePriority", "update priority from process", &updateProcessPriority, 2);
    loadModule("yield", "Abandonar cpu", &yield, 1);
    loadModule("sleep", "Sleep (param= #ticks)", &sleep, 1);
    loadModule("createPipe", "Crea un pipe", &runWithPipe, 1);
}


/**
 * @brief Función que llama al módulo correspondiente dependiendo del parámetro ingresado. En caso de
 * no existir, imprime un mensaje de error.
 * 
 * @param input Nombre del módulo que se busca comparar.
 * necesitaria conocer el numero de parametros que espero de la funcion que matcheo para despues poder chequear si se paso un | o un &
 */
void runModule(const char * input[]){
    printf("\n");    
    for(int i=0;i<TOTAL_MODULES;i++){
        if (strcmp(modules[i].name,input[0])){
            if (!input[1]){
                modules[i].function(1);
                return;
            }
            modules[i].function(strToNum(input[1]));
            return;
        }
    }
    printf("Invalid functionality: for a list of available functionalities type 'help'");
    return;
}

// -------------------------------------- FUNCIONES DE LOS MÓDULOS -------------------------------------- //

/**
 * @brief Función correspondiente para el módulo de "help". Imprime todas las funcionalidades disponibles.
 */
void printHelp(int none) {
    printf("The shell's functionalities are the following:\n");
    for(int i=0; i<TOTAL_MODULES; i++) {
        printf("\n");
        print(" - ", GREEN);
        print(modules[i].name, GREEN);
        printf(" : ");
        printf(modules[i].description);
    }
    printf("\n");
    printf("\nEnter the chosen functionality's name through the command line and press enter to run it");
}

/**
 * @brief Función correspondiente al módulo de "clear". Limpia la pantalla de la shell.
 */
void clear(int none) {
    enableDoubleBuffer(1);
    enableDoubleBuffer(0);
    exit();
}
