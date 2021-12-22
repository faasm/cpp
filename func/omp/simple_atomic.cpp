#include <cstdio>
#include <omp.h>

#include <faasm/shared_mem.h>

int main()
{
    int nThreads = 100;
    int counter = 0;

    FAASM_SHARED_VAR(counter, FAASM_TYPE_INT)

#pragma omp parallel num_threads(nThreads) default(none) shared(counter)
    {
        // NOTE - it appears OpenMP doesn't actually support compiling the
        // atomic pragma to anything useful in wasm, so we have to implement it
        // with a critical instead
#pragma omp critical
        {
            counter += omp_get_thread_num();
        }
    }

    int expected = 0;
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
