#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "queue.h"
#include "memoryAllocator.h"
#include "lru.h"

double clock_time;
static double systemMemory;
static double new_process;
FILE *Requests;
FILE *Results;

// void stats(Process *p) {
// 	int p0count = 0, p1count = 0, p2count = 0, p3count = 0;	// Each Priorities process count
// 	double p0life = 0, p1life = 0, p2life = 0, p3life = 0;	// Each Priorities life span
// 	double p0int = 0, p1int = 0, p2int = 0, p3int = 0;		// Each priorities max intervals
// 	return;
// }

void endProcess(QUEUE *Completed, Process *p)
{
	double workingSet = p->memory / 10;
	systemMemory += workingSet;
	for(int x = 0; x < 256; ++x)
		if(p->pageTable[x][1] != 0)
			put_mem(p->pageTable[x][1]);
	enqueue(Completed, p);
}

void createProcess(QUEUE **Memory, QUEUE **Disk)
{
	Process *p = newProcess(clock_time);
	if(p == NULL)
		return;

	double workingSet = p->memory / 10;
	if(systemMemory - workingSet < 0)
	{
		enqueue(Disk[p->priority], p);
	} else
	{
		systemMemory -= workingSet;
		enqueue(Memory[p->priority], p);
	}
}

void unblock()
{

}

void diskRequest(Process *p, int address, int dirty, QUEUE *Blocked)
{
	double uTime;
	int uId;
	fprintf(Requests, "%18.6f %14i %9i\n", clock_time, address, p->pID);
	enqueue(Blocked, p);
	while(fscanf(Results, "%24f %9i\n", &uTime, &uId) > 0)
	{
		printf("%18.6f %9i\n", uTime, uId);
		int size = sizeQUEUE(Blocked);
		for(int x = 0; x < size; ++x)
		{
			Process *b = dequeue(Blocked);
			if(b->pID == uId)
				b->blocked_time = uTime;
			enqueue(Blocked, b);
		}
	}
	fseek(Results, -1, SEEK_CUR);
}

void replace(Process *p, double block, int address, int dirty, QUEUE *Blocked)
{
	int lruAddress = dequeueLru(p->LRU);
	int memAddress = getMemory(p, lruAddress);
	if((getDisk(p, lruAddress) != 0 && getDirty(p, lruAddress) == 1) || getDisk(p, lruAddress) == 0)
	{
		diskRequest(p, lruAddress, 0, Blocked);
	}

	setMemory(p, lruAddress, 0);
	setMemory(p, address, memAddress);
	setDirty(p, address, getDirty(p, address) || dirty);

	enqueueLru(p->LRU, address);
	return;
}

/*
** 0 == Dirty Flag
** 1 == Memory Address
** 2 == Instruction Block
**/
int pageTableHandler(Process *p, double block, int address, int dirty, QUEUE *Blocked)
{
	if(getMemory(p, address) != 0)
	{
		setDirty(p, address, getDirty(p, address) || dirty);
		return 1;
	}
	p->diskRequest[0] = dirty; p->diskRequest[1] = address; p->diskRequest[2] = block;
	else if(p->pageCount)
	{
		if(getDisk(p, address) == 0)
		{
			int memAddress = get_mem();
			if(memAddress)
			{
				setMemory(p, address, memAddress);
				setDirty(p, address, 1);
				p->pageCount--;
				enqueueLru(p->LRU, address);
				return 1;
			}
			else
				replace(p, block, address, dirty, Blocked);
			return 0;
		}
		else
			diskRequest(p, block, address, dirty, Blocked);
		return 0;
	}
	else
		replace(p, block, address, dirty, Blocked);
	return 0;
}

void executionEngine(QUEUE *q, QUEUE *Completed, QUEUE **Memory, QUEUE **Disk, QUEUE *Blocked)
{
	Process *p = dequeue(q);
	if ( p->max_int == 0.0)
		p->max_int = clock_time - p->start_time;
	else
	{
		double temp = clock_time - p->clock_int;
		if(temp > p->max_int)
			p->max_int = temp;
	}

	double quantum = clock_time + 0.01; // quantum is 10ms 1ms = 0.001s
	while(clock_time < quantum)
	{
		double block = ((rand() % 19) + 1) / 1000000.0; // A block represents a microsecond 1us = 0.000001s
		int address = rand() % 1000000;
		int dirty = rand() % 2;
		address = address >> 12;	// Only care about upper 8 bits
		p->block_count--;
		if(!pageTableHandler(p, block, address, dirty, Blocked))
			return;
		clock_time += block;
		p->exec_time += block;

		if(new_process <= clock_time)
		{
			createProcess(Memory, Disk);
			new_process = clock_time + (((rand() % 9981) + 20) / 1000.0);
		}

		if(p->block_count == 0 || p->exec_time > p->runtime)
		{
			p->end_time = clock_time;
			if(p->block_count == 0)
			{
				// printf("Block Count was exceeded\n");
				endProcess(Completed, p);
				return;
			}
			
			if(p->exec_time > p->runtime)
			{
				// printf("Execution Time passed Run Time\n");
				endProcess(Completed, p);
				return;
			}
		}
	}

	p->clock_int = clock_time;
	enqueue(q, p);
	return;
}

