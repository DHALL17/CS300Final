#ifndef __CDA_INCLUDED__
#define __CDA_INCLUDED__

#include <stdio.h>

typedef struct cda CDA;

CDA *newCDA(void (*free)(void *));
void insertCDA(CDA *items,int index,void *value);
void *removeCDA(CDA *items,int index);
void *getCDA(CDA *items,int index);
void *setCDA(CDA *items,int index,void *value);
int sizeCDA(CDA *items);
void freeCDA(CDA *);

#define insertCDAfront(items,value) insertCDA(items,0,value)
#define insertCDAback(items,value)  insertCDA(items,sizeCDA(items),value)
#define removeCDAfront(items)       removeCDA(items,0)
#define removeCDAback(items)        removeCDA(items,sizeCDA(items)-1)

#endif