#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>  // For sleep
#include <inttypes.h> // Include this for PRIu64
#include <string.h>   // Include this for strlen
#include "fsync.h"

#define NUM_THREADS 8
#define NUM_ITERATIONS 100000

// POSIX Mutex and Condition Variable
pthread_mutex_t posix_mutex;
pthread_cond_t posix_cond;

// Custom Mutex, Condition Variable, and Semaphore
fs_mutex_t my_mutex;
fs_cond_t my_cond;
fs_semaphore_t my_semaphore;

// Function to get CPU cycles
static inline uint64_t rdtsc() {
    unsigned int lo, hi;
    asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

// Function to format large numbers with commas
void format_cycles(uint64_t cycles) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%" PRIu64, cycles);
    
    // Print with commas
    int len = strlen(buffer);
    for (int i = len; i > 0; i--) {
        putchar(buffer[i - 1]);
        if ((len - i) % 3 == 2 && i > 1) {
            putchar(',');
        }
    }
    putchar('\n');
}

// Benchmarking function for POSIX Mutex
void *posix_mutex_benchmark(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&posix_mutex);
        pthread_mutex_unlock(&posix_mutex);
    }
    return NULL;
}

// Benchmarking function for Custom Mutex
void *my_mutex_benchmark(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        fs_mutex_lock(&my_mutex);
        fs_mutex_unlock(&my_mutex);
    }
    return NULL;
}

// Benchmarking function for POSIX Condition Variable
void *posix_cv_benchmark(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_mutex_lock(&posix_mutex);
        pthread_cond_signal(&posix_cond);
        pthread_mutex_unlock(&posix_mutex);
    }
    return NULL;
}

// Benchmarking function for Custom Condition Variable
void *my_cv_benchmark(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        fs_cond_signal(&my_cond);
    }
    return NULL;
}

// Benchmarking function for POSIX Semaphore
void *posix_semaphore_benchmark(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        sem_post((sem_t *)arg);
    }
    return NULL;
}

// Benchmarking function for Custom Semaphore
void *my_semaphore_benchmark(void *arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        fs_semaphore_signal((fs_semaphore_t *)arg);
    }
    return NULL;
}

// Function to run benchmarks and calculate performance
void run_benchmarks() {
    pthread_t threads[NUM_THREADS];

    // Initialize POSIX Mutex and Condition Variable
    pthread_mutex_init(&posix_mutex, NULL);
    pthread_cond_init(&posix_cond, NULL);
    
    // Initialize Custom Mutex and Condition Variable
    fs_cond_init(&my_cond);
    fs_mutex_init(&my_mutex);
    fs_semaphore_init(&my_semaphore, 0);  // Initialize semaphore with an initial count

    // Benchmark POSIX Mutex
    clock_t start_time;
    uint64_t start_cycles, end_cycles;

    start_time = clock();
    start_cycles = rdtsc();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, posix_mutex_benchmark, NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    end_cycles = rdtsc();
    clock_t end_time = clock();
    double posix_mutex_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("POSIX Mutex benchmark time: %f seconds\n", posix_mutex_time);
    printf("POSIX Mutex benchmark cycles: ");
    format_cycles(end_cycles - start_cycles);

    // Benchmark Custom Mutex
    start_time = clock();
    start_cycles = rdtsc();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, my_mutex_benchmark, NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    end_cycles = rdtsc();
    end_time = clock();
    double my_mutex_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Custom Mutex benchmark time: %f seconds\n", my_mutex_time);
    printf("Custom Mutex benchmark cycles: ");
    format_cycles(end_cycles - start_cycles);

    // Percentage comparison for Mutex
    double mutex_percentage = (my_mutex_time / posix_mutex_time) * 100;
    printf("Custom Mutex is %.2f%% of POSIX Mutex time.\n\n", mutex_percentage);

    // Benchmark POSIX Condition Variable
    start_time = clock();
    start_cycles = rdtsc();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, posix_cv_benchmark, NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    end_cycles = rdtsc();
    end_time = clock();
    double posix_cv_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("POSIX Condition Variable benchmark time: %f seconds\n", posix_cv_time);
    printf("POSIX Condition Variable benchmark cycles: ");
    format_cycles(end_cycles - start_cycles);

    // Benchmark Custom Condition Variable
    start_time = clock();
    start_cycles = rdtsc();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, my_cv_benchmark, NULL);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    end_cycles = rdtsc();
    end_time = clock();
    double my_cv_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Custom Condition Variable benchmark time: %f seconds\n", my_cv_time);
    printf("Custom Condition Variable benchmark cycles: ");
    format_cycles(end_cycles - start_cycles);

    // Percentage comparison for Condition Variables
    double cv_percentage = (my_cv_time / posix_cv_time) * 100;
    printf("Custom Condition Variable is %.2f%% of POSIX Condition Variable time.\n\n", cv_percentage);

    // Benchmark POSIX Semaphore
    start_time = clock();
    start_cycles = rdtsc();
    
    sem_t posix_semaphore;
    sem_init(&posix_semaphore, 0, 0);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, posix_semaphore_benchmark, (void *)&posix_semaphore);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    end_cycles = rdtsc();
    end_time = clock();
    double posix_semaphore_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("POSIX Semaphore benchmark time: %f seconds\n", posix_semaphore_time);
    printf("POSIX Semaphore benchmark cycles: ");
    format_cycles(end_cycles - start_cycles);

    // Benchmark Custom Semaphore
    start_time = clock();
    start_cycles = rdtsc();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, my_semaphore_benchmark, (void *)&my_semaphore);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    end_cycles = rdtsc();
    end_time = clock();
    double my_semaphore_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Custom Semaphore benchmark time: %f seconds\n", my_semaphore_time);
    printf("Custom Semaphore benchmark cycles: ");
    format_cycles(end_cycles - start_cycles);

    // Percentage comparison for Semaphores
    double semaphore_percentage = (my_semaphore_time / posix_semaphore_time) * 100;
    printf("Custom Semaphore is %.2f%% of POSIX Semaphore time.\n\n", semaphore_percentage);

    // Cleanup
    pthread_mutex_destroy(&posix_mutex);
    pthread_cond_destroy(&posix_cond);
    sem_destroy(&posix_semaphore);
}

int main() {
    run_benchmarks();
    return 0;
}
