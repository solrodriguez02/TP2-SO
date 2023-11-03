#include <shell.h>
#include <library.h>
#include <stdint.h>

//! VAR SOLO USADA PARA TESTS
static int nextPid=2;

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
    //while(1){
//void enter(int argc, char ** argv){
    //printf("hola soy %d, %s \n", argc, argv[0]);
    //for (int i = 0; i < 2; i++){
        //printf("--------------------------------------xxxxxxx------------");
    //}
    /*
    printf("bloqueo proceso\n");
    blockProcess(0);
    */
    for (int i = 0; i < 20; i++){
        printf("--------------------------------------xxxxxxx------------");
    }
    printf("\nSobrevivi?");
    exit();
}

static void printStatus(status){
    switch(status){
            case RUNNING:
                printf("running\n");
                break;
            case READY:
                printf("ready\n");
                break;
            case BLOCKED:
                printf("bloqueado\n");
                break;
            case TERMINATED:
                printf("terminado\n");
                break;
        }
}


void getDefinedStatus(int pid){
        int status =  getStatus(pid);
        printStatus(status);
}

void getCurrentPid(int none){
    printf("El pid: %d\n", getPid());
}

void killLastCreated(){
    kill(nextPid-1);
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
    execveNew(3,0);
}

void execveNew(int functionIndex, char isForeground ){
    if ( functionIndex < 1 || functionIndex > TOTAL_MODULES ){
        printf("Invalid module");
        return;
    }
    //int pid = execve(modules[functionIndex-1].function, isForeground);
    // comentado pues x ahora usamos isForeground para identif halt
    char * argv[1];
    argv [0] = "mi pid es...";
    int pid = execve(modules[functionIndex-1].function, 1, 1, argv );
    
    nextPid = pid; 
    nextPid++;
    
    if (isForeground)
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


void blockLastCreated (){
    block(nextPid-1); 
}

void blockProcess(int pid){
    block(pid);
}

void ps() {
    // podria sino hacer un malloc
    int MAX_PROCESS = 5;
    stat arrayStats[MAX_PROCESS];
    getAllProcessInfo(arrayStats);
    for ( int i=0; arrayStats[i]!= 0; i++ ){
        printf("\nProcess %s with pid %d:\n", arrayStats[i]->name, arrayStats[i]->pid);
        printf("\t Prioridad: %d", arrayStats[i]->priority);
        printf("\t Estado: ");
        printStatus(arrayStats[i]->pid);
        printf("\t %ssta en foreground \n", (arrayStats[i]->isForeground)? "E":"NO e" );
        printf("\t StackPointer: %d", arrayStats[i]->stackPointer );
        printf("\t BasePointer: %d", arrayStats[i]->basePointer );
    }

    //exit();
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
    loadModule("k", "to kill last created process", &killLastCreated, 1);
    loadModule("fork", "executes fork+execve for a given process", &execveNew, 2);
    loadModule("bgEnter", "crea proceso en bg", &enterBg, 0);
    loadModule("block", "block specific process", &blockProcess, 1);
    loadModule("b", "to block last created process", &blockLastCreated, 0);
    loadModule("getPriority", "get priority from process", &getProcessPriority, 1);
    loadModule("updatePriority", "update priority from process", &updateProcessPriority, 2);
    loadModule("yield", "Abandonar cpu", &yield, 1);
    loadModule("sleep", "Sleep (param= #ticks)", &sleep, 1);
    loadModule("createPipe", "Crea un pipe", &runWithPipe, 1);
    loadModule("ps", "Ver el estado de los procesos en ejecucion", &ps, 0);
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
}