void diskToMemory(QUEUE **Memory, QUEUE **Disk, Process *p)
{
	double workingSet = p->memory / 10;
	if(systemMemory - workingSet >= 0)
	{
		clock_time += 0.1;
		systemMemory -= workingSet;
		reQUEUE(Memory[p->priority], p);
		return;
	}
	else
	{
		for(int x = 3; x >= 0; --x)
		{
			int prioritySize = sizeQUEUE(Memory[x]);
			for(int y = 0; y < prioritySize; ++y)
			{
				Process *removed = dequeue(Memory[x]);
				double removedWorkingSet = removed->memory / 10;
				systemMemory += removedWorkingSet;
				enqueue(Disk[x], removed);
				if(systemMemory - workingSet >= 0)
				{
					clock_time += 0.1;
					systemMemory -= workingSet;
					reQUEUE(Memory[p->priority], p);
					return;
				}
			}
		}
	}
}

int main(void)
{
	Requests = fopen("requests", "w");
	Results = fopen("results", "r");
	clock_time = 0;
	systemMemory = 100;
	new_process = ((rand() % 9981) + 20) / 1000.0;
	srand(time(NULL));
	QUEUE **Memory = malloc(sizeof(QUEUE*) * 4);
	QUEUE *MP0 = newQUEUE(NULL);
	QUEUE *MP1 = newQUEUE(NULL);
	QUEUE *MP2 = newQUEUE(NULL);
	QUEUE *MP3 = newQUEUE(NULL);
	Memory[0] = MP0;
	Memory[1] = MP1;
	Memory[2] = MP2;
	Memory[3] = MP3;

	QUEUE **Disk = malloc(sizeof(QUEUE*) * 4);
	QUEUE *DP0 = newQUEUE(NULL);
	QUEUE *DP1 = newQUEUE(NULL);
	QUEUE *DP2 = newQUEUE(NULL);
	QUEUE *DP3 = newQUEUE(NULL);
	Disk[0] = DP0;
	Disk[1] = DP1;
	Disk[2] = DP2;
	Disk[3] = DP3;

	QUEUE *Completed = newQUEUE(freeProcess);
	QUEUE *Blocked = newQUEUE(NULL);

	for(int x = 0; x < 2; ++x)
		createProcess(Memory, Disk);
	
	//  ------   SCHEDULER  ------   //
	int processCount;
	do
	{
		processCount = 0;

		for(int x = 0; x < 4; ++x)
		{
			int prioritySize = sizeQUEUE(Memory[x]);
			for(int y = 0; y < prioritySize; ++y)
			{
				executionEngine(Memory[x], Completed, Memory, Disk, Blocked);
			}
			processCount += prioritySize;
		}

		for(int x = 0; x < 4; ++x)
		{
			int prioritySize = sizeQUEUE(Disk[x]);
			for(int y = 0; y < prioritySize; ++y)
			{
				diskToMemory(Memory, Disk, dequeue(Disk[x]));
				executionEngine(Memory[x], Completed, Memory, Disk, Blocked);
			}
			processCount += prioritySize;
		}

	} while(processCount != 0);
	//  ------   SCHEDULER  ------   //

	printf("Total Processes: %i\n", sizeQUEUE(Completed));
	int size = sizeQUEUE(Completed);
	for(int x = 0; x < size; ++x)
	{
		Process *p = dequeue(Completed);
		printf("Process Name: p%i\n", p->pID);
		printf("Process Priority: %i\n", p->priority);
		printf("Process Block_Count: %i\n", p->block_count);
		printf("Process RunTime: %f\n", p->runtime);
		printf("Process Ecec_Time: %f\n", p->exec_time);
		printf("Process Start_Time: %f\n", p->start_time);
		printf("Process End_Time: %f\n", p->end_time);
		printf("Process Max_Int: %f\n\n", p->max_int);

		for(int x = 0; x < 256; ++x)
			if(p->pageTable[x][1] != 0)
				printf("x: %i\tAddress 1: %i\n", x, p->pageTable[x][1]);
	}
}