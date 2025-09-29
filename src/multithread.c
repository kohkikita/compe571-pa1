#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "config.h"

// Array to hold partial sums from each thread
static unsigned long long *partial_sums;

// Each thread computes sum of its chunk
void *thread_sum(void *arg) {
    int id = *(int *)arg;
    
    // Determine the range of numbers this thread will sum
    unsigned long long start = id * (N / NUM_THREADS) + 1;
    unsigned long long end = (id == NUM_THREADS - 1) ? N : (start + (N / NUM_THREADS) - 1);

    unsigned long long total = 0;
    for (unsigned long long i = start; i <= end; i++) {
        total += i;
    }
    partial_sums[id] = total;
    return NULL;
}

int main(void) {
    // Timing variables
    clock_t t0, t1;
    double secs;

    // Thread handles and IDs
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Allocate array for partial sums
    partial_sums = malloc(sizeof(unsigned long long) * NUM_THREADS);
    if (!partial_sums) {
        perror("malloc");
        return 1;
    }

    t0 = clock(); // Start timing

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_sum, &ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Add up all partial sums
    unsigned long long result = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        result += partial_sums[i];
    }

    t1 = clock(); // End timing
    secs = (double)(t1 - t0) / CLOCKS_PER_SEC;

    // Print result and timing
    printf("Sum = %llu\n", result);
    printf("Time taken: %f seconds\n", secs);

    free(partial_sums);
    return 0;
}
