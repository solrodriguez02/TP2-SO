#include <stdint.h>
#include <interrupts.h>
#include <videodriver.h>
#include <speakerDriver.h>
#include <keyboardDriver.h>
#include <time.h>
#include <clock.h>
#include <MemoryManager.h>
#include <scheduler.h>
#include <semaphore.h>
#include <manageProcess.h>
#include <blockingSys.h>
#include <scheduler.h>

extern char buffer;
extern pcbEntryADT PCB[MAX_SIZE_PCB]; 
extern uint16_t lastSelected;
sem_ptr sem;


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


/**
 * @brief Llama a los distintos Handlers para cada interrupción de software.
 * 
 * @param syscall Número de la interrupción de software
 * @param param1 Primer parámetro recibido. Corresponde al registro RSI.
 * @param param2 Segundo parámetro recibido. Corresponde al registro RDX
 * @param param3 Tercer parámetro recibido. Corresponde al registro R10.
 * @param param4 Cuarto parámetro recibido. Corresponde al registro R8.
 * @param param5 Quinto parámetro recibido. Corresponde al registro R9.
 * @return Retorna en el caso de ser necesario, el valor de retorno de la interrupción llamada. En otro caso
 * retorna el valor 0.
 */
long int syscallsDispatcher (uint64_t syscall, uint64_t param1, uint64_t param2, uint64_t param3, uint64_t param4, uint64_t param5) {

    switch (syscall) {
		case 0:
        return read(param1,(char *) param2,param3);
			return readVIEJO();
        case 1:
            if (param1==127)    
                deleteChar();
            else
                write(param1, param2, param3);
            break;
        case 2:
            drawNextLine();
            break;
        case 4:
            timeToStr((char*)param1);
            break;     
        case 6:
            beep(param1,param2);
            break;
        case 8:
            return getAllProcessInfo(param1);
        case 9:
            loadScreen();         
            break;
        case 10:
            enableDoubleBuffer(param1);            
            break;
        case 12:
            drawCharAt(param1, param4, param5, param2, param3);            
            break;
        case 13:
            return allocMemory(param1);
        case 14:
            freeMemory((void *) param1);
            break;
        case 15:
            return getPid();
        case 16:
            return getStatus(param1);
        case 17:
            return deleteFromScheduler(param1);
        case 18:
            return execve((void *)param1, param2, param3, param4);
        case 19:
            if (getStatus(param1) == BLOCKED){
                unblockProcess(param1);
            }else{
                blockProcess(param1, BLOCKBYUSER);
            }
            break;
        case 20:
            updatePriority(param1, param2);
            break;
        case 21:
            return getPriority(param1);
        case 22:
            /* le da prioridad 0 en prox ejecucion */
            completeQuantum();
            forceTimerInt();
            break;
        case 23:
            waitChildren();
            break;
        case 24:
            waitChild(param1);
            break;
        case 25:
            return openSem((char *)param1, param2);
        case 26:
            sem = getSemByName((char *)param1);
            if ( sem == NULL)
                return -1;
            return getSemValue(sem);
        case 27:
            sem = getSemByName((char *)param1);
            if ( sem == NULL)
                return -1;
            waitSem(sem);
            return 0;
        case 28:
            sem = getSemByName((char *)param1);
            if ( sem == NULL)
                return -1;
            postSem(sem);
            return 0;
        case 29:
            closeSemSyscall((char *)param1);
            return 0;
        case 30:
            createNewPipe((char **) param1, (char **) param2);
            return 0;
        case 31:
            getState((char *) param1);
            return 0;

    }
	return 0;
}
