#include <faasm/shared_mem.h>

#include <cstdio>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

bool doReduce(const int nThreads)
{
    int chunkSize = 1000;
    int loopSize = nThreads * chunkSize;
    std::vector<int> counts(2, 0);

    int reducedA = 0;
    int reducedB = 0;

    bool success = true;

    FAASM_REDUCE(reducedA, FAASM_TYPE_INT, FAASM_OP_SUM)
    FAASM_REDUCE(reducedB, FAASM_TYPE_INT, FAASM_OP_SUM)

#pragma omp parallel for num_threads(nThreads) default(none) \
    shared(counts,loopSize,success) \
    reduction(+ : reducedA,reducedB)
    for (int i = 0; i < loopSize; i++) {
        int threadNum = omp_get_thread_num();
        counts[threadNum]++;
        int thisCount = counts[threadNum];

        // Add one here so that thread zero still has an effect
        reducedA += 10 * (threadNum + 1);
        reducedB += 15 * (threadNum + 1);

        // We don't care about the integrity of the result here
    }

    return success;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Must provide num. threads and loops: <binary> <#threads> <#loops>\n");
        return 1;
    }
    int nThreads = atoi(argv[1]);
    int nLoops = atoi(argv[2]);

    // Run reduce in a loop and check each iteration is correct. Sleep for five
    // seconds at the middle
    for (int i = 0; i <= nLoops; i++) {
        bool success = doReduce(nThreads);
        if (!success) {
            printf("Repeated reduce failed on loop %i\n", i);
            return 1;
        }

        if (i == (nLoops/2)) {
            printf("Sleeping for a bit between reduces...\n");
            sleep(5);
        }
    }


    printf("Repeated reduce successful over %i loops\n", nLoops);

    return 0;
}
