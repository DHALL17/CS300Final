#ifndef __QUEUE_INCLUDED__
#define __QUEUE_INCLUDED__

#include <stdio.h>

typedef struct queue QUEUE;

QUEUE *newQUEUE(void (*)(void *));
void  enqueue(QUEUE *items,void *value);
void *dequeue(QUEUE *items);
void *peekQUEUE(QUEUE *items);
void reQUEUE(QUEUE *items, void *value);
int sizeQUEUE(QUEUE *items);
int sizeQUEUEarray(QUEUE **items);
void  freeQUEUE(QUEUE *items);

#endif