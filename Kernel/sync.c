#include <sync.h>
#include <scheduler.h>

void enterRegion(int * lock){
    /**
     * @brief while preventivo, asi por ejemplo si el user desbloquea el proceso, 
     * este va a solicitar entrar a la región critica nuevamente, y sino se va a bloquear
     * no es busy waiting
     * 
     */
    // 
    while( enter_region(lock) ){
        blockRunningProcess(BLOCKBYIPC,MUTEX,lock);
    }
    /* version anterior */
    /* 
        int isBusy = enter_region(lock);
        if (isBusy){
            blockProcess(0, BLOCKBYIPC);
            enter_region(lock);
        }
    */
}

void leaveRegion(int * lock, int reason){
    leave_region(lock);
    //tryToUnlockPipe(0);
    tryToUnlockSem(lock,reason);
    //no se vuelve a setear en 0 porque la idea es que al desbloquear el otro procesa corra desde donde quedo bloqueado
}