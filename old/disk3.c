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
void load(double time);
int smallest();
int largest();
double process(double time);

int main()
{
    inf = fopen("requests", "r");
    outf = fopen("results", "w");
    
    double time = 0;
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

void load(double time)
{
    static double t = 0;
    static rec_un x = {0};
    int loc;
    int proc;
    
    if (t > time)
    {
        time = t;
    }
    while (t <= time)
    {
        while (fscanf(inf, "%24lf %14i %9i\n", &t, &loc, &proc) > 0)
        {
            x.x.loc = loc;
            x.x.proc = proc;
            enq(x.y);
        }
        fseek(inf, -1, SEEK_CUR);
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

double process(double time)
{
    static int dir = 0;
    
    if (dir)
    {
        while (qi)
        {
            rec_un y;
            y.y = deq(smallest());
            time += 5.0 / 1000.0;
            fprintf(outf, "%18.6lf %9i\n", time, y.x.proc);
        }
    }
    else
    {
        while (qi)
        {
            rec_un y;
            y.y = deq(largest());
            time += 5.0 / 1000.0;
            fprintf(outf, "%18.6lf %9i\n", time, y.x.proc);
        }
    }
    fflush(outf);
    dir ^= 1;
    return time;
}