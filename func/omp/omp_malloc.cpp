#include <cstdio>
#include <omp.h>

#define ITERATIONS 100000
#define N_THREADS 20

int main(int argc, char** argv)
{
    long result = 0;

    uint8_t* ptrs[ITERATIONS];

#pragma omp parallel num_threads(N_THREADS) default(none) shared(ptrs)
    {
#pragma omp for
        for (int i = 0; i < ITERATIONS; i++) {
            size_t mallocSize = (i % 5) * 1024;
            auto mem = (uint8_t*)malloc(mallocSize);

            ptrs[i] = mem;
        }
    }

#pragma omp parallel num_threads(N_THREADS) default(none) shared(ptrs)
    {
#pragma omp for
        for (int i = 0; i < ITERATIONS; i++) {
            free(ptrs[i]);
        }
    }

#pragma omp parallel num_threads(N_THREADS) default(none) shared(ptrs)
    {
#pragma omp for
        for (int i = 0; i < ITERATIONS; i++) {
            size_t mallocSize = (i % 6) * 1024;
            auto mem = (uint8_t*)malloc(mallocSize);

            ptrs[i] = mem;
        }
    }

    printf("Successful multi-threaded memory test\n");
    return 0;
}
