#include <faasm/input.h>
#include <faasm/shared_mem.h>

#include <cstdio>
#include <math.h>
#include <omp.h>
#include <unistd.h>
#include <vector>

// This reduce method is called with a varying number of threads, but with
// a maximum of 10. In addition, the inner parallel for pragma may be
// elastically scaled from nThreads, all the way up to 10.
bool doReduce(int numThreads)
{
    int chunkSize = 1000;
    int loopSize = numThreads * chunkSize;
    int maxNumThreads = 10;
    std::vector<int> counts(maxNumThreads, 0);

    int reducedA = 0;
    int reducedB = 0;

    bool success = true;

    FAASM_REDUCE(reducedA, FAASM_TYPE_INT, FAASM_OP_SUM)
    FAASM_REDUCE(reducedB, FAASM_TYPE_INT, FAASM_OP_SUM)

#pragma omp parallel for num_threads(numThreads) default(none)                 \
  shared(counts, loopSize, success) reduction(+ : reducedA, reducedB)
    for (int i = 0; i < loopSize; i++) {
        int threadNum = omp_get_thread_num();
        counts[threadNum]++;
        int thisCount = counts[threadNum];

        // Add one here so that thread zero still has an effect
        reducedA += 10 * (threadNum + 1);
        reducedB += 15 * (threadNum + 1);

        int expectedReduceA = thisCount * 10 * (threadNum + 1);
        if (reducedA != expectedReduceA) {
            printf("Thread %i iteration %i count %i: reducedA %i != %i\n",
                   threadNum,
                   i,
                   thisCount,
                   reducedA,
                   expectedReduceA);

            success = false;
        }
    }

    if (!success) {
        printf("Problem within repeated reduce loop\n");
        return 1;
    }

    // First, work out how many threads actually executed the loop, by checking
    // how many threads wrote to the counts array
    int actualNumThreads = 0;
    for (int i = 0; i < counts.size(); i++) {
        if (counts.at(i) != 0) {
            actualNumThreads++;
        }
    }

    if ((actualNumThreads < numThreads) || (actualNumThreads > maxNumThreads)) {
        printf("Actual number of threads outside valid range: %i \\not \\in "
               "[%i, %i]\n",
               actualNumThreads,
               numThreads,
               maxNumThreads);

        // Exit fast in this case as posterior checks may seg-fault
        return false;
    }

    // Check counts (only count the aggregate, and a uniform distribution, as
    // we may elastically change the parallelism of the loop)
    int actualChunkSize = (int)loopSize / actualNumThreads;
    int total = 0;
    for (int tNum = 0; tNum < actualNumThreads; tNum++) {
        if (counts[tNum] != actualChunkSize) {
            printf("Loop count for thread %i: %i != %i\n",
                   tNum,
                   counts[tNum],
                   actualChunkSize);
            success = false;
        }

        total += counts[tNum];
    }

    if (total != loopSize) {
        printf("Total loop count failed: %i != %i\n", total, loopSize);
        success = false;
    }

    // The expected final value is: constant (10/15) * (sum [1, nThreads]) *
    // chunkSize
    int expectedFinalReducedA =
      (int)10 * actualNumThreads * (actualNumThreads + 1) / 2 * actualChunkSize;
    int expectedFinalReducedB =
      (int)15 * actualNumThreads * (actualNumThreads + 1) / 2 * actualChunkSize;

    if (reducedA != expectedFinalReducedA) {
        printf("reducedA %i != %i\n", reducedA, expectedFinalReducedA);
        success = false;
    }

    if (reducedB != expectedFinalReducedB) {
        printf("reducedB %i != %i\n", reducedB, expectedFinalReducedB);
        success = false;
    }

    return success;
}

int main(int argc, char* argv[])
{
    int numThreads = faasm::getIntInput();
    if (numThreads <= 0) {
        printf("Incorrect number of threads passed as input: %i\n", numThreads);
        return 1;
    }

    // Run reduce in a loop and check each iteration is correct
    int nLoops = 10;
    for (int i = 0; i < nLoops; i++) {
        bool success = doReduce(numThreads);
        if (!success) {
            printf("Repeated reduce failed on loop %i\n", i);
            return 1;
        }
    }

    printf("Repeated reduce successful over %i loops\n", nLoops);

    return 0;
}
