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

    int reducedA = 0;
    double reducedB = 0;

#pragma omp parallel for num_threads(nThreads) default(none) \
    shared(counts,loopSize,chunkSize) \
    reduction(+ : reducedA,reducedB)
    for (int i = 0; i < loopSize; i++) {
        int threadNum = omp_get_thread_num();
        counts[threadNum]++;

        // Add one here so that thread zero still has an effect
        reducedA = 10 * (threadNum + 1);
        reducedB += 15 * ((threadNum + 1) / 4.2);
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
    int expectedReducedA = 150;
    double expectedReducedB = 1071.428571;

    if (reducedA != expectedReducedA) {
        printf("reducedA %i != %i\n", reducedA, expectedReducedA);
        return false;
    }

    if (!approxCompare(reducedB, expectedReducedB)) {
        printf("reducedB %f != %f (based on approx compare)\n",
               reducedB,
               expectedReducedB);
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
