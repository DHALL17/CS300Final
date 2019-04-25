#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "queue.h"

double clock_time;
double systemMemory;
double new_process;
int totalProcesses;

// void stats(Process *p) {
// 	int p0count = 0, p1count = 0, p2count = 0, p3count = 0;	// Each Priorities process count
// 	double p0life = 0, p1life = 0, p2life = 0, p3life = 0;	// Each Priorities life span
// 	double p0int = 0, p1int = 0, p2int = 0, p3int = 0;		// Each priorities max intervals
// 	return;
// }

void endProcess(QUEUE *Completed, Process *p) {
	// Working set of a process is 10% of it's memory
	double workingSet = p->memory /10;
	// Replacing the memory the process used
	systemMemory += workingSet;
	enqueue(Completed, p);
	return;
}

void execution_engine(QUEUE *q, QUEUE *Completed, QUEUE **Memory, QUEUE **Disk) {
	Process *p = dequeue(q);
	// printf("Process Name in execution engine: %s\n", p->pname);
	// Will run until block count == 0 or execution time passes runtime or reaches the quantum
	double quantum = clock_time + 0.01; // quantum is 10ms 1ms = 0.001s
	while(clock_time < quantum)
	{
		double block = ((rand() % 19) + 1) / 1000000.0; // A block represents a microsecond 1us = 0.000001s
		int address = rand() % 1000001;
		p->block_count--;

		clock_time += block;
		p->exec_time += block;

		if(new_process <= clock_time)
		{
			createProcess(Memory, Disk);
			new_process = clock_time + (((rand() % 9981) + 20) / 1000.0);
		}

		if(p->block_count == 0 || p->exec_time > p->runtime)
		{
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
	// Enqueues the process back into the queue it was in after it reaches the quantum
	enqueue(q, p);
	return;
}

void createProcess(QUEUE **Memory, QUEUE **Disk) {
	totalProcesses++;
	Process *p = newProcess(clock_time);
	// Only a processes working set is placed in memory
	if(p == NULL)
		return;
	double workingSet = p->memory / 10;
	// If the processes working set doesn't fit in memory it goes to Disk
	if(systemMemory - workingSet < 0)
	{
		enqueue(Disk[p->priority], p);
	}
	// If there is room in Memory the Processes working set is placed in memory
	else
	{
		systemMemory -= workingSet;
		enqueue(Memory[p->priority], p);
	}
	
	return;
}

void diskToMemory(QUEUE **Memory, QUEUE **Disk, Process *p) {
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
		int prioritySize;
		for(int x = 3; x >= 0; --x)
		{
			prioritySize = sizeQUEUE(Memory[x]);
			// printf("PrioritySize: %i\n", prioritySize);
			// printf("Memory Size: %i\n", sizeQUEUE(Memory[x]));
			for(int y = 0; y < prioritySize; ++y)
			{
				// printf("Y Value: %i\n", y);
				Process *removed = dequeue(Memory[x]);
				double removedWorkingSet = removed->memory / 10;
				systemMemory += removedWorkingSet;
				enqueue(Disk[x], removed);
				// printf("systemMemory: %f\t workingSet: %f\tResult: %f\n", systemMemory, workingSet, systemMemory - workingSet);
				if(systemMemory - workingSet >= 0)
				{
					clock_time += 0.1;
					systemMemory -= workingSet;
					reQUEUE(Memory[p->priority], p);
					return;
				}
			}
			prioritySize = 0;
		}
	}
}

int main(void) {
	totalProcesses = 0;
	clock_time = 0;
	systemMemory = 100;
	new_process = ((rand() % 9981) + 20) / 1000.0;
	srand(time(NULL));
	QUEUE **Memory = malloc(sizeof(QUEUE*) * 4);
	QUEUE *MP0 = newQUEUE(freeProcess);
	QUEUE *MP1 = newQUEUE(freeProcess);
	QUEUE *MP2 = newQUEUE(freeProcess);
	QUEUE *MP3 = newQUEUE(freeProcess);
	Memory[0] = MP0;
	Memory[1] = MP1;
	Memory[2] = MP2;
	Memory[3] = MP3;

	QUEUE **Disk = malloc(sizeof(QUEUE*) * 4);
	QUEUE *DP0 = newQUEUE(freeProcess);
	QUEUE *DP1 = newQUEUE(freeProcess);
	QUEUE *DP2 = newQUEUE(freeProcess);
	QUEUE *DP3 = newQUEUE(freeProcess);
	Disk[0] = DP0;
	Disk[1] = DP1;
	Disk[2] = DP2;
	Disk[3] = DP3;

	QUEUE *Completed = newQUEUE(freeProcess);

	// Processes are created and placed onto Memory or Disk
	for(int x = 0; x < 12; ++x)
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
				execution_engine(Memory[x], Completed, Memory, Disk);
			}
			processCount += prioritySize;
		}

		for(int x = 0; x < 4; ++x)
		{
			int prioritySize = sizeQUEUE(Disk[x]);
			for(int y = 0; y < prioritySize; ++y)
			{
				diskToMemory(Memory, Disk, dequeue(Disk[x]));
				execution_engine(Memory[x], Completed, Memory, Disk);
			}
			processCount += prioritySize;
		}

	} while(processCount != 0);
	//  ------   SCHEDULER  ------   //
}