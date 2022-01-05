#include <cstdio>
#include <omp.h>

#include <faasm/shared_mem.h>

int main()
{
    int nThreads = 100;
    int counter = 20;

    FAASM_REDUCE(counter, FAASM_TYPE_INT, FAASM_OP_SUM)
#pragma omp parallel num_threads(nThreads) default(none) shared(counter)
    {
        // NOTE - it appears OpenMP doesn't actually support compiling the
        // atomic pragma to anything useful in wasm, so we have to replace with
        // our own custom stuff
        FAASM_ATOMIC_INCR_BY(counter, omp_get_thread_num());
    }

    int expected = 20;
    for (int i = 0; i < nThreads; i++) {
        expected += i;
    }

    if (counter != expected) {
        printf("Atomic check failed %i != %i\n", counter, expected);
        return 1;
    }

    printf("Atomic check succeeded\n");

    return 0;
}
