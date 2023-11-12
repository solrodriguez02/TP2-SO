// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <programs.h>
#include <library.h>
#include <shell.h>

void loadAllPrograms(){
    loadModule("cat", "program 'cat'", &catWrapper,0);
    loadModule("cat", "program 'cat'", &cat,0);
    loadModule("wc", "program 'wc'", &wcWrapper,0);
    loadModule("wc", "program 'wc'", &wc,0);
    loadModule("filter", "program 'filter'", &filterWrapper,0);
    loadModule("filter", "program 'filter'", &filter,0);
    loadModule("loop", "program 'loop'", &loopWrapper,0);
    loadModule("loop", "Runs loop with print", &loop, 0);
}

void execveNew( char ** params){
    int functionIndex = strToNum(params[0]);
    char isForeground = strToNum(params[1]);
    if ( functionIndex < 1 || functionIndex > TOTAL_MODULES ){
        printf("Invalid module");
        return;
    }
    
    int argc = getFunctionNumParams(functionIndex) + 1;    //+1 x el nom
    char ** argv = malloc( argc*20 );  // calculo peor escenario
    argv[0] = getFunctionName(functionIndex);
    for (int i = 1; i <= argc; i++){
        argv[i] = params[i+1];
    }
    
    int pid = execve(getFunctionPointer(functionIndex), isForeground, argc, argv );
    
    if (isForeground)
        waitChild(pid);
    
    if (pid == -1){
        printf("\ncreacion de proceso fallido");
    }
}

void loopWrapper(char ** param){
    char * params[2];
    for (int i = 0; i < 2; i++){
        params[i] = malloc(2);
    }
    numToStr(getIndexModule("loop_original"), 10, params[0]);
    memcpy(params[1], (param != 1 && strcmp(param[0], "&")) ? "0": "1", 1);
    execveNew(params);
}

void loop(){
    while(1){
        bussy_wait(WAITTIME* 5);
        printf("Soy el proceso con PID: %d\n", getPid());
    }
    exit();
}

void catWrapper(char ** param){
    char * params[2];
    for (int i = 0; i < 2; i++){
        params[i] = malloc(2);
    }
    numToStr(getIndexModule("cat_original"), 10, params[0]);
    memcpy(params[1], "1", 1);
    execveNew(params);
}

void cat(){
    char read;
    while ((read = getChar()) != EOFILE){
        printf("%c", read);
    }
    exit();
}

void wcWrapper(char ** param){
    char * params[2];
    for (int i = 0; i < 2; i++){
        params[i] = malloc(2);
    }
    numToStr(getIndexModule("wc_original"), 10, params[0]);
    memcpy(params[1], "1", 1);
    execveNew(params);
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

void filterWrapper(char ** param){
    char * params[2];
    for (int i = 0; i < 2; i++){
        params[i] = malloc(2);
    }
    numToStr(getIndexModule("filter_original"), 10, params[0]);
    memcpy(params[1], "1", 1);
    execveNew(params);
}

void filter(){
    char read;
    while ((read = getChar()) != EOFILE){
        if (read != 'a' && read != 'e' && read != 'i' && read != 'o' && read != 'u' && read != 'A' && read != 'E' && read != 'I' && read != 'O' && read != 'U'){
            printf("%c", read);
        }
    }
    exit();
}
