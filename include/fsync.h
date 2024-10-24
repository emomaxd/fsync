#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>

#include <stdatomic.h>
#include <sched.h>
#include <stdio.h>

typedef struct {
    atomic_flag flag;
} fs_mutex_t;

void fs_mutex_init(fs_mutex_t *mutex) {
    atomic_flag_clear(&mutex->flag);
}

void fs_mutex_lock(fs_mutex_t *mutex) {
    while (atomic_flag_test_and_set_explicit(&mutex->flag, memory_order_acquire)) {
        // Use exponential backoff strategy
        for (int i = 0; i < 10; ++i) {
            sched_yield(); // Yield CPU
        }
    }
}

void fs_mutex_unlock(fs_mutex_t *mutex) {
    atomic_flag_clear_explicit(&mutex->flag, memory_order_release);
}

typedef struct {
    atomic_int waiting_count;
    atomic_int signaled_count;
} fs_cond_t;


void fs_cond_init(fs_cond_t *cond) {
    atomic_store(&cond->waiting_count, 0);
    atomic_store(&cond->signaled_count, 0);
}


void fs_cond_wait(fs_cond_t *cond, fs_mutex_t *mutex) {
    atomic_fetch_add(&cond->waiting_count, 1);

    fs_mutex_unlock(mutex);

    while (1) {
        if (atomic_load(&cond->signaled_count) > 0) {
            atomic_fetch_sub(&cond->signaled_count, 1);
            atomic_fetch_sub(&cond->waiting_count, 1);
            break;
        }
        sched_yield();
    }

    fs_mutex_lock(mutex);
}


void fs_cond_signal(fs_cond_t *cond) {
    if (atomic_load(&cond->waiting_count) > 0) {
        atomic_fetch_add(&cond->signaled_count, 1);
    }
}

void fs_cond_broadcast(fs_cond_t *cond) {
    // Signal all waiting threads
    atomic_store(&cond->signaled_count, atomic_load(&cond->waiting_count));
}

void fs_cond_destroy(fs_cond_t *cond) {
    // No resources to clean up in this simple implementation
}

void fs_mutex_destroy(fs_mutex_t *mutex) {
    // No resources to clean up in this simple implementation
}


typedef struct {
    fs_cond_t cond;
    fs_mutex_t mutex;
    int count;
} fs_semaphore_t;

void fs_semaphore_init(fs_semaphore_t *sem, int initial_count) {
    fs_cond_init(&sem->cond); // Initialize the condition variable
    fs_mutex_init(&sem->mutex); // Initialize the mutex
    atomic_store(&sem->count, initial_count); // Initialize the count atomically
}


void fs_semaphore_wait(fs_semaphore_t *sem) {
    fs_mutex_lock(&sem->mutex); // Lock the mutex

    while (atomic_load(&sem->count) <= 0) { // Check if count is zero or negative
        fs_cond_wait(&sem->cond, &sem->mutex); // Wait and release the mutex
    }

    atomic_fetch_sub(&sem->count, 1); // Decrement the count atomically
    fs_mutex_unlock(&sem->mutex); // Unlock the mutex
}


void fs_semaphore_signal(fs_semaphore_t *sem) {
    fs_mutex_lock(&sem->mutex); // Lock the mutex
    atomic_fetch_add(&sem->count, 1); // Increment the count atomically
    fs_cond_signal(&sem->cond); // Signal the condition variable
    fs_mutex_unlock(&sem->mutex); // Unlock the mutex
}
