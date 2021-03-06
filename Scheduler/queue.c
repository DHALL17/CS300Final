#include <stdlib.h>
#include <assert.h>
#include "cda.h"
#include "queue.h"
/**************************
*   queue.c by David Hall *
**************************/
struct queue{
    CDA *cda;
    int size;
};

QUEUE *newQUEUE(void (*freeElement)(void *)){
    QUEUE *queue = malloc(sizeof(QUEUE));
    assert(queue != 0);
    queue->cda = newCDA(freeElement);
    queue->size = 0;
    return queue;
}

//Adds an element to the back of the array
void  enqueue(QUEUE *items,void *value){
    insertCDAback(items->cda, value);
    items->size++;
    return;
}

//Removes an element from the front of the array
void *dequeue(QUEUE *items){
    assert(items->size != 0);
    void *temp = removeCDAfront(items->cda);
    items->size--;
    return temp;
}

//Shows the element to be removed next
void *peekQUEUE(QUEUE *items){
    assert(items->size != 0);
    return getCDA(items->cda, 0);
}

// Breaks queue rules by inserting into the front but it's ok
void reQUEUE(QUEUE *items, void *value){
    insertCDAfront(items->cda, value);
    items->size++;
    return;
}

//Returns the size of the array
int   sizeQUEUE(QUEUE *items){
    return items->size;
}

// Returns the size of the Queue array
// Can be assumed to have 4 indexes in this project
int sizeQUEUEarray(QUEUE **items){
    int size = 0;
    for(int x = 0; x < 4; ++x)
        size += sizeQUEUE(items[x]);
    return size;
}

//Free's the array
void  freeQUEUE(QUEUE *items){
    freeCDA(items->cda);
    free(items);
    return;
}