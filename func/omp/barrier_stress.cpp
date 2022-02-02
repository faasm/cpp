#include <cstdio>
#include <omp.h>
#include <string>

int main(int argc, char* argv[])
{
    int nThreads = 2;
    int nIterations = 1000;
    int counters[100];

    if (argc > 1) {
        nThreads = std::stoi(argv[1]);
    }

    bool failed = false;

#pragma omp parallel num_threads(nThreads) default(none)                       \
  shared(nThreads, nIterations, counters, failed)
    {
        int threadNum = omp_get_thread_num();
        counters[threadNum] = 0;

        for (int i = 0; i < nIterations; i++) {
            // Drop out if things have failed
            if (failed) {
                break;
            }

            // Update counter for this thread
            counters[threadNum]++;

#pragma omp barrier
            // In main thread, check all others have hit the barrier
            if (threadNum == 0) {
                for (int t = 0; t < nThreads; t++) {
                    if (counters[t] != i + 1) {
                        printf(
                          "Barrier stress failed. i=%i, counter %i is %i\n",
                          i,
                          t,
                          counters[t]);
                        failed = true;
                    }
                }
            }
#pragma omp barrier
        }
    }

    if (failed) {
        printf("Barrier stress test failed over %i threads\n", nThreads);
        return 1;
    } else {
        printf("Barrier stress test succeeded over %i threads\n", nThreads);
        return 0;
    }
}
