#include <stdio.h>
#include <stdlib.h>
#include "lru.h"

LRU *newLRU()
{
    LRU *lru = malloc(sizeof(LRU));
    for(int x = 0; x < 100; ++x)
        lru->array[x] = -1;
    lru->endIndex = 0;
    return lru;
}

int peekLru(LRU *lru)
{
    return lru->array[0];
}

int lookUp(LRU *lru, int address)
{
    for(int x = 0; x < 100; ++x)
        if(lru->array[x] == address)
        {
            return x;
        }
    return -1;
}

// int peekLru(LRU *lru)
// {
//     return lru->array[lru->frontIndex];
// }

void insertLru(LRU *lru, int value)
{
    lru->array[lru->endIndex++] = value;
}

// void enqueueLru(LRU *lru, int value)
// {
//     lru->array[lru->endIndex] = value;
//     lru->endIndex = (lru->endIndex + 1) % 100;
// }

void deleteLru(LRU *lru, int index)
{
    int temp = index + 1;
    while(temp < lru->endIndex)
    {
        lru->array[index] = lru->array[temp];
        index++;
        temp = index + 1;
    }
    lru->array[index] = -1;
    lru->endIndex--;
}

// int dequeueLru(LRU *lru)
// {
//     int address = lru->array[lru->frontIndex];
//     lru->frontIndex = (lru->frontIndex + 1) % 100;
//     return address;
// }

int getLru(LRU *lru)
{
    int index = lru->array[0];
    deleteLru(lru, 0);
    return index;
}