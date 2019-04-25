#ifndef __PROCESS_INCLUDED__
#define __PROCESS_INCLUDED__

typedef struct Process {
	char pname[10];
	int priority, 
	memory,
    block_count;
	double blocked,
    runtime,
	exec_time,
	start_time,
	end_time,
	max_int;
} Process;

Process *newProcess(double clock_time);
void freeProcess(void *process);

#endif