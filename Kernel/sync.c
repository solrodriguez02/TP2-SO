#include <sync.h>
#include <scheduler.h>

void enterRegion(int * lock){
    /**
     * @brief while preventivo, asi por ejemplo si el user desbloquea el proceso, 
     * este va a solicitar entrar a la región critica nuevamente, y sino se va a bloquear
     * no es busy waiting
     * 
     */
    while( enter_region(lock) ){
        blockRunningProcess(BLOCKBYSYNC,MUTEX,lock);
    }
}

void leaveRegion(int * lock, int reason){
    leave_region(lock);
    tryToUnlockSem(lock,reason);    
}