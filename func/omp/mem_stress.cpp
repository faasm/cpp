#include <cstdio>
#include <omp.h>
#include <string>

#define ITERATIONS 100000
#define N_THREADS 20

int main(int argc, char** argv)
{
    int nThreads = 0;
    int summation = 0;
    if (argc == 1) {
        nThreads = 2;
    } else {
        nThreads = std::stoi(argv[1]);
    }

    printf("Running mem stress test with %i threads\n", nThreads);
    uint8_t* ptrs[ITERATIONS];

#pragma omp parallel for num_threads(nThreads) default(none)                   \
  shared(ptrs, summation)
    for (int i = 0; i < ITERATIONS; i++) {
        summation++;
        size_t mallocSize = (i % 5) * 1024;
        auto mem = (uint8_t*)::malloc(mallocSize);

        ptrs[i] = mem;
    }
    printf("Memory allocated by %i threads\n", nThreads);

#pragma omp parallel for num_threads(nThreads) default(none)                   \
  shared(ptrs, summation)
    for (int i = 0; i < ITERATIONS; i++) {
        summation++;
        ::free(ptrs[i]);
    }
    printf("Memory freed by %i threads\n", nThreads);

#pragma omp parallel for num_threads(nThreads) default(none)                   \
  shared(ptrs, summation)
    for (int i = 0; i < ITERATIONS; i++) {
        summation++;
        size_t mallocSize = (i % 6) * 1024;
        auto mem = (uint8_t*)::malloc(mallocSize);

        ptrs[i] = mem;
    }

    printf("Successful multi-threaded memory test. %i loops.\n", summation);
    return 0;
}
