#include <cstdio>
#include <math.h>
#include <omp.h>
#include <unistd.h>

/**
 * We want to check handling of multipled private and shared variables as part
 * of a reduction of multiple variables.
 */

struct DummyStruct
{
    int a = 10;
    double b = 25.1;
};

bool approxCompare(float a, float b)
{
    return truncf(1000.0f * a) == truncf(1000.0f * b);
}

int main(int argc, char* argv[])
{
    int chunkSize = 20;
    int nThreads = 5;
    int counts[] = { 0, 0, 0, 0, 0 };
    int loopSize = nThreads * chunkSize;

    int privateA = 0;
    double privateB = 0;

    int reducedA = 0;
    double reducedB = 0;

    int sharedA = 100;
    DummyStruct sharedB;

#pragma omp parallel for num_threads(nThreads) default(none) \
    private(privateA,privateB) \
    shared(loopSize,sharedA,sharedB,counts) \
    reduction(+ : reducedA,reducedB)
    for (int i = 0; i < loopSize; i++) {
        int threadNum = omp_get_thread_num();

        counts[threadNum]++;

        int expectedPrivateA = threadNum + 1;
        double expectedPrivateB = (double)2.5 * threadNum + 1;

        privateA = expectedPrivateA;
        privateB = expectedPrivateB;

        reducedA += sharedA * threadNum;

        // Note we need this to be a fraction to check we're not rounding to an
        // integer
        reducedB += sharedB.b * (threadNum / 4.2);

        // Wait and check privates haven't changed
        usleep(30 * 1000);
        if (privateA != expectedPrivateA) {
            printf("Nested privateA %i != %i\n", privateA, expectedPrivateA);
            exit(1);
        }
        if (privateB != expectedPrivateB) {
            printf("Nested privateB %f != %f\n", privateB, expectedPrivateB);
            exit(1);
        }
    }

    // Check counts
    for (int t = 0; t < nThreads; t++) {
        if (counts[t] != chunkSize) {
            printf(
              "Loop count for thread %i: %i != %i\n", t, counts[t], chunkSize);
            return 1;
        }
    }

    // Note - to get these values you can just compile and run natively
    int expectedReducedA = 20000;
    double expectedReducedB = 1195.238095;

    if (reducedA != expectedReducedA) {
        printf("reducedA %i != %i\n", reducedA, expectedReducedA);
        return 1;
    }

    if (!approxCompare(reducedB, expectedReducedB)) {
        printf("reducedB %f != %f (based on approx compare)\n",
               reducedB,
               expectedReducedB);
        return 1;
    }

    // Check private values to be those of the main thread;
    int expectedPrivateA = 1;
    double expectedPrivateB = 1;
    if (privateA != 0) {
        printf("privateA %i != %i\n", privateA, expectedPrivateA);
        return 1;
    }

    if (privateB != 0) {
        printf("privateB %f != %f\n", privateB, expectedPrivateB);
        return 1;
    }

    printf("OpenMP complex reduction checks complete\n");
}
