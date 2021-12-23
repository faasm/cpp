#include <cstdio>
#include <omp.h>

#ifdef __wasm
#include <faasm/shared_mem.h>
#else
#define FAASM_REDUCE(...) (void)0;
#define FAASM_DEFAULT_SHARED(...) (void)0;
#endif

int main(int argc, char* argv[])
{
    int count = 0;

    // Note: Loop will be divided evenly among the threads
    int nThreads = 5;
    int chunkSize = 20;
    int nLoops = nThreads * chunkSize;
    int flagsStack[5];
    int* flagsHeap = new int[nThreads];

    FAASM_REDUCE(count, FAASM_TYPE_INT, FAASM_OP_SUM)
    FAASM_DEFAULT_SHARED()

#pragma omp parallel for num_threads(5) \
    default(none) \
    shared(nLoops, flagsStack, flagsHeap) \
    reduction(+ : count)
    for (int i = 0; i < nLoops; i++) {
        int threadNum = omp_get_thread_num();
        count += (threadNum + 1);

        flagsStack[threadNum] += (threadNum + 1);
        flagsHeap[threadNum] += (threadNum + 1);
    }

    bool success = true;
    int expected = 0;
    for (int i = 0; i < nThreads; i++) {
        // Build up expected sum
        expected += (i + 1) * chunkSize;

        // Check flag
        int expectedFlag = (i + 1) * chunkSize;

        if (flagsStack[i] != expectedFlag) {
            printf("%ith stack flag not as expected:  %i != %i\n",
                   i,
                   flagsStack[i],
                   expectedFlag);
            success = false;
        }

        if (flagsHeap[i] != expectedFlag) {
            printf("%ith heap flag not as expected:  %i != %i\n",
                   i,
                   flagsStack[i],
                   expectedFlag);
            success = false;
        }
    }

    if (count != expected) {
        printf("Count not as expected: %i != %i\n", count, expected);
        success = false;
    }

    delete[] flagsHeap;

    if (!success) {
        return 1;
    }

    return 0;
}
