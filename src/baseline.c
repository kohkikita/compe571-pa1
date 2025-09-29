#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "config.h"

#define NUM_THREADS 4   // adjust as needed

unsigned long long partial_sums[NUM_THREADS];

void *thread_sum(void *arg) {
    int id = *(int *)arg;
    unsigned long long start = id * (N / NUM_THREADS) + 1;
    unsigned long long end   = (id + 1) * (N / NUM_THREADS);
    unsigned long long total = 0;

    for (unsigned long long i = start; i <= end; i++) {
        total += i;
    }

    partial_sums[id] = total;
    return NULL;
}

int main(void) {
    clock_t start, end;
    double cpu_time_used;
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    start = clock();  // start timing

    // create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_sum, &ids[i]);
    }

    // wait for all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // combine results
    unsigned long long result = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        result += partial_sums[i];
    }

    end = clock();  // end timing
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Sum = %llu\n", result);
    printf("Time taken: %f seconds\n", cpu_time_used);

    return 0;
}
