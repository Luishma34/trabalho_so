#include <stdio.h>
#include <time.h>
#include <math.h>

#define NUM_TERMS 2000000000LL

int main(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    double sum = 0.0;
    double sign = 1.0;
    for (long k = 0; k < NUM_TERMS; k++) {
        sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }
    double pi = 4.0 * sum;

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Pi aproximado: %.9f\n", pi);
    printf("Total Processo (Sequencial): %.2f s\n", time_spent);
    return 0;
}