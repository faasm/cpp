#include <cstdio>
#include <math.h>
#include <omp.h>
#include <unistd.h>

bool approxCompare(float a, float b)
{
    return truncf(1000.0f * a) == truncf(1000.0f * b);
}

bool doReduce()
{
    int nThreads = 5;
    int chunkSize = 20;
    int loopSize = nThreads * chunkSize;
    int counts[] = { 0, 0, 0, 0, 0 };

    short reducedO = 0;
    int reducedA = 0;
    double reducedB = 0;

    bool success = true;

#pragma omp parallel for num_threads(nThreads) default(none) \
    shared(counts,loopSize,success) \
    reduction(+ : reducedO,reducedA,reducedB)
    for (int i = 0; i < loopSize; i++) {
        int threadNum = omp_get_thread_num();
        counts[threadNum]++;
        int thisCount = counts[threadNum];

        // Add one here so that thread zero still has an effect
        reducedA += 10 * (threadNum + 1);
        reducedB += 15 * ((threadNum + 1) / 4.2);
        reducedO += 10;

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

    // Check counts
    for (int t = 0; t < nThreads; t++) {
        if (counts[t] != chunkSize) {
            printf(
              "Loop count for thread %i: %i != %i\n", t, counts[t], chunkSize);
            return false;
        }
    }

    // Note - to get these values you can just compile and run natively
    int expectedFinalReducedA = 3000;
    double expectedFinalReducedB = 1071.428571;

    if (reducedA != expectedFinalReducedA) {
        printf("reducedA %i != %i\n", reducedA, expectedFinalReducedA);
        return false;
    }

    if (!approxCompare(reducedB, expectedFinalReducedB)) {
        printf("reducedB %f != %f (approx)\n", reducedB, expectedFinalReducedB);
        return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    // Run reduce in a loop and check each iteration is correct
    int nLoops = 10000;
    for (int i = 0; i < nLoops; i++) {
        bool success = doReduce();
        if (!success) {
            printf("Repeated reduce failed on loop %i\n", i);
            return 1;
        }
    }

    printf("Repeated reduce successful over %i loops\n", nLoops);

    return 0;
}
