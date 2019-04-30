#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "queue.h"
#include "memoryAllocator.h"
#include "lru.h"

double clock_time;
double unblockInterval;
static double systemMemory;
static double new_process;
FILE *Requests;
FILE *Results;
int addressCount = 0;
int nonZero = 0;
int a = 0;

void endProcess(QUEUE *Completed, Process *p)
{
	double workingSet = p->memory / 10;
	systemMemory += workingSet;
	// for(int x = 0; x < 256; ++x)
	// {
	// 	if(p->pageTable[x][1] != 0)
	// 		put_mem(p->pageTable[x][1]);
	// 	if(p->pageTable[x][2] != 0)
	// 		put_disk(p->pageTable[x][2]);
	// }

	// printf("\n\nPROCESS %i FINISHED\n\n\n", p->pID);
	// printf("Process Name: p%i\n", p->pID);
	// printf("Process Priority: %i\n", p->priority);
	// printf("Process Block_Count: %i\n", p->block_count);
	// printf("Process Request Count: %i\n", p->requestCount);
	// printf("Process RunTime: %f\n", p->runtime);
	// printf("Process Ecec_Time: %f\n", p->exec_time);
	// printf("Process Start_Time: %f\n", p->start_time);
	// printf("Process End_Time: %f\n", p->end_time);
	// printf("Process Max_Int: %f\n\n", p->max_int);
	fflush(stdout);
	
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

int readUnblocks(QUEUE *Blocked)
{
	double uTime;
	int uId;
	int reads = 0;

	while(fscanf(Results, "%24lf %9i\n", &uTime, &uId) > 0)
	{
		int size = sizeQUEUE(Blocked);
		for(int x = 0; x < size; ++x)
		{
			fflush(stdout);
			Process *b = dequeue(Blocked);
			if(b->pID == uId)
				b->blocked_time = uTime;
			enqueue(Blocked, b);
		}
		reads++;
	}
	fseek(Results, -1, SEEK_CUR);
	return reads;
}

void checkUnblocked(QUEUE *Blocked, QUEUE **Disk, QUEUE **Memory)
{
	fflush(stdout);
	fprintf(Requests, "%24lf %14i %9i\n", clock_time, 300, 50);
    fflush(Requests);
	readUnblocks(Blocked);
	int size = sizeQUEUE(Blocked);
	for(int x = 0; x < size; ++x)
	{
		Process *b = dequeue(Blocked);
		if(b->blocked_time != 0)
		{
			if(clock_time >= b->blocked_time)
			{
				b->blocked_time = 0;
				double workingSet = b->memory / 10;
				if(systemMemory - workingSet < 0)
				{
					enqueue(Disk[b->priority], b);
				} else
				{
					systemMemory -= workingSet;
					enqueue(Memory[b->priority], b);
				}
			} else
			{
				enqueue(Blocked, b);
			}
		} else
		{
			enqueue(Blocked, b);
		}
	}
}

void diskRequest(Process *p, int address, QUEUE *Blocked)
{
	p->requestCount++;
	setDirty(p, address, 0);
	fprintf(Requests, "%24lf %14i %9i\n", clock_time, address, p->pID);
    fflush(Requests);
	double workingSet = p->memory / 10;
	setDisk(p, address, get_disk());
	systemMemory += workingSet;
	enqueue(Blocked, p);
	readUnblocks(Blocked);
}

int replace(Process *p, int address, int dirty, QUEUE *Blocked)
{
	int lruAddress = peekLru(p->LRU);
	int memAddress = getMemory(p, lruAddress);
	if(getDirty(p, lruAddress) == 1 || getDisk(p, lruAddress))
	{
		put_mem(memAddress);
		diskRequest(p, lruAddress, Blocked);
		setMemory(p, lruAddress, 0);
		return 0;
	}

	setMemory(p, lruAddress, 0);
	setMemory(p, address, memAddress);
	setDirty(p, address, getDirty(p, address) || dirty);
	getLru(p->LRU);
	insertLru(p->LRU, lruAddress);

	return 1;
}

int pageTableHandler(Process *p, double block, int address, int dirty, QUEUE *Blocked)
{
	int index = lookUp(p->LRU, address);
	if(index != -1)
	{
		deleteLru(p->LRU, index);
		insertLru(p->LRU, address);
	}

	if(getMemory(p, address) != 0)
	{
		setDirty(p, address, getDirty(p, address) || dirty);
		nonZero++;
		return 1;
	}
	else if(p->pageCount)
	{
		int memAddress = get_mem();
		if(memAddress)
		{
			if(getDisk(p, address) == 0)
			{
				setMemory(p, address, memAddress);
				setDirty(p, address, 1);
				p->pageCount--;
				insertLru(p->LRU, address);
				return 1;
			}

			setHolding(p, dirty, address, block);
			diskRequest(p, address, Blocked);
			return 0;
		}

		setHolding(p, dirty, address, block);
		if(replace(p, address, dirty, Blocked))
			diskRequest(p, address, Blocked);
		return 0;
	}

	setHolding(p, dirty, address, block);
	return (replace(p, address, dirty, Blocked)) ? 1 : 0;
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
		double block;
		int address;
		int dirty;

		if(isHolding(p))
		{
			block = p->diskRequest[2];
			address = p->diskRequest[1];
			dirty = p->diskRequest[0];
			clearHolding(p);
		} else
		{
			block = ((rand() % 19) + 1) / 1000000.0; // A block represents a microsecond 1us = 0.000001s
			address = a++ % 100;
			dirty = rand() % 2;
			p->block_count--;
			addressCount++;
		}

		if(p->block_count >= 0)
		{
			if(!pageTableHandler(p, block, address, dirty, Blocked))
			{
				p->clock_int = clock_time;
				return;
			}
			clock_time += block;
			p->exec_time += block;

			// if(new_process <= clock_time)
			// {
			// 	createProcess(Memory, Disk);
			// 	new_process = clock_time + (((rand() % 9981) + 20) / 1000.0);
			// }

			if(clock_time >= unblockInterval)
			{
				checkUnblocked(Blocked, Disk, Memory);
				unblockInterval += 0.005;
			}
		}

		if(p->block_count <= 0 || p->exec_time > p->runtime)
		{
			p->end_time = clock_time;
			if(p->block_count == 0)
			{
				endProcess(Completed, p);
				return;
			}
			
			if(p->exec_time > p->runtime)
			{
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
					systemMemory -= workingSet;
					reQUEUE(Memory[p->priority], p);
					return;
				}
			}
		}
	}
	printf("PROBLEM IN DISKTOMEMORY\n");
	exit(1);
}

