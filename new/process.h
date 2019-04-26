#ifndef __PROCESS_INCLUDED__
#define __PROCESS_INCLUDED__

#include "lru.h"

typedef struct Process {
	char pname[10];
	int pageTable[256][3];
	LRU *LRU;
	int pageCount,
	priority,
	memory,
    block_count;
	double blocked,
    runtime,
	exec_time,
	start_time,
	end_time,
	max_int,
	clock_int;
} Process;

Process *newProcess(double clock_time);
void freeProcess(void *process);

#endif