#define MAX_NAME 20
#define MAX_SHMS 20
#include <stdint.h>
#include <shm.h>

struct shm  {
    char name [MAX_NAME];
    void * ptr;
} ;
typedef struct shm * shm;
shm shms[MAX_SHMS];
int lastShmCreated=-1;

shm getShmByName(char * name){
    int i;
    for (i = 0; i < MAX_SHMS; i++){
        if ( shms[i]!=0x0 && strCmp(name, shms[i]->name )){
            return shms[i];
        }
    }
    return 0x0;
}


void * openShm(void * name, int size ) {
    int i;
    shm newShm = getShmByName(name);
    if ( newShm==0x0){
        for ( i=lastShmCreated+1; i!=lastShmCreated; i++ ){
            if ( i == MAX_SHMS ){
                i=0;
                if ( i==lastShmCreated)
                    break;
            }
            if ( shms[i]==0x0 )
                break;
        }
        /* si no hay mas espacio disponible */
        if ( i==lastShmCreated && shms[i]!=0x0)
            return 0x0; 
        lastShmCreated = i;
        newShm = allocMemory(sizeof(struct shm));
        newShm->ptr = allocMemory(size);
        memcpy(newShm->name, name, strlen(name));
        shms[i] = newShm;
    }
    
    return newShm->ptr;
}

