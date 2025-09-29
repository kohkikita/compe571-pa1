#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      
#include <sys/types.h>   
#include <sys/wait.h>    
#include <time.h>
#include "config.h"

// calculate sum from lo to hi
long long sum_range(long long lo, long long hi) { 
    long long total = 0;
    long long x;
    for (x = lo; x <= hi; x++) {
        total = total + x;
    }
    return total;
}

int main(void) {
    int pipes[NUM_TASKS][2];
    pid_t pids[NUM_TASKS];
    long long total_sum = 0;
    clock_t start, end;
    double cpu_time_used;
    int i;
    int k;

    // Start timing
    start = clock();

    // create pipes
    for (i = 0; i < NUM_TASKS; i++) {
        int result = pipe(pipes[i]);
        if (result == -1) { 
            perror("pipe"); 
            exit(1); 
        }
    }

    // fork children
    long long base = N / NUM_TASKS;
    for (i = 0; i < NUM_TASKS; i++) {
        pid_t pid = fork();
        pids[i] = pid;
        
        if (pid < 0) { 
            perror("fork"); 
            exit(1); 
        }

        if (pid == 0) {
            // child process

            // close pipes we dont need
            for (k = 0; k < NUM_TASKS; k++) {
                if (k == i) {
                    close(pipes[k][0]); 
                } else {
                    close(pipes[k][0]);
                    close(pipes[k][1]);
                }
            }

            // redirect output to pipe
            close(1);          
            int dup_result = dup(pipes[i][1]);
            if (dup_result == -1) {    
                perror("dup"); 
                _exit(1);
            }
            close(pipes[i][1]);         

            // calculate range for this child
            long long lo = i * base + 1;
            long long hi;
            if (i == NUM_TASKS - 1) {
                hi = N;
            } else {
                hi = (i + 1) * base;
            }

            long long partial = sum_range(lo, hi);

            // send result through pipe
            printf("%lld\n", partial);
            fflush(stdout);
            _exit(0);
        }
    }

    // close write ends of pipes in parent
    for (i = 0; i < NUM_TASKS; i++) {
        close(pipes[i][1]);
    }

    // read results from children
    for (i = 0; i < NUM_TASKS; i++) {
        long long partial = 0;
        FILE *fp = fdopen(pipes[i][0], "r");
        if (fp == NULL) { 
            perror("fdopen"); 
            exit(1); 
        }
        int scan_result = fscanf(fp, "%lld", &partial);
        if (scan_result != 1) {
            fprintf(stderr, "read failed for child %d\n", i);
        } else {
            total_sum = total_sum + partial;
        }
        fclose(fp);
    }

    // wait for all children
    for (i = 0; i < NUM_TASKS; i++) {
        int wait_result = waitpid(pids[i], NULL, 0);
        if (wait_result == -1) {
            perror("waitpid");
        }
    }

    // End timing
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Total sum (1..%lld) = %lld\n", (long long)N, total_sum);
    printf("Time taken: %f seconds\n", cpu_time_used);

    return 0;
}