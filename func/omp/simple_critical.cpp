#include <cstdio>
#include <omp.h>

#include <faasm/shared_mem.h>

int main(int argc, char* argv[])
{
    const int expected = 1;
    bool failed = false;

    // Variable only modified in critical section
    int criticalVar = expected;

    int randomValue = 2;

    FAASM_SHARED_VAR(randomValue, FAASM_TYPE_INT);
    FAASM_SHARED_VAR(criticalVar, FAASM_TYPE_INT);
    FAASM_SHARED_VAR(expected, FAASM_TYPE_INT);
    FAASM_SHARED_VAR(failed, FAASM_TYPE_BOOL);

#pragma omp parallel for num_threads(4) default(none)                          \
  shared(randomValue, criticalVar, expected, failed)
    for (int i = 0; i < 1000; i++) {
#pragma omp critical
        {
            if (criticalVar != expected) {
                printf("Thread %d critical unexpected %d != %d\n",
                       omp_get_thread_num(),
                       criticalVar,
                       expected);
                failed = true;
            }

            // Do a load of modifications to the critical variable that will be
            // picked up by other threads if the critical isn't working.
            criticalVar = 2;
            for (int j = 0; j < 10000; j++) {
                criticalVar = (criticalVar + randomValue) % 4 + 10;
            }

            randomValue = ((omp_get_thread_num() + 1) * randomValue) % 7;

            // Set back to original value
            criticalVar = expected;
        }
    }

    if (failed || criticalVar != expected) {
        printf("Critical section error, assumption failed: %d\n", criticalVar);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
