#ifndef __LRU_INCLUDED__
#define __LRU_INCLUDED__

typedef struct lru LRU;

LRU *newLRU(void);
int peekLru(LRU *lru);
void enqueueLru(LRU *, int);
int dequeueLru(LRU *);

#endif