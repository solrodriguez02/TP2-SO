#include <blockingSys.h>

extern char buffer;

/**
 * @brief Lee del buffer correspondiente al fd 
 * 
 */
int readNuevo(int fd, char * placeholder, int count){
    void * buf = getFd(fd);
    
    // ya fuerza interrup 
    //blockProcess(RUNNING_PROCESS, BLOCKBYREAD);
    blockRunningProcess(BLOCKBYREAD,count,STDIN);
    
    while ( count--){
        *placeholder++ = consumeKeyFromBuffer();
    }
    
    // copio resultado a placeholder
    return 0;
}

int write( int fd, void * placeholder, int count){
    void * buf = getFd(fd);
    blockProcess(RUNNING_PROCESS, BLOCKBYWRITE);      
    return 0;
}