
#define _DARWIN_C_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <errno.h>
#include <string.h>

typedef struct {
    unsigned long long start;   
    unsigned long long end;     
    __uint128_t partial;        
    int demo;
    int use_return_ptr;
} Task;

static void u128_to_str(__uint128_t x, char buf[40]) {
    char tmp[40]; int i = 0;
    if (!x) { buf[0] = '0'; buf[1] = '\0'; return; }
    while (x) { tmp[i++] = '0' + (x % 10); x /= 10; }
    for (int j = 0; j < i; ++j) buf[j] = tmp[i-1-j];
    buf[i] = '\0';
}

static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);  
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void *worker(void *arg) {
    Task *t = (Task *)arg;
    if (t->demo) {
        printf("[thread %llu] range=[%llu,%llu)\n",
               (unsigned long long)pthread_self(), t->start, t->end);
    }
    __uint128_t s = 0;
    for (unsigned long long i = t->start; i < t->end; ++i) s += i;

    if (t->use_return_ptr) {
        __uint128_t *ret = (__uint128_t *)malloc(sizeof(__uint128_t));
        if (!ret) { perror("malloc"); pthread_exit(NULL); }
        *ret = s;
        pthread_exit(ret);
    } else {
        t->partial = s;
        return NULL;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr,"usage: %s N NUM_THREADS [--retptr] [--demo] [--check]\n", argv[0]);
        return 1;
    }
    unsigned long long N = strtoull(argv[1], NULL, 10);
    unsigned long long T = strtoull(argv[2], NULL, 10);
    if (T == 0) { fprintf(stderr, "NUM_THREADS must be >= 1\n"); return 1; }

    int opt_demo = 0, opt_check = 0, opt_retptr = 0;
    for (int i = 3; i < argc; ++i) {
        if (!strcmp(argv[i], "--demo"))   opt_demo = 1;
        else if (!strcmp(argv[i], "--check"))  opt_check = 1;
        else if (!strcmp(argv[i], "--retptr")) opt_retptr = 1;
        else fprintf(stderr, "warning: unknown flag '%s'\n", argv[i]);
    }

    pthread_t *th = (pthread_t*)malloc(sizeof(pthread_t)*T);
    Task *tasks   = (Task*)malloc(sizeof(Task)*T);
    if (!th || !tasks) { perror("malloc"); return 1; }

    // Evenly split [0, N), handle remainder if N % T != 0 (robustness)
    unsigned long long base = N / T, rem = N % T, cur = 0;
    for (unsigned long long k = 0; k < T; ++k) {
        unsigned long long len = base + (k < rem ? 1ULL : 0ULL);
        tasks[k].start = cur;
        tasks[k].end   = cur + len;
        tasks[k].partial = 0;
        tasks[k].demo = opt_demo;
        tasks[k].use_return_ptr = opt_retptr;
        cur += len;
    }

    double t0 = now_seconds();

    for (unsigned long long k = 0; k < T; ++k) {
        int rc = pthread_create(&th[k], NULL, worker, &tasks[k]);
        if (rc) { errno = rc; perror("pthread_create"); return 2; }
    }

    __uint128_t sum = 0;
    if (opt_retptr) {
        for (unsigned long long k = 0; k < T; ++k) {
            void *ret = NULL;
            int rc = pthread_join(th[k], &ret);
            if (rc) { errno = rc; perror("pthread_join"); return 3; }
            if (!ret) { fprintf(stderr, "thread %llu returned NULL\n", k); continue; }
            sum += *(__uint128_t *)ret;
            free(ret);
        }
    } else {
        for (unsigned long long k = 0; k < T; ++k) {
            int rc = pthread_join(th[k], NULL);
            if (rc) { errno = rc; perror("pthread_join"); return 3; }
            sum += tasks[k].partial;
        }
    }

    double t1 = now_seconds();

    if (opt_check) {
        __uint128_t expected = ((__uint128_t)N * ((__uint128_t)N - 1)) / 2;
        if (sum != expected) {
            char got[40], exp[40]; u128_to_str(sum, got); u128_to_str(expected, exp);
            fprintf(stderr, "ERROR: sum mismatch!\n  got=%s\n  exp=%s\n", got, exp);
        }
    }

    char buf[40]; u128_to_str(sum, buf);
    printf("N=%llu, NUM_THREADS=%llu\n", N, T);
    printf("Sum=%s\n", buf);
    printf("Time(s)=%.6f\n", t1 - t0);

    free(th); free(tasks);
    return 0;
}
