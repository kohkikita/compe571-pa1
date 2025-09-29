# COMPE571 Programming Assignment 1 - README

## Files Description

- **baseline.c** - Case 1: Sequential implementation (no parallelism)
- **multithread.c** - Case 2: Multithreading implementation using pthreads
- **multitask.c** - Case 3: Multitasking implementation using fork() and pipes (Option 1)
- **config.h** - Configuration file with N, NUM_THREADS, and NUM_TASKS values

---

## How to Compile and Run

### Baseline
```bash
gcc -std=c11 -O2 baseline.c -o baseline
./baseline
```

### Multithread
```bash
gcc -std=c11 -O2 multithread.c -o multithread -lpthread
./multithread
```

### Multitask
```bash
gcc -std=c11 -O2 multitask.c -o multitask
./multitask
```

---

## Configuration

Edit `config.h` to change test parameters:
- N = 100000000, 1000000000, or 10000000000
- NUM_THREADS = 2, 4, or 8
- NUM_TASKS = 2, 4, or 8

After editing config.h, recompile the programs.