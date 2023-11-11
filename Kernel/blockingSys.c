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

void waitChildren(){
    blockProcess(0,BLOCKBYWAITCHILDREN);   
}

void waitChild(int pid){
    blockRunningProcess(BLOCKBYWAITCHILD, pid, 0x0);   
}