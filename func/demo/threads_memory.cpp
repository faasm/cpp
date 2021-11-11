#include <faasm/compare.h>

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <vector>

#define N_THREADS 2
#define WASM_PAGE_SIZE 65536
#define DATA_SIZE 20

// Global array
bool successFlags[N_THREADS];

struct ThreadArgs
{
    int threadIdx;
    uint8_t* basePtr;
};

void* threadFunc(void* arg)
{
    auto* threadArgs = (struct ThreadArgs*)arg;

    int idx = threadArgs->threadIdx;
    printf("Thread memory check %i\n", idx);

    // Write a value to the global shared array
    successFlags[idx] = true;

    // Write to the mmapped shared memory
    int offset = idx * WASM_PAGE_SIZE;
    uint8_t* target = threadArgs->basePtr + offset;
    std::vector<uint8_t> data(DATA_SIZE, idx);
    ::memcpy(target, data.data(), data.size());

    return nullptr;
}

int main(int argc, char* argv[])
{
    pthread_t threads[N_THREADS];
    ThreadArgs inputs[N_THREADS];

    // Shared memory
    size_t memSize = 100 * WASM_PAGE_SIZE;
    uint8_t* sharedMem = (uint8_t*)mmap(
      nullptr, memSize, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Spawn the threads
    for (int i = 0; i < N_THREADS; i++) {
        // Initialise the global array
        successFlags[i] = false;

        inputs[i].threadIdx = i;
        inputs[i].basePtr = sharedMem;
        int ret = pthread_create(&threads[i], nullptr, threadFunc, &inputs[i]);

        if (ret != 0) {
            printf("Error creating thread (%i)\n", ret);
            return 1;
        }
    }

    // Join the threads
    for (int i = 0; i < N_THREADS; i++) {
        if (pthread_join(threads[i], nullptr)) {
            printf("Error joining thread\n");
            return 1;
        }
    }

    // Check changes have been applied
    for (int i = 0; i < N_THREADS; i++) {
        if (!successFlags[i]) {
            printf("Global success flag not set for %d\n", i);
            return 1;
        }

        int offset = i * WASM_PAGE_SIZE;
        uint8_t* target = sharedMem + offset;

        std::vector<uint8_t> expected(DATA_SIZE, i);

        if (!faasm::compareArrays<uint8_t>(
              expected.data(), target, DATA_SIZE)) {
            return 1;
        }
    }

    return 0;
}
