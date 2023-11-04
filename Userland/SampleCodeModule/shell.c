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
    void (*function)(char ** params);
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
void loadModule(char * name, char * description, void (*function)(char** params), int numparams) {
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
    for (int i = 0; i < 15; i++){
        printf("--------------------------------------xxxxxxx------------");
    }
    printf("\nSobrevivi?");
    exit();
}

static getIndexModule(char * name){
    for(int i=0; i<TOTAL_MODULES; i++) {
        if (strcmp(modules[i].name,name)){
            return i;
        }
    }
    return -1;
}

static void printStatus(int status){
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


void getDefinedStatus(char ** params){
        int pid = strToNum(params[0]);
        int status =  getStatus(pid);
        printStatus(status);
}

void getCurrentPid(){
    printf("El pid: %d\n", getPid());
}

void killLastCreated(){
    kill(nextPid-1);
}


void killProcess(char ** params){
    int pid = strToNum(params[0]);
    if ( pid==0)
        printf("Invalid pid\n");
    else
        kill(pid);
}

void getProcessPriority(char ** params){
    int pid = strToNum(params[0]);
    int priority = getPriority(pid);
    printf("El proceso %d tiene prioridad %d\n", pid, priority);
}

void updateProcessPriority(char ** params){
    int pid = strToNum(params[0]);
    updatePriority(pid);
}


void execveNew( char ** params){
    int functionIndex = strToNum(params[0]);
    char isForeground = strToNum(params[1]);

    if ( functionIndex < 1 || functionIndex > TOTAL_MODULES ){
        printf("Invalid module");
        return;
    }
    //int pid = execve(modules[functionIndex-1].function, isForeground);
    // comentado pues x ahora usamos isForeground para identif halt
    char * argv[1];
    argv[0] = modules[functionIndex-1].name;
    int pid = execve(modules[functionIndex-1].function, isForeground, 1, argv );
    
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

void getInputAndPrint(char ** params){
    printf("estoy en el proceso que lee del pipe e imprime");
    while (1){
        char read = getChar();
        print("proceso get&print:", BLUE);
        print(&read, 0XFF0000);
    }
}

void enterBg(){
    char * params[2] = {"3", "0"};
    execveNew(params);
}

void runWithPipe(char ** params){
    char** params1 = { params[0], "0" };
    char** params2 = { params[1], "0" };
    syscall_createPipe();
    execveNew(params1);
    execveNew(params2);
}

void openSem(char ** params){
    char * name = params[0]; 
    int value = strToNum(params[1]);
    int result = syscall_openSem(name, value);
    if (result == 0){
        printf("Se creo el semaforo: %s con valor inicial %d", name, value);
    }
    else{
        printf("No se pudo crear el semaforo: %s", name);
    }
}

void getSemValue(char ** params){
    char * name = params[0];
    int semValue = syscall_getSemValue(name);
    printf("El valor del semaforo: %s es: %d", name, semValue);
}

void waitSem(char ** params){
    char * name = params[0];
    syscall_waitSem(name);
    printf("Se hizo wait al semaforo %s: \n", name);
    getSemValue(params);
}

void postSem(char ** params){
    char * name = params[0];
    syscall_postSem(name);
    printf("Se hizo post al semaforo %s: \n", name);
    getSemValue(params);
}

void closeSem(char ** params){
    char * name = params[0];
    int result = syscall_closeSem(name);
    if (result == 0){
        printf("Se elimino el semaforo: %s", name);
    }
    else{
        printf("No se pudo eliminar el semaforo: %s", name);
    }
}

void blockLastCreated (){
    block(nextPid-1); 
}

void blockProcess(char ** params){
    int pid = strToNum(params[0]);
    block(pid);
}

void ps() {
    // podria sino hacer un malloc
    int MAX_PROCESS = 5;
    stat arrayStats[MAX_PROCESS];
    int end = getAllProcessInfo(arrayStats);
    for ( int i=0; i<end; i++ ){
        printf("\nProcess %s with pid %d:\n", arrayStats[i]->name, arrayStats[i]->pid);
        printf("\t Prioridad: %d", arrayStats[i]->priority);
        printf("\t Estado: ");
        printStatus(arrayStats[i]->state);
        printf("\t %ssta en foreground \n", (arrayStats[i]->isForeground)? "E":"NO e" );
        printf("\t StackPointer: %x", arrayStats[i]->stackPointer );
        printf("\t BasePointer: %x", arrayStats[i]->basePointer );
    }

    //exit();
}

void yieldFun(){
    print("AAAA", BLUE);
    yield();   
    print("BBBB", 0xFF0000);
}

void loop(){
    while(1){
        print("Hola soy Oscar", ORANGE );
        //    syscall_wait(2);
        //!  esta haciendo busy waiting!!! 
        // => conviene usar int RTC
        //  PERO q el lo desbloquee es costoso

    }
    printf("Me despido\n");
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
    loadModule("enter", "Tests a loop of printf's",&enter, 0);
    loadModule("getInputAndPrint", "Prints the values in STDIN",&getInputAndPrint, 0);
    loadModule("getPid", "Returns current process PID", &getCurrentPid, 0);
    loadModule("getstatus", "Gets the current status from process", &getDefinedStatus, 1);
    loadModule("kill", "Kills a given process", &killProcess, 1);
    loadModule("k", "Kills the last-created process", &killLastCreated, 1);
    loadModule("fork", "Executes fork+execve for a given process", &execveNew, 2);
    loadModule("bgEnter", "Runs the program Enter in Background", &enterBg, 0);
    loadModule("block", "Blocks a specific process", &blockProcess, 1);
    loadModule("b", "Blocks the last-created process", &blockLastCreated, 0);
    loadModule("getPriority", "Get priority from process", &getProcessPriority, 1);
    loadModule("updatePriority", "Update priority from process", &updateProcessPriority, 2);
    loadModule("yield", "Makes the current process stop it's quantum", &yield, 1);
    loadModule("sleep", "Sleep (param= #ticks)", &sleep, 1);
    loadModule("createPipe", "Creates a pipe between 2 arbitrary processes (for now)", &runWithPipe, 1);
    loadModule("ps", "List all processes in executions with their states", &ps, 0);
    loadModule("openSem", "Opens a sem for the current proccess", &openSem, 2);
    loadModule("getSemValue", "Gets the curret value in a sem", &getSemValue, 1);
    loadModule("waitSem", "Does the operation to the given sem", &waitSem, 1);
    loadModule("postSem", "Does the operation to the given sem", &postSem, 1);
    loadModule("closeSem", "Closes a sem for the current proccess", &closeSem, 1);
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
            //debería pasarle el array de parametros y directamente que cada uno lo maneje a su gusto
            modules[i].function(&input[1]);
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
void printHelp() {
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
void clear() {
    enableDoubleBuffer(1);
    enableDoubleBuffer(0);
    exit();
}
