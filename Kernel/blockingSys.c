#include <blockingSys.h>

extern char buffer;

/**
 * @brief Lee del buffer correspondiente al fd 
 * 
 */
int readNuevo(int fd, void * placeholder, int count){
    void * buf = getFd(fd);
    
    // ya fuerza interrup 
    //blockProcess(RUNNING_PROCESS, BLOCKBYREAD);
    blockRunningProcess(BLOCKBYREAD,count,0x0);
    char aux = buffer;
    //*placeholder = buffer;
    buffer = 0;
    // copio resultado a placeholder
    return aux;
}

int write( int fd, void * placeholder, int count){
    void * buf = getFd(fd);
    blockProcess(RUNNING_PROCESS, BLOCKBYWRITE);      
    return 0;
}