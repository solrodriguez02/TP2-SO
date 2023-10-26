#ifndef listADT_h
#define listADT_h
#include <string.h>
#include <stdlib.h>
#include "scheduler.h"

// Lista lineal ordenada y sin repetidos

typedef struct listCDT * listADT;

typedef pcbEntryADT elemType;

// Esto iria si no usamos un puntero a funcion
//static int compare (elemType elem1, elemType elem2) {
//    return elem1 - elem2;
//}

/**
 * función compare:
 * @param elem1
 * @param elem2
 * @return <0 si elem1 es "menor" a elem2
 *         >0 si elem1 es "mayor" que elem2
 *         0  si son "iguales"
 */
listADT newList(int (* compare)(elemType elem1, elemType elem2));

/**
 *
 * @param list
 * @param elem el elemento a insertar
 * @return 1 si lo agregó, 0 si no lo agegó
 */
int addList(listADT list, elemType elem);

int deleteList(listADT list, elemType elem);

int belongsList(const listADT list, elemType elem);

int sizeList(const listADT list);

void freeList(listADT list);

void toBegin(listADT list);

/**
 *
 * @param list
 * @return 1 si hay un elemento siguiente a consultar
 * El usuario ANTES tuvo que llamar a toBegin y en forma opcional next
 */
int hasNext(const listADT list);

/**
 *
 * @param list
 * @return el siguiente elemento. Retorna el primero si se acaba de llamar
 * a toBegin. Si no hay siguiente elemento, aborta
 * El usuario ANTES tuvo que llamar a toBegin
 */
elemType next(listADT list);
elemType getList (const listADT list, size_t idx);

void map(listADT list, elemType (* function)(elemType elem1));

#endif 