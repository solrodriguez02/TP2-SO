#include <shell.h>
#include <library.h>
#include <stdint.h>
#include "syscall.h"
#include "test_util.h"
#include <stdio.h>
#include <string.h>

//se deben incluir los siguientes archivos de testeo junto con sus headers:
// syscall.c
// test_util.c

#define MAX_BLOCKS 100

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;
  uint8_t test;
  test = 1;
  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  while (1) {
    rq = 0;
    total = 0;
    sleep(1);
    printf("\n --------------------------TEST %d-----------------------\n", test++);
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = syscall_malloc(mm_rqs[rq].size);
      
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        sleep(2);
      printf("################# malloc size %d ####################### total storage used: %d / %d\n", mm_rqs[rq].size, total, max_memory);
        rq++;
      }
    }
    sleep(2);
    printf("se pudieron crear %d bloques dado un maximo de %d bloques\n", rq, MAX_BLOCKS);
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size);

    sleep(3);
    printf("se setean los bloques con el size que ocupa c/u\n");
    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          return -1;
        }

    sleep(2);
    printf("ahora se los libera\n");
    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        free(mm_rqs[i].address);
    
    sleep(1);
    printf("\n --------------TEST %d EXITOSO-------------------\n", test++);
  }
}

/**
 * @brief Estructura que contiene el nombre, la descripción y la dirección de la función correspondinte al módulo.
 */
typedef struct {
    char * name;
    char * description;
    void (*function)(void);
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
    modules[0].function();
    char input[COMMAND_MAX_SIZE];
    while(1){
        printf("\n");
        print("$ ", BLUE);
        getInput(input);
        runModule(input);
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

void testMemoryManager(){
    char * max_mem[] = {"10000"};
    int result = test_mm(1, max_mem);
    printf("##################################################################");
    printf("el test termino con valor de retorno %d", result);
}

/**
 * @brief Carga todas los módulos/funcionalidades de la Shell disponibles para el usuario.
 */
void loadAllModules() {
    loadModule("help", "Prints name and description for all the functionalities available for the user", &printHelp);
    loadModule("testingMM", "test memory manager", &testMemoryManager);
    loadModule("clear", "Clears the screen of the shell", &clear);
}

/**
 * @brief Función que llama al módulo correspondiente dependiendo del parámetro ingresado. En caso de
 * no existir, imprime un mensaje de error.
 * 
 * @param input Nombre del módulo que se busca comparar.
 */
void runModule(const char * input){
    printf("\n");    
    for(int i=0;i<TOTAL_MODULES;i++){
        if (strcmp(modules[i].name,input)){
            modules[i].function();
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
}
