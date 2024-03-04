#include <cstdio>
#include <omp.h>
#include <string>

#include <faasm/shared_mem.h>

#define ITERATIONS 100
#define N_THREADS 20
#define MALLOC_BIG 5 * 1024 * 1024
#define MALLOC_SMALL 5 * 1024

int main(int argc, char** argv)
{
    int nThreads = 0;
    int totalLoops = 0;
    int expectedLoops = 3 * ITERATIONS;

    if (argc == 1) {
        nThreads = 50;
    } else {
        nThreads = std::stoi(argv[1]);
    }

    printf("Running mem stress test with %i threads\n", nThreads);
    uint8_t* ptrs[ITERATIONS];
    int counts[ITERATIONS];

    FAASM_REDUCE(totalLoops, FAASM_TYPE_INT, FAASM_OP_SUM)

#pragma omp parallel for num_threads(nThreads) default(none)                   \
  shared(ptrs, counts) reduction(+ : totalLoops)
    for (int i = 0; i < ITERATIONS; i++) {
        totalLoops++;

        size_t mallocSize;
        if (i % 2 == 0) {
            mallocSize = MALLOC_BIG;
        } else {
            mallocSize = MALLOC_SMALL;
        }

        auto mem = (uint8_t*)::malloc(mallocSize);

        counts[i]++;
        ptrs[i] = mem;
    }
    printf("Memory allocated by %i threads\n", nThreads);

    FAASM_REDUCE(totalLoops, FAASM_TYPE_INT, FAASM_OP_SUM)

#pragma omp parallel for num_threads(nThreads) default(none)                   \
  shared(ptrs, counts) reduction(+ : totalLoops)
    for (int i = 0; i < ITERATIONS; i++) {
        totalLoops++;
        ::free(ptrs[i]);

        counts[i]++;
    }
    printf("Memory freed by %i threads\n", nThreads);

    FAASM_REDUCE(totalLoops, FAASM_TYPE_INT, FAASM_OP_SUM)

#pragma omp parallel for num_threads(nThreads) default(none)                   \
  shared(ptrs, counts) reduction(+ : totalLoops)
    for (int i = 0; i < ITERATIONS; i++) {
        totalLoops++;
        size_t mallocSize = (i % 6) * 1024;
        auto mem = (uint8_t*)::malloc(mallocSize);

        counts[i]++;
        ptrs[i] = mem;
    }

    // Check counts
    bool failed = false;
    for (int i = 0; i < ITERATIONS; i++) {
        if (counts[i] != 3) {
            printf("counts[%i]=%i\n", i, counts[i]);

            failed = true;
        }
    }

    if (failed) {
        printf("Counts not as expected\n");
        return 1;
    }

    if (totalLoops != expectedLoops) {
        printf("Memory stress test failed, expected %i loops but got %i.\n",
               expectedLoops,
               totalLoops);
        return 1;
    } else {
        printf("Successful multi-threaded memory test. %i loops.\n",
               totalLoops);
        return 0;
    }
}
