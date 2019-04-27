#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    FILE *fp = fopen("requests", "w");
    
    double time = 0;
    
    while (1)
    {
        for (int i = 0; i < rand() % 100; i++)
        {
            time += rand() % 1023 / 1000000.0;
            fprintf(fp, "%24lf %14d %9d\n", time, rand() % (1 << 20), rand() % 100);
            fflush(fp);
        }
        sleep(2);
    }
    
    fclose(fp);
}


