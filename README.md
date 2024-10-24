# Overview
fast-sync, is a header-only sync library that aims to implement sync mechanisms as wait-free and blazingly fast!

# Embedding into your existing codebase
Put include/fsync.h into your codebase's include directory.

# Building and running (only to see benchmark result)
```bash
mkdir build
cd build
cmake ..
cmake --build . # or make
./fsync
```

# Details
Common sync structs have been implemented by atomic operations.

# Comparision
In the benchmarks/ folder you can see the demo code and the results against POSIX sync mechanisms.

# Mutex usage
```c
fs_mutex_t mutex;
fs_mutex_init(&mutex); // Initialize the mutex

fs_mutex_lock(&mutex); // Acquire the mutex

// Critical Section: Access shared resources here
// Example: Increment a shared counter
shared_counter++;

// Unlock the mutex to allow other threads to access the critical section
fs_mutex_unlock(&mutex);
```



# Condition variable usage
```c
fs_mutex_t mutex;
fs_cond_t cond;

fs_mutex_init(&mutex);     // Initialize the mutex
fs_cond_init(&cond);       // Initialize the condition variable

// Thread A: Waiting on the condition
void thread_a() {
    fs_mutex_lock(&mutex);  // Lock the mutex before waiting
    // Perform some work
    fs_cond_wait(&cond, &mutex); // Wait for condition
    // Continue after being signaled
    fs_mutex_unlock(&mutex); // Unlock the mutex
}

// Thread B: Signaling the condition
void thread_b() {
    fs_mutex_lock(&mutex);  // Lock the mutex before signaling
    // Modify shared resources
    fs_cond_signal(&cond);   // Signal condition
    fs_mutex_unlock(&mutex); // Unlock the mutex
}
```



# Semaphore usage
```c
fs_semaphore_t semaphore;

fs_semaphore_init(&semaphore, 1); // Initialize semaphore with a count of 1

// Thread A
void thread_a() {
    fs_semaphore_wait(&semaphore); // Wait for the semaphore
    // Critical section: perform work
    fs_semaphore_signal(&semaphore); // Signal the semaphore
}

// Thread B
void thread_b() {
    fs_semaphore_wait(&semaphore); // Wait for the semaphore
    // Critical section: perform work
    fs_semaphore_signal(&semaphore); // Signal the semaphore
}
```