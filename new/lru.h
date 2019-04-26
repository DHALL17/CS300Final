#ifndef __LRU_INCLUDED__
#define __LRU_INCLUDED__

typedef struct lru LRU;

LRU *newLRU(void);
void enqueueInt(LRU *, int);
int dequeueInt(LRU *);

#endif