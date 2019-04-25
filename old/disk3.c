#include <stdlib.h>
#include <stdio.h>

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
void load(unsigned long long time);
int smallest();
int largest();
unsigned long long process(unsigned long long time);

int main()
{
    inf = fopen("requests", "r");
    outf = fopen("results", "w");
    
    unsigned long long time = 0;
    while (1)
    {
        load(time);
        time = process(time);
    }
    
    fclose(outf);
    fclose(inf);
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

void load(unsigned long long time)
{
    static unsigned long long t = 0;
    static rec_un x = {0};
    int loc;
    int proc;
    
    if (t > time)
    {
        time = t;
    }
    while (t <= time)
    {
        if (x.y != 0)
        {
            enq(x.y);
        }
        while (fscanf(inf, "%24llu %14d %9d\n", &t, &loc, &proc) <= 0)
        {
            fseek(inf, -1, SEEK_CUR);
        }
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

unsigned long long process(unsigned long long time)
{
    static int dir = 0;
    
    if (dir)
    {
        while (qi)
        {
            rec_un y;
            y.y = deq(smallest());
            time += 5;
            fprintf(outf, "%24llu %9d\n", time, y.x.proc);
        }
    }
    else
    {
        while (qi)
        {
            rec_un y;
            y.y = deq(largest());
            time += 5;
            fprintf(outf, "%24llu %9d\n", time, y.x.proc);
        }
    }
    fflush(outf);
    dir ^= 1;
    return time;
}