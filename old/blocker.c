#include <stdlib.h>
#include <stdio.h>


int main(void) {
    int cTime = 0;
    int block = 0; // rand() % 1million
    int procID = 0;

    for(int x = 0; x < 10000; ++x) {
        cTime += rand() % 11;
        block = rand() % (1 << 20); //rand() % 1million
        procID = rand() % 100;
        printf("%d %d %d\n", cTime, block, procID);
    }
}