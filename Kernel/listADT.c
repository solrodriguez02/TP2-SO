#include <stdio.h>
#include <listADT.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

typedef struct node {
    elemType elem;
    struct node * tail;
} TNode;

typedef TNode * TList;

struct listCDT {
    size_t size;    // Cantidad de elementos
    TList first;
    int (* compare)(elemType elem1, elemType elem2);
    TList next; // auxiliar para los iteradores
};

listADT newList(int (* compare)(elemType elem1, elemType elem2)) {
    listADT aux = malloc(sizeof(struct listCDT));
    aux->size = 0;
    aux->first= NULL;
    aux->compare = compare;
    aux->next = NULL;
    return aux;
}

// Pensar que pasa si compare es "pesada"
static TList addRec(TList list, elemType elem,
                    int (* compare)(elemType elem1, elemType elem2),
                    int * flag) {
    int status;

    if ( list==NULL || (status = compare(elem,list->elem)) < 0) {
        TList aux = malloc(sizeof(TNode));
        aux->elem = elem;
        aux->tail = list;
        *flag =1;
        return aux;
    }
    if ( status > 0) {
        list->tail = addRec(list->tail, elem, compare, flag);
    }
    return list;
}

int addList(listADT list, elemType elem) {
    int flag=0;
    list->first = addRec(list->first, elem, list->compare, &flag);
    list->size += flag;
    return flag;
}

static TList deleteRec(TList list, elemType elem,
                       int (* compare)(elemType elem1, elemType elem2),
                       int * flag) {
    int c;
    if (list == NULL || (c = compare(elem, list->elem)) < 0) {
        return list;
    }
    if (c == 0) {
        TList aux = list->tail;
        free(list);
        *flag = 1;
        return aux;
    }
    list->tail = deleteRec(list->tail, elem, compare, flag);
    return list;
}

int deleteList(listADT list, elemType elem) {
    int flag = 0;
    list->first = deleteRec(list->first, elem, list->compare, &flag);
    list->size -= flag;
    return flag;
}

static int belongsRec(TList first, elemType elem, int (* compare)(elemType elem1, elemType elem2)) {
    if ( first == NULL || compare(elem, first->elem) < 0)
        return 0;
    if (compare(first->elem, elem)==0)
        return 1;
   /* int status = compare(elem,first->elem);
    if (status <= 0)
        return -1 * status;*/
    return belongsRec(first->tail, elem, compare);
}

int belongsList(const listADT list, elemType elem) {
    return belongsRec(list->first, elem, list->compare);
}

int sizeList(const listADT list) {
    return list->size;
}

// Auxiliar, solo se usa en este .c
static void freeListRec(TList first) {
    if ( first == NULL)
        return;
    TList aux = first->tail;
    free(first);
    freeListRec(aux);
}

void freeList(listADT list) {
    freeListRec(list->first);
    free(list);
}

void toBegin(listADT list) {
    list->next = list->first;   // el siguiente será el primero
}

int hasNext(const listADT list) {
    return list->next != NULL;
}

elemType next(listADT list) {
    // Chequeo opcional
    assert(hasNext(list));
    elemType aux = list->next->elem;
    list->next = list->next->tail;
    return aux;
}

static int isEmpty (listADT list){
    return list->first == NULL;
}

elemType getList (const listADT list, size_t idx) {
    assert(idx < sizeList(list)); //aborta en ejecución
    TList aux = list->first; // no puedo modificar el first directamente sino pierdo la lista
    for(int i=0; i < idx; i++ ) { //si el idx = 0 devuelve el elemento en el que estoy
        aux = aux->tail;
    }
    return aux->elem;
}

void map(listADT list, elemType (* function)(elemType elem1)){
    TList aux = list->first; //me manejo con un auxiliar para no modificar la lista
    while (aux != NULL){
        aux->elem = function(aux->elem);
        aux = aux->tail;
    }
}

static elemType doble(elemType elem){
    return elem * 2;
}
static elemType equals(elemType elem1, elemType elem2){
    return elem1 == elem2;
}
