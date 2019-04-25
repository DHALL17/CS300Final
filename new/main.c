#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "queue.h"

double clock_time;
double memorySize;

// void stats(Process *p) {
// 	int p0count = 0, p1count = 0, p2count = 0, p3count = 0;	// Each Priorities process count
// 	double p0life = 0, p1life = 0, p2life = 0, p3life = 0;	// Each Priorities life span
// 	double p0int = 0, p1int = 0, p2int = 0, p3int = 0;		// Each priorities max intervals
// 	return;
// }

void execution_engine(QUEUE *q, QUEUE *Completed) {
	Process *p = dequeue(q);
	// Will run until block count == 0 or execution time passes runtime or reaches the quantum
	double quantum = clock_time + 0.01; // quantum is 10ms 1ms = 0.001s
	while(clock_time < quantum) {
		double block = ((rand() % 19) + 1) / 1000000; // A block represents a microsecond 1us = 0.000001s
		int address = rand() % 1000001;
		p->block_count--;

		clock_time += block;
		p->exec_time += block;

		if(p->block_count == 0 || p->exec_time > p->runtime) {
			if(p->block_count == 0) {
				// printf("Block Count was exceeded\n");
				enqueue(Completed, p);
				return;
			}
			
			if(p->exec_time > p->runtime) {
				// printf("Execution Time passed Run Time\n");
				enqueue(Completed, p);
				return;
			}
		}
	}
	// Enqueues the process back into the queue it was in after it reaches the quantum
	enqueue(q, p);
	return;
}

void createProcess(QUEUE **Memory, QUEUE **Disk) {
	Process *p = newProcess(clock_time);
	// printf("Process Name: %s\n", p->pname);
	// printf("Process Priority: %i\n", p->priority);
	// printf("Process Memory: %i\n", p->memory);
	// printf("Process Runtime: %f\n", p->runtime);
	// printf("Process Block Count: %i\n\n", p->block_count);
	// printf("Current MemorySize: %f\n", memorySize);
	// Only a processes working set is placed in memory
	double workingSet = p->memory / 10;
	// If the processes working set doesn't fit in memory it goes to Disk
	if(memorySize - workingSet < 0) {
		enqueue(Disk[p->priority], p);
		// printf("%s enqueued to Disk\n\n", p->pname);
	}
	// If there is room in Memory the Processes working set is placed in memory
	else {
		memorySize -= workingSet;
		enqueue(Memory[p->priority], p);
		// printf("%s enqueued to Memory\n\n", p->pname);
	}
	
	return;
}

int main(void) {
	clock_time = 0;
	memorySize = 100;
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
	for(int x = 0; x < 5; ++x)
		createProcess(Memory, Disk);
	
	int processCount;
	do {
		processCount = 0;

		for(int x = 0; x < 4; ++x) {
			int prioritySize = sizeQUEUE(Memory[x]);
			for(int y = 0; y < prioritySize; ++y) {
				execution_engine(Memory[x], Completed);
			}
			processCount += prioritySize;
		}

		for(int x = 0; x < 4; ++x) {
			int prioritySize = sizeQUEUE(Disk[x]);
			for(int y = 0; y < prioritySize; ++y) {
				execution_engine(Disk[x], Completed);
			}
			processCount += prioritySize;
		}
		

	} while(processCount != 0);

	int size = sizeQUEUE(Completed);

	for(int x = 0; x < size; ++x) {
		Process *p = dequeue(Completed);
	}
}