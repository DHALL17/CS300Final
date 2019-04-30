#ifndef __LRU_INCLUDED__
#define __LRU_INCLUDED__

typedef struct lru{
    int array[100];
    int endIndex;
} LRU;
LRU *newLRU(void);

int peekLru(LRU *lru);
int lookUp(LRU *lru, int index);
void insertLru(LRU *lru, int value);
void deleteLru(LRU *lru, int index);
int getLru(LRU *lru);

#endif