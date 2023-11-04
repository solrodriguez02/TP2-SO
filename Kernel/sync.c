#include <sync.h>
#include <scheduler.h>

void enterRegion(int * lock){
    int isBusy = enter_region(lock);
    if (isBusy){
        blockProcess(0, BLOCKBYIPC);
    }
}

void leaveRegion(int * lock){
    leave_region(lock);
    tryToUnlockPipe(0);
    //no se vuelve a setear en 0 porque la idea es que al desbloquear el otro procesa corra desde donde quedo bloqueado
}