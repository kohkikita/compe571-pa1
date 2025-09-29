#include <stdio.h>
#include <time.h>
#include "config.h"

static long long sum(void) {
    long long total = 0;
    for (long long i = 1; i <= N; i++) {
        total += i;
    }
    return total;
}

int main(void) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();                 // Start timing
    long long result = sum();        // <-- 64-bit result
    end = clock();                   // End timing

    cpu_time_used = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Sum = %lld\n", result);  // <-- print as long long
    printf("Time taken: %f seconds\n", cpu_time_used);

    return 0;
}