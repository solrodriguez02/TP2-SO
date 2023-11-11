#include <stdint.h>

struct statProcess{
    uint8_t * name;
    int16_t pid;
    uint8_t state;
    uint8_t priority;
    uint8_t isForeground;
    void * stackPointer;
    void * basePointer;
};

typedef struct statProcess * stat;

void loadAllCommands();

void printHelp();

void clear();

void killProcess(char ** params);

void updateProcessPriority(char ** params);

void blockProcess(char ** params);

void ps();

void mem();

void initializePhiloWrapper();