#ifndef __LRU_INCLUDED__
#define __LRU_INCLUDED__

typedef struct lru LRU;

LRU *newLRU(void);
void enqueueLru(LRU *, int);
int dequeueLru(LRU *);

#endif