#include <pthread.h>
#include <stdio.h>

/**
 * This function uses a thread that modifies a value on the stack
 * of the main thread, hence will only work when all threads have access to the
 * same shared memory.
 */

struct threadArgs
{
    int idx;
    int* ptr;
};

void* threadFunc(void* arg)
{
    // Write to the struct passed in
    auto args = (threadArgs*)arg;
    *args->ptr = 200 + args->idx;
    return nullptr;
}

int main()
{
    // Spawn a couple of threads
    int nThreads = 4;
    int x[nThreads];
    pthread_t threads[nThreads];
    struct threadArgs args[nThreads];

    for (int i = 0; i < nThreads; i++) {
        args[i].idx = i;
        args[i].ptr = &x[i];

        int ret = pthread_create(&threads[i], NULL, threadFunc, &args[i]);
        if (ret != 0) {
            fprintf(stderr, "Error creating thread (%i)\n", ret);
            return 1;
        }
    }

    // Join the threads
    for (int i = 0; i < nThreads; i++) {
        if (pthread_join(threads[i], nullptr)) {
            fprintf(stderr, "Error joining thread\n");
            return 1;
        }

        // Check stack variable
        int expected = 200 + i;
        if (x[i] != expected) {
            printf("Thread invocation failed (expected %i but got %i)\n",
                   expected,
                   x[i]);
            return 1;
        }
    }

    return 0;
}
