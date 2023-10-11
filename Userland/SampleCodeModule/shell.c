#include <shell.h>
#include <library.h>
#include <stdint.h>

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

/**
 * @brief Carga todas los módulos/funcionalidades de la Shell disponibles para el usuario.
 */
void loadAllModules() {
    loadModule("help", "Prints name and description for all the functionalities available for the user", &printHelp);
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
