#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "queue.h"

typedef struct Process {
    int
    priority,
    memory,
    blocked,
    in_blocks;
    double
    runtime,
    exec_time,
    start_time,
    end_time,
    max_int;
} Process;

void dam(char *s) {
    printf("%s\n", s);
    exit(1);
}

Process *newProcess(void) {
    Process *p = malloc(sizeof(Process));
    p->priority = rand() % 4;
    p->memory = (rand() % 191) + 10;
    p->runtime = (rand() % 39601) + 400;
    p->in_blocks = (rand() % 99001) + 1000;
    p->blocked = 0;
    p->runtime = 0;
    p->exec_time = 0;
    p->end_time = 0;
    p->max_int = 0;
    return p;
}

void run(Process *p){
    if(p->blocked) {

    }

    
}

int main(int argc, char *argv[]) {
    QUEUE **Memory = malloc(sizeof(QUEUE*) * 4);
    QUEUE *P0 = newQUEUE( free );
    QUEUE *P1 = newQUEUE( free );
    QUEUE *P2 = newQUEUE( free );
    QUEUE *P3 = newQUEUE( free );
    Memory[ 0 ] = P0;
	Memory[ 1 ] = P1;
	Memory[ 2 ] = P2;
	Memory[ 3 ] = P3;

    for(int x = 0; x < argv[1]; ++x) {
        Process *p = newProcess();
        enqueue(Memory[p->priority], p);
    }


    while(1) {
        for(int x = 0; x < 4; ++x) {
            int queueSize = sizeQUEUE(Memory[x]);
            for(int y = 0; y < queueSize; ++x){
                Process *p = dequeue(Memory[x]);
                run(p);
            }
        }
    }
}