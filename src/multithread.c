#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"

// results array allocated in main so we can keep the code tiny
static unsigned long long *partial_sums;

void *thread_sum(void *arg) {
    int id = *(int *)arg;

    unsigned long long chunk = N / NUM_THREADS;
    unsigned long long start = id * chunk + 1;
    // last thread takes any remainder
    unsigned long long end   = (id == NUM_THREADS - 1) ? N : (start + chunk - 1);

    unsigned long long total = 0;
    for (unsigned long long i = start; i <= end; i++) total += i;

    partial_sums[id] = total;
    return NULL;
}

int main(void) {
    clock_t start_t, end_t;
    double cpu_time_used;

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    partial_sums = (unsigned long long *)malloc(sizeof(unsigned long long) * NUM_THREADS);
    if (!partial_sums) { perror("malloc"); return 1; }

    start_t = clock();  // Start timing

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_sum, &ids[i]);
    }

    // Wait for all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Combine results
    unsigned long long result = 0;
    for (int i = 0; i < NUM_THREADS; i++) result += partial_sums[i];

    end_t = clock();  // End timing
    cpu_time_used = ((double)(end_t - start_t)) / CLOCKS_PER_SEC;

    printf("Sum = %llu\n", result);
    printf("Time taken: %f seconds\n", cpu_time_used);

    free(partial_sums);
    return 0;
}
