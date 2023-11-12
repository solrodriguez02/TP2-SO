#include <stdint.h>

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