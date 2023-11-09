#include <shell.h>
#include <library.h>
#include <stdint.h>
#include "philosophers.h"

//! VAR SOLO USADA PARA TESTS
static int nextPid=2;
int maxCantArg;
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

int getIndexModule(char * name){
    for(int i=0; i<TOTAL_MODULES; i++) {
        if (strcmp(modules[i].name,name)){
            return i;
        }
    }
    return -1;
}

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
        maxCantArg = strtok(input,' ', command, MAX_NUM_ARGUMENTS);
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

int64_t my_create_process(char *name, uint64_t argc, char *argv[]){
    return execve( modules[getIndexModule(name)].function , 1, argc, argv);
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
    for (int i = 0; i < 500; i++){
        print("--------------------------------------xxxxxxx------------",0xFF0000);
    }
    printf("\nSobrevivi?");
    exit();
}

void enterOrange(){
    int i, aux = nextPid; 
    for ( i = 0; i < 750; i++){
        print("--------------------------------------xxxxxxx------------",ORANGE << aux);
    }
    printf("\nSobrevivi?");
    exit();
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


#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  uint64_t aux = *p;
  my_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n = 5;
  int8_t inc = 1;
  int8_t use_sem = 1;
  /*
  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;
*/
  printf("entre al proceso\n");
  if (use_sem)
    if (my_sem_open(SEM_ID, 1) == -1) {
      printf("test_sync: ERROR opening semaphore\n");
      exit();
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      my_sem_wait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      my_sem_post(SEM_ID);
  }

  if (use_sem)
    my_sem_close(SEM_ID);
  
  printf("salio todo ok\n");
  exit();
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

/*
  if (argc != 2)
    return -1;
*/
  char **argvDec = malloc(3 * sizeof(char *));
  //memcpy(argvDec[0], argv[0], strlen(argv[0]));
  argvDec[0] = argv[0];
  memcpy(argvDec[1], "-1", 2);
  memcpy(argvDec[2], argv[1], strlen(argv[1]));
  char **argvInc = malloc(3 * sizeof(char *));
  argvInc[0] = argv[0];
  memcpy(argvInc[1], "1", 1);
  memcpy(argvInc[2], argv[1], strlen(argv[1]));


  global = 0;
  printf("starting test_sync nombre:%s arg1:%s, arg2:%s\n", argv[0], argv[1], argv[2]);
  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = my_create_process("my_process_inc", 3, argvDec);
    pids[i + TOTAL_PAIR_PROCESSES] = my_create_process("my_process_inc", 3, argvInc);
  }

  /*
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    my_wait(pids[i]);
    my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
  }
  */
  waitChildren();

  printf("Final value: %d\n", global);

    exit();
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
    int prior = strToNum(params[1]);
    updatePriority(pid, prior);
    printf("Se actualizo la prioridad del proceso con pid %d a %d", pid, prior);
}


void execveNew( char ** params){
    int functionIndex = strToNum(params[0]);
    char isForeground = strToNum(params[1]);
    if ( functionIndex < 1 || functionIndex > TOTAL_MODULES ){
        printf("Invalid module");
        return;
    }
    
    int argc = modules[functionIndex].numParams + 1;    //+1 x el nom
    char ** argv = malloc( argc*8 );  // calculo peor escenario
    argv[0] = modules[functionIndex].name;
    for (int i = 1; i <= argc; i++){
        argv[i] = params[i+1];
    }
    //int pid = execve(modules[functionIndex-1].function, isForeground);
    // comentado pues x ahora usamos isForeground para identif halt
    int pid = execve(modules[functionIndex].function, isForeground, argc, argv );
    
    nextPid = pid; 
    nextPid++;
    
    if (isForeground)
        waitChild(pid);
    
    /* Despues hay q sacar esto, solo dejar el mensaje de error*/
    if (pid == -1){
        //printf("proceso creado con pid: %d", pid);
    //}else{
        printf("\ncreacion de proceso fallido");
    }
}

void getInputAndPrint(char ** params){
    printf("estoy en el proceso que lee del pipe e imprime");
    char read; 
    while ( (read = getChar()) != EOF){
        //bussy_wait(WAIT*100);
        
        print("/", BLUE);
        print(&read, 0X00FF00);
    }
    exit();
}

void enterBg(){
    char * params[2] = {"2", "0"};
    execveNew(params);
}

void bgOrange(){
    char * params[2] = {"27", "0"};
    execveNew(params);
}

void testSync(char ** params){
    char * *argvExec;
    argvExec = (char**) malloc(5 * sizeof(char *));
    for (int i = 0; i < 5; i++){
        argvExec[i] = (char*) malloc(40 * sizeof(char));
    }
    int index = getIndexModule("test_sync");
    numToStr(index, 10, argvExec[0]);
    memcpy(argvExec[1], "1", 1);
    memcpy(argvExec[2], params[0], strlen(params[0]));
    memcpy(argvExec[3], params[1], strlen(params[1]));
    memcpy(argvExec[4], params[2], strlen(params[2]));
    execveNew(argvExec);
}

void runWithPipe(char ** params){
    char ** params1 = malloc(3 * sizeof(char *));
    for (int i = 0; i < 2; i++){
        params1[i] = (char*) malloc(40 * sizeof(char));
    }
    memcpy(params1[0], params[0], strlen(params[0]));
    memcpy(params1[1], "0", 1);
    char ** params2= malloc(3 * sizeof(char *));
    for (int i = 0; i < 2; i++){
        params2[i] = (char*) malloc(40 * sizeof(char));
    }
    memcpy(params2[0], params[1], strlen(params[0]));
    memcpy(params2[1], "0", 1);
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
    if ( nextPid > 2)
    block(nextPid-1); 
}

