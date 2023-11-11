#include <blockingSys.h>
#include <lib.h>
#include <videodriver.h>
#include <scheduler.h>
#include <pipes.h>

extern char buffer[MAX_SIZE_BUF];
/**
 * @brief Lee del buffer correspondiente al fd 
 * 
 */
int read(int fd, char * placeholder, int count){
    
    void * buf = getFdBuffer(0, STDIN);

    if ( buf == &buffer){
        blockRunningProcess(BLOCKBYREAD,count, fd);
        while ( count--){
            *placeholder++ = consumeKeyFromBuffer(fd);
        }
    }
    else {
       int dim = count; 
       pipeADT pipe = (pipeADT) buf;
       readPipe(pipe,placeholder,dim);
    }
    return 0;
}

/**
 * @brief Escribe sobre la pantalla un caracter con su color deseado.
 * 
 * @param c Caracter al cual se quiere escribir sobre la pantalla.
 * @param FGColor Color del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 * @param BGColor Color de fondo del caracter deseado en el formato 0xRRGGBB, siendo RR el byte para el código de 
 * color rojo, GG el código de color verde, y BB el código de color azul.
 */
static void write(unsigned char c, int FGColor, int BGColor) { 
    if (getFdBuffer(0, STDOUT) == BASEDIRVIDEO){
        drawChar(c, FGColor, BGColor);
        return;
    }
    int result = writePipe(getFdBuffer(0,STDOUT), &c, 1);
    if (result == -1){
        deleteFromScheduler(0);
    }
}


void waitChildren(){
    blockProcess(0,BLOCKBYWAITCHILDREN);   
}

void waitChild(int pid){
    blockRunningProcess(BLOCKBYWAITCHILD, pid, 0x0);   
}