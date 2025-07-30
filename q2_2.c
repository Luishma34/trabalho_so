#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define NUM_TERMS 2000000000LL
#define NUM_THREADS 32

double total_sum = 0.0;
double total_threads_time = 0.0;
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

double partialFormula(long first_term, long num_terms) {
    double local_sum = 0.0;
    double sign = (first_term % 2 == 0) ? 1.0 : -1.0;
    for (long k = first_term; k < num_terms; k++) {
        local_sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }
    return local_sum;
}

void *partialProcessing(void *args) {
    int id = *((int *)args);
    long partial = NUM_TERMS / NUM_THREADS;
    long start = id * partial;
    long end = start + partial;
    if (id == NUM_THREADS - 1) end = NUM_TERMS;

    struct timespec tstart, tend;
    clock_gettime(CLOCK_MONOTONIC, &tstart);

    double local_sum = partialFormula(start, end);

    clock_gettime(CLOCK_MONOTONIC, &tend);
    double ttime = (tend.tv_sec - tstart.tv_sec) + (tend.tv_nsec - tstart.tv_nsec) / 1e9;

    pthread_mutex_lock(&sum_mutex);
    total_sum += local_sum;
    total_threads_time += ttime;
    pthread_mutex_unlock(&sum_mutex);

    printf("TID: %lu: %.2f s\n", (unsigned long)pthread_self(), ttime);
    return NULL;
}

int main(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, partialProcessing, &ids[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double pi = 4.0 * total_sum;

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Pi aproximado: %.9f\n", pi);
    printf("Total Processo (Paralelo): %.2f s\n", time_spent);
    printf("Total Threads: %.2f s\n", total_threads_time);
    return 0;
}