void blockProcess(char ** params){
    int pid = strToNum(params[0]);
    block(pid);
}

void psWrapper(){
    char * params[2] = {"17", "1"};
    execveNew(params);
}

void ps() {
    // podria sino hacer un malloc
    int MAX_PROCESS = 8;
    struct statProcess arrayStats[MAX_PROCESS];
    int end = getAllProcessInfo(arrayStats);
    // ps stackpointer queda = a su basepointer
    for ( int i=0; i<end; i++ ){
        printf("\nProcess %s with pid %d:\n", arrayStats[i].name, arrayStats[i].pid);
        printf("\t Prioridad: %d", arrayStats[i].priority);
        printf("\t Estado: ");
        printStatus(arrayStats[i].state);
        printf("\t %ssta en foreground \n", (arrayStats[i].isForeground)? "E":"NO e" );
        printf("\t StackPointer: %x", arrayStats[i].stackPointer );
        printf("\t BasePointer: %x", arrayStats[i].basePointer );
    }

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

void cat(){
    char read;
    while ((read = getChar()) != EOFILE){
        printf("%c", read);
    }
    exit();
}

void wc(){
    int count = 1;
    char read;
    while ((read = getChar()) != EOFILE){
        printf("%c", read);
        if (read == '\n'){
            count++;
        }
    }
    printf("\nCantidad de lineas: %d", count);
    exit();
}

void filter(){
    char read;
    while ((read = getChar()) != EOFILE){
        if (read != 'a' && read != 'e' && read != 'i' && read != 'o' && read != 'u'){
            printf("%c", read);
        }
    }
    exit();
}

void initializePhiloWrapper(){
    initializePhilo();
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
    loadModule("print", "Prints the values in STDIN",&getInputAndPrint, 0);
    loadModule("getPid", "Returns current process PID", &getCurrentPid, 0);
    loadModule("getstatus", "Gets the current status from process", &getDefinedStatus, 1);
    loadModule("kill", "Kills a given process", &killProcess, 1);
    loadModule("k", "Kills the last-created process", &killLastCreated, 1);
    loadModule("fork", "Executes fork+execve for a given process", &execveNew, 2);
    loadModule("bgEnter", "Runs the program Enter in Background", &enterBg, 0);
    loadModule("block", "Blocks a specific process", &blockProcess, 1);
    loadModule("b", "Blocks the last-created process", &blockLastCreated, 0);
    loadModule("getPriority", "Get priority from process", &getProcessPriority, 1);
    loadModule("nice", "Update priority from process", &updateProcessPriority, 2);
    loadModule("yield", "Makes the current process stop it's quantum", &yield, 1);
    loadModule("sleep", "Sleep (param= #ticks)", &sleep, 1);
    loadModule("createPipe", "Creates a pipe between 2 arbitrary processes (for now)", &runWithPipe, 1);
    loadModule("ps", "List all processes in execution with their states", &ps, 0);
    loadModule("openSem", "Opens a sem for the current proccess", &openSem, 2);
    loadModule("getSemValue", "Gets the curret value in a sem", &getSemValue, 1);
    loadModule("waitSem", "Does the operation to the given sem", &waitSem, 1);
    loadModule("postSem", "Does the operation to the given sem", &postSem, 1);
    loadModule("closeSem", "Closes a sem for the current proccess", &closeSem, 1);
    loadModule("testSync", "Tests sems. order n, use_sem, inc", &testSync, 3);
    loadModule("test_sync", "Tests sems. order n, use_sem, inc", &test_sync, 3);
    loadModule("my_process_inc", "Increments a global variable", &my_process_inc, 3);
    loadModule("bgOrange", "Runs the program Enter in Background", &bgOrange, 0);
    loadModule("orange", "Tests a loop of printf's",&enterOrange, 0);
    loadModule("psExec", "Wrapper for ps", &psWrapper,0);
    loadModule("cat", "program 'cat'", &cat,0);
    loadModule("wc", "program 'wc'", &wc,0);
    loadModule("filter", "program 'filter'", &filter,0);
    loadModule("philos", "Run philosophers", &initializePhiloWrapper,1);
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
            int numParams1 = modules[i].numParams;
            if (maxCantArg >= numParams1 + 1){
                int numParams2;
                //debería pasarle el array de parametros y directamente que cada uno lo maneje a su gusto
                if (strcmp(input[numParams1 + 1],"|")){
                            char ** params1;
                            params1 = malloc((4 + numParams1) * sizeof(char *));
                            params1[0] = (char *) modules[i].function;
                            params1[1] = (char *) 1;//foreground por ahora
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
                            params2[0] = (char *) modules[i].function;//numParams1 + 4
                            params2[1] = (char *) 1;//foreground por ahora
                            params2[2] = numParams2 + 1;
                            params2[3] = modules[i].name;
                            for (int j = 0; j < numParams2; j++){
                                params2[j+4] = input[j+numParams1+2];
                            }
                            syscall_createPipe(params1, params2);
     
                            if (!strcmp(input[numParams1 + 3],"&")){
                                waitChildren();
                            }
                            return;
                        }
                    }
                    printf("Invalid command: for a list of available functionalities type 'help'");
                    return;
                }
                if (strcmp(input[numParams1+1], "&")){
                    char ** params1;
                    params1 = malloc((4 + numParams1) * sizeof(char *));
                    params1[0] = (char *) modules[i].function;
                    params1[1] = (char *) 0; //background
                    for (int j = 0; j < numParams1; j++){
                        params1[j+4] = input[j+1];
                    }
                    execveNew(params1);
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
