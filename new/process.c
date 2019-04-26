#include <stdlib.h>
#include <stdio.h>
#include "process.h"

Process *newProcess(double clock_time) {
    static int pnum = 0;
    if(pnum == 42)
        return NULL;
    Process *p = malloc(sizeof(Process));
    sprintf(p->pname, "P%i", pnum++);
    for(int x = 0; x < 256; ++x)
        for(int y = 0; y < 3; ++y)
            p->pageTable[x][y] = 0;
    p->LRU = newLRU();
    p->pageCount = 100;
    p->priority = rand() % 4;                   // Priority of the process
    p->memory = (rand() % 191) + 10;            // Amount of memory required for the process
    p->runtime = (rand() % 39601) + 400;        // Maximum allowed run-time
    p->runtime /= 1000.0;                       // Runtime is represented in ms
    p->block_count = (rand() % 99001) + 1000;   // Number of blocks to create
    p->exec_time = 0;
    p->blocked = 0;
    p->start_time = clock_time;
    p->max_int = 0;
    return p;
}

void freeProcess(void *process) {
    Process *p = (Process*) process;
	free(p);
}