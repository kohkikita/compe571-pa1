#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // fork, pipe, dup, close, read, write
#include <sys/types.h>   // pid_t
#include <sys/wait.h>    // waitpid
#include "config.h"

long long compute_partial_sum(long long start, long long end) {
    long long total = 0;
    for (long long i = start; i <= end; i++) {
        total += i;
    }
    return total;
}

int main(void) {
    pid_t pids[NUM_TASKS];
    int pipes[NUM_TASKS][2];
    long long range = N / NUM_TASKS;
    long long start, end;
    long long total_sum = 0;

    // Create pipes and fork processes
    for (int i = 0; i < NUM_TASKS; i++) {
        if (pipe(pipes[i]) ==  -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) { // Child process
            close(pipes[i][0]); // Close read end

            start = i * range + 1;
            end = (i == NUM_TASKS - 1) ? N : (i + 1) * range;

            long long partial_sum = compute_partial_sum(start, end);

            write(pipes[i][1], &partial_sum, sizeof(partial_sum));
            close(pipes[i][1]); // Close write end
            exit(0);
        } else { // Parent process
            close(pipes[i][1]); // Close write end
        }
    }

    // Collect results from child processes
    for (int i = 0; i < NUM_TASKS; i++) {
        long long partial_sum;
        read(pipes[i][0], &partial_sum, sizeof(partial_sum));
        close(pipes[i][0]); // Close read end

        total_sum += partial_sum;
        waitpid(pids[i], NULL, 0); // Wait for child to finish
    }

    printf("Total Sum = %lld\n", total_sum);
    return 0;
}