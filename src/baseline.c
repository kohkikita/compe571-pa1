#include <stdio.h>
#include <time.h>
#include "config.h"

int sum(void) {
    int total = 0;
    for (int i = 1; i <= N; i++) {
        total += i;
    }
    return total;
}

int main(void) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();          // Start timing
    int result = sum();       // Call the function
    end = clock();            // End timing

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Sum = %d\n", result);
    printf("Time taken: %f seconds\n", cpu_time_used);

    return 0;
}
