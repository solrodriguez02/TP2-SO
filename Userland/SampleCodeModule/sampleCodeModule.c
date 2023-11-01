/* sampleCodeModule.c */

#include <shell.h>

#include <library.h>

int main(int argc, char ** argv) {
	printf("Hola %d, %s \n", argc, argv[0]);
	startShell();
	return 0;
}