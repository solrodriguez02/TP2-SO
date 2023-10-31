#include <shell.h>
#include <library.h>
#include <stdint.h>

//! VAR SOLO USADA PARA TESTS
static int nextPid=2;

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
void loadModule(char * name, char * description, void (*function)(void)) {
    modules[modulesCount].name = name;
    modules[modulesCount].description = description;
    modules[modulesCount].function = function;
    modulesCount++;
}


void enter(){
    for (int i = 0; i < 28; i++){
        printf("--------------------------------------xxxxxxx------------");
    }
    printf("\nSobrevivi?");
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

void killLastCreated(){
    kill(nextPid);
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
    if ( functionIndex < 1 || functionIndex > 3 ){
        printf("Invalid module");
        return;
    }
    //int pid = execve(modules[functionIndex-1].function, isForeground);
    // comentado pues x ahora usamos isForeground para identif halt
    int pid = execve(modules[functionIndex-1].function, 1);
    
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


void blockLastCreated (){
    block(nextPid); 
}

void blockProcess(int pid){
    block(pid);
}

/**
 * @brief Carga todas los módulos/funcionalidades de la Shell disponibles para el usuario.
 */
void loadAllModules() {
    loadModule("help", "Prints name and description for all the functionalities available for the user", &printHelp);
    loadModule("clear", "Clears the screen of the shell", &clear);
    loadModule("enter", "Prueba enters",&enter);
    loadModule("getPid", "Returns current process PID", &getCurrentPid);
    loadModule("getstatus", "get status from process", &getDefinedStatus);
    loadModule("kill", "kill a process", &killProcess);
    loadModule("k", "to kill last created process", &killLastCreated);
    loadModule("fork", "executes fork+execve for a given process", &execveNew);
    loadModule("bgEnter", "crea proceso en bg", &enterBg);
    loadModule("block", "block specific process", &blockLastCreated);
    loadModule("b", "to block last created process", &blockProcess);
    loadModule("getPriority", "get priority from process", &getProcessPriority);
    loadModule("updatePriority", "update priority from process", &updateProcessPriority);
    loadModule("yield", "Abandonar cpu", &yield);
    loadModule("sleep", "Sleep (param= #ticks)", &sleep);
}


/**
 * @brief Función que llama al módulo correspondiente dependiendo del parámetro ingresado. En caso de
 * no existir, imprime un mensaje de error.
 * 
 * @param input Nombre del módulo que se busca comparar.
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