int main(void)
{
	Requests = fopen("requests", "w");
	Results = fopen("results", "w");
	fclose(Results);
	Results = fopen("results", "r");
	clock_time = 0;
	unblockInterval = 0.005;
	systemMemory = 100.0;
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

	for(int x = 0; x < 1; ++x)
		createProcess(Memory, Disk);

	//  ------   SCHEDULER  ------   //
	int processCount;
sched:
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

	if(processCount != 0)
	{
		goto sched;
	}
	else if(sizeQUEUE(Blocked))
	{
		clock_time = unblockInterval;
		unblockInterval += 0.005;
		checkUnblocked(Blocked, Disk, Memory);
		goto sched;
	}
	//  ------   SCHEDULER  ------   //

	printf("Total Processes: %i\n", sizeQUEUE(Completed));
	int size = sizeQUEUE(Completed);
	for(int x = 0; x < size; ++x)
	{
		Process *p = dequeue(Completed);
		printf("\n\nPROCESS %i FINISHED\n\n\n", p->pID);
		printf("Process Name: p%i\n", p->pID);
		printf("Process Priority: %i\n", p->priority);
		printf("Process Block_Count: %i\n", p->block_count);
		printf("Process Request Count: %i\n", p->requestCount);
		printf("Process RunTime: %f\n", p->runtime);
		printf("Process Ecec_Time: %f\n", p->exec_time);
		printf("Process Start_Time: %f\n", p->start_time);
		printf("Process End_Time: %f\n", p->end_time);
		printf("Process Max_Int: %f\n\n\n", p->max_int);

		printf("Address Count: %i\n", addressCount);
		printf("Non Zero: %i\n", nonZero);
		for(int x = 0; x < 100; ++x)
			printf("Page %i: %i\n", x, p->LRU->array[x]);
		fflush(stdout);
	}

	fclose(Requests);
	fclose(Results);
}