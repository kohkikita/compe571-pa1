#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    unsigned long long start, end;
    unsigned long long partial;
} Task;

void *worker(void *arg) {
    Task *t = arg;
    unsigned long long s = 0;
    for (unsigned long long i = t->start; i < t->end; i++) s += i;
    t->partial = s;
    return NULL;
}

double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec*1e-9;
}

int main(int argc, char *argv[]) {
    if (argc < 3) { printf("usage: %s N NUM_THREADS\n", argv[0]); return 1; }
    unsigned long long N = strtoull(argv[1], NULL, 10);
    int T = atoi(argv[2]);

    pthread_t th[T]; Task tasks[T];
    unsigned long long base = N / T, cur = 0;

    for (int k = 0; k < T; k++) {
        tasks[k].start = cur;
        tasks[k].end   = cur + base;
        tasks[k].partial = 0;
        cur += base;
    }

    double t0 = now_s();
    for (int k = 0; k < T; k++) pthread_create(&th[k], NULL, worker, &tasks[k]);
    for (int k = 0; k < T; k++) pthread_join(th[k], NULL);

    unsigned long long sum = 0;
    for (int k = 0; k < T; k++) sum += tasks[k].partial;
    double t1 = now_s();

    printf("N=%llu, threads=%d\n", N, T);
    printf("sum=%llu\n", sum);
    printf("time=%.6f s\n", t1 - t0);
    return 0;
}
