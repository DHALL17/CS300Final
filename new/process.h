#ifndef __PROCESS_INCLUDED__
#define __PROCESS_INCLUDED__

#include "lru.h"

typedef struct Process {
	int pID;
	int pageTable[256][3];
	int diskRequest[3];
	LRU *LRU;
	int pageCount,
	priority,
	memory,
    block_count;
	double blocked_time,
    runtime,
	exec_time,
	start_time,
	end_time,
	max_int,
	clock_int;
} Process;

Process *newProcess(double clock_time);

int getDirty(Process *p, int index);
void setDirty(Process *p, int index, int flag);

int getMemory(Process *p, int index);
void setMemory(Process *p, int index, int address);

int getDisk(Process *p, int index);
void setDisk(Process *p, int index, int address);

int isHolding(Process *p);
void cleanHolding(Process *p);

void freeProcess(void *process);

#endif