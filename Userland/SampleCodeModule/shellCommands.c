// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shellCommands.h>
#include <library.h>
#include <shell.h>

void loadAllCommands(){
    loadModule("help", "Prints name and description for all the functionalities available for the user", &printHelp, 0);
    loadModule("clear", "Clears the screen of the shell", &clear, 0);
    loadModule("kill", "Kills a given process", &killProcess, 1);
    loadModule("block", "Blocks a specific process", &blockProcess, 1);
    loadModule("nice", "Update priority from process", &updateProcessPriority, 2);
    loadModule("mem", "Prints the memory status", &mem, 0);
    loadModule("ps", "List all processes in execution with their states", &ps, 0);
    loadModule("philos", "Run philosophers", &initializePhiloWrapper,0);
}

void printHelp() {
    printf("The shell's functionalities are the following:\n");
    for(int i=0; i<18; i++) {
        printf("\n");
        print(" - ", GREEN);
        print(getFunctionName(i), GREEN);
        printf(" : ");
        printf(getFunctionDescription(i));
    }
    printf("\n");
    printf("\nEnter the chosen functionality's name through the command line and press enter to run it");
}

void clear() {
    enableDoubleBuffer(1);
    enableDoubleBuffer(0);
}

void killProcess(char ** params){
    int pid = strToNum(params[0]);
    if ( pid==0)
        printf("Invalid pid\n");
    else
        kill(pid);
}

void updateProcessPriority(char ** params){
    int pid = strToNum(params[0]);
    int prior = strToNum(params[1]);
    updatePriority(pid, prior);
    printf("Se actualizo la prioridad del proceso con pid %d a %d", pid, prior);
}

void blockProcess(char ** params){
    int pid = strToNum(params[0]);
    block(pid);
}

int getAllProcessInfo(stat arrayStats){
    return syscall_getAllProcessInfo( arrayStats);
}

static void printStatus(int status){
    switch(status){
            case RUNNING:
                printf("running");
                break;
            case READY:
                printf("ready");
                break;
            case BLOCKED:
                printf("blocked");
                break;
            case TERMINATED:
                printf("terminated");
                break;
        }
}

void ps() {
    int MAX_PROCESS = 14;
    struct statProcess arrayStats[MAX_PROCESS];
    int end = getAllProcessInfo(arrayStats);
    for ( int i=0; i<end; i++ ){
        print("PID ",0xEE00FF);
        printf("%d: NAME: %s", arrayStats[i].pid, arrayStats[i].name);
        printf("\t PRIORITY: %d", arrayStats[i].priority);
        printf("\t FG: %s \n", (arrayStats[i].isForeground)? "YES":"NO" );
        printf("\t STATE: ");
        printStatus(arrayStats[i].state);
        printf("\t SP: %x", arrayStats[i].stackPointer );
        printf("\t BP: %x\n", arrayStats[i].basePointer );
    }

}

#define FREE 1
#define USED 2
#define BOUNDRY 3

void mem(){
    char * state = malloc(257);
    int blockIndex = 0, size, usedMem=0, allocations=0;
    size = getMemStatus(state);
    printf("MEMORY STATE:\n");
    while(state[blockIndex] != '\0'){
        printf("|%s", (state[blockIndex] != FREE)? "1":"0");
        
        if ( state[blockIndex]==USED )
            usedMem++;
        else if ( state[blockIndex]==BOUNDRY )
            allocations++; 
        blockIndex++;
        //printf("Bloque %d: %s", blockIndex+1, (state[blockIndex] != -1)? "Ocupado\n":"Libre\n");
        
    }
    long unsigned totalMem = getTotalMemory();

    printf("\nSIZE PER BLOCK: %d\n", size );
    printf("TOTAL MEM: %d bytes\n", totalMem);
    printf("OCCUPIED MEM: %d bytes\n", size*usedMem );
    printf("ALLOCATED MEM: %d bytes\n", size*allocations);
    /*
    
    printf("TOTAL OCCUPIED BLOCKS: %d", blockIndex);
    
    printf("TOTAL ALLOCATIONS: %d", blockIndex);
*/
}

void initializePhiloWrapper(){
    initializePhilo();
}