#include <cstdio>
#include <omp.h>

#include <faasm/shared_mem.h>

int main(int argc, char* argv[])
{
    int count = 0;

    // Note: Loop will be divided evenly among the threads
    int chunkSize = 20;
    int nThreads = 5;
    int loopSize = nThreads * chunkSize;

    FAASM_SHARED_VAR(loopSize, FAASM_TYPE_INT)
    FAASM_REDUCE(count, FAASM_TYPE_INT, FAASM_OP_SUM)

#pragma omp parallel for num_threads(5) default(none) shared(loopSize) reduction(+ : count)
    for (int i = 0; i < loopSize; i++) {
        count += (omp_get_thread_num() + 1);
    }

    int expected = 0;
    for (int i = 0; i < nThreads; i++) {
        expected += (i + 1) * chunkSize;
    }

    if (count != expected) {
        printf("Failed reduce: expected %d, got %d\n", expected, count);
        return 1;
    } else {
        printf("Reduce as expected: %d\n", count);
        return 0;
    }
}
