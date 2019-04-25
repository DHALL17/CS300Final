#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    FILE *fp = fopen("requests", "w");
    
    unsigned long long time = 0;
    
    while (1)
    {
        for (int i = 0; i < rand() % 100; i++)
        {
            time += rand() % 11;
            fprintf(fp, "%24llu %14d %9d\n", time, rand() % (1 << 20), rand() % 100);
            fflush(fp);
        }
        sleep(2);
    }
    
    fclose(fp);
}


