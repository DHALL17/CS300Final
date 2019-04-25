#include <stdlib.h>
#include <stdio.h>

long queue[10000];
int qi = 0;

typedef struct {
    int loc  : 30;
    int proc : 30;
} rec; //record

typedef union {
    rec X;
    long y;
} rec_un; //record union?

void enqueue(long n) {
    queue[qi++] = n;
}

void dequeue(long pos) {
    int r = queue[pos];
    queue[pos] = queue[--qi];
    return r;
}

int loadQueue(int cTime) {
    static int t = 0;
    static rec_un x;
    int loc;
    int proc;

    while(t <= cTime) {
        if(t > 0) enqueue(x.y);
        scanf("%d%d%d\n", &t, &loc, &proc);
        x.x.loc = loc;
        x.x.proc = proc;
    }
}

int main(void) {
    while(!EOF) {
        LOAD
        Process
    }
}