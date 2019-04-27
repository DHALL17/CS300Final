#include <stdlib.h>
#include <stdio.h>
#include "process.h"

Process *newProcess(double clock_time) {
    static int pnum = 0;
    if(pnum == 2200)
        return NULL;
    Process *p = malloc(sizeof(Process));
    p->pID = pnum++;
    for(int x = 0; x < 256; ++x)
        for(int y = 0; y < 3; ++y)
            p->pageTable[x][y] = 0;
    for(int x = 0; x < 3; ++x)
        p->diskRequest[x] = 0;
    p->LRU = newLRU();
    p->pageCount = 100;
    p->priority = rand() % 4;                   // Priority of the process
    p->memory = (rand() % 191) + 10;            // Amount of memory required for the process
    p->runtime = (rand() % 39601) + 400;        // Maximum allowed run-time
    p->runtime /= 1000.0;                       // Runtime is represented in ms
    p->block_count = (rand() % 99001) + 1000;   // Number of blocks to create
    p->exec_time = 0;
    p->blocked_time = 0;
    p->start_time = clock_time;
    p->max_int = 0;
    return p;
}

int getDirty(Process *p, int index)
{
    return p->pageTable[index][0];
}

void setDirty(Process *p, int index, int flag)
{
    p->pageTable[index][0] = flag;
}

int getMemory(Process *p, int index)
{
    return p->pageTable[index][1];
}

void setMemory(Process *p, int index, int address)
{
    p->pageTable[index][1] = address;
}

int getDisk(Process *p, int index)
{
    return p->pageTable[index][2];
}

void setDisk(Process *p, int index, int address)
{
    p->pageTable[index][2] = address;
}

int isHolding(Process *p)
{
    return (p->diskRequest[2]) ? 1 : 0;
}

void cleanHolding(Process *p)
{
    for(int x = 0; x < 3; ++x)
        p->diskRequest[x] = 0;
}

void freeProcess(void *process) {
    Process *p = (Process*) process;
	free(p);
}