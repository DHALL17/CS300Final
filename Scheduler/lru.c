#include <stdio.h>
#include <stdlib.h>
#include "lru.h"

struct lru{
    int array[100];
    int frontIndex;
    int endIndex;
};

LRU *newLRU()
{
    LRU *lru = malloc(sizeof(LRU));
    for(int x = 0; x < 100; ++x)
        lru->array[x] = 0;
    lru->frontIndex = 0;
    lru->endIndex = 0;
    return lru;
}

int peekLru(LRU *lru)
{
    return lru->array[lru->frontIndex];
}

void enqueueLru(LRU *lru, int value)
{
    lru->array[lru->endIndex] = value;
    lru->endIndex = (lru->endIndex + 1) % 100;
}

int dequeueLru(LRU *lru)
{
    int address = lru->array[lru->frontIndex];
    lru->frontIndex = (lru->frontIndex + 1) % 100;
    return address;
}