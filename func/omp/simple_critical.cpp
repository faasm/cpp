#include <cstdio>
#include <omp.h>

int main(int argc, char* argv[])
{
    const int expected = 1;
    bool failed = false;

    // Variable only modified in critical section
    int criticalVar = expected;

    int randomValue = 2;

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
            // picked up if the critical isn't working.
            criticalVar = 2;
            for (int j = 0; j < 10000; j++) {
                criticalVar = (criticalVar + randomValue) % 4 + 10;
            }

            // Sets back to original value
            criticalVar = expected;
        }

        randomValue = (omp_get_thread_num() + 1 * randomValue) % 7;
    }

    if (failed || criticalVar != expected) {
        printf("Critical section error, assumption failed: %d\n", criticalVar);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
