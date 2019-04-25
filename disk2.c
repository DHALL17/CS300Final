/*******************************************************************************
disk.c
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>

// Don't know where to put this yet
// 262,244 disk blocks
// 4096 memory blocks

// typedef struct
// {
//     long memory: 12;
//     long disk: 18;
//     long dirty: 1;
// } pageTableEntry;

typedef struct
{
    unsigned int loc  : 30;
    unsigned int proc : 30;
} rec;

typedef union
{
    rec x;
    long y;
} rec_un;

FILE *inf;
FILE *outf;

long q[10000];
int qi = 0;

void enq(long n);
long deq(int pos);
void load(int time);
int smallest();
int largest();
int process(int time);

int main()
{
    inf = fopen("requests", "r");
    outf = fopen("results", "w");

    long time = 0;
    while (!feof(inf))
    {
        load(time);
        time = process(time);
    }
}

void enq(long n)
{
    q[qi++] = n;
}

long deq(int pos)
{
    long r = q[pos];
    q[pos] = q[--qi];
    return r;
}

void load(int time)
{
    static int t = -1;
    static rec_un x;
    int loc;
    int proc;
    
    if (t > time)
    {
        time = t;
    }
    while (t <= time)
    {
        if (t >= 0)
        {
            enq(x.y);
        }
        fscanf(inf, "%d %d %d\n", &t, &loc, &proc);
        x.x.loc = loc;
        x.x.proc = proc;
    }
}

int smallest()
{
    int si = 0;
    
    for (int i = 1; i < qi; i++)
    {
        if (q[i] < q[si])
        {
            si = i;
        }
    }
    return si;
}

int largest()
{
    int li = 0;
    
    for (int i = 1; i < qi; i++)
    {
        if (q[i] > q[li])
        {
            li = i;
        }
    }
    return li;
}

int process(int time)
{
    static int dir = 0;

    if (dir)
    {
        while (qi)
        {
            rec_un y;
            y.y = deq(smallest());
            time += 5;
            printf("%d %d\n", time, y.x.proc);
            fprintf(outf, "%d %d\n", time, y.x.proc);
        }
    }
    else
    {
        while (qi)
        {
            rec_un y;
            y.y = deq(largest());
            time += 5;
            printf("%d %d\n", time, y.x.proc);
            fprintf(outf, "%d %d\n", time, y.x.proc);
        }
    }
    dir ^= 1;
    return time;
}