#include <blockingSys.h>
#include <lib.h>
#include <videodriver.h>
#include <pipes.h>

extern char buffer;
/**
 * @brief Lee del buffer correspondiente al fd 
 * 
 */
int read(int fd, char * placeholder, int count){
    
    void * buf = getFdBuffer(0, fd);

    // ya fuerza interrup 
    //blockProcess(RUNNING_PROCESS, BLOCKBYREAD);
    blockRunningProcess(BLOCKBYREAD,count, fd);

    // ojo con los EOF!
    if ( buf == &buffer)
        while ( count--){
            *placeholder++ = consumeKeyFromBuffer(fd);
        }
    else {
        // tal vez se pueda traer consumeKeyFromBuff 
        // y usarlo pa todos los buff
        // pasas el void * buf y el count
        /* // => si count < dim, tiene q bloq otra vez
        int dim = (count > SIZE_PIPE_BUF)? SIZE_PIPE_BUF : count;
        count -= dim; 
        */
       int dim = count; 
       pipeADT pipe = (pipeADT) buf;
       readPipe(pipe,placeholder,dim);
    }
    
    return 0;
}

int write( int fd, void * placeholder, int count){
    void * buf = getFdBuffer(0, fd);
    blockProcess(0, BLOCKBYWRITE);      
    return 0;
}

void waitChildren(){
    blockProcess(0,BLOCKBYWAITCHILDREN);   
}