#include <cstdio>
#include <math.h>
#include <omp.h>

#include <faasm/shared_mem.h>

/**
 * We want to check the single thread case works for everything we test
 * elsewhere with multiple threads.
 */

bool approxCompare(float a, float b)
{
    return truncf(1000.0f * a) == truncf(1000.0f * b);
}

int main(int argc, char* argv[])
{
    int loopSize = 100;
    int nThreads = 5;

    int privateA = 0;
    double privateB = 0;

    int reducedA = 0;
    double reducedB = 0;

    double sharedA = 105.5;

    FAASM_REDUCE(reducedA, FAASM_TYPE_INT, FAASM_OP_SUM)
    FAASM_REDUCE(reducedB, FAASM_TYPE_DOUBLE, FAASM_OP_SUM)
    FAASM_SHARED_VAR(loopSize, FAASM_TYPE_INT)
    FAASM_SHARED_VAR(sharedA, FAASM_TYPE_DOUBLE)

#pragma omp parallel for num_threads(1) default(none) \
    private(privateA,privateB) \
    reduction(+ : reducedA,reducedB) \
    shared(loopSize, sharedA)
    for (int i = 0; i < loopSize; i++) {
        int threadNum = omp_get_thread_num();

        reducedA += 10;

        // Note we need this to be a fraction to check we're not rounding to an
        // integer
        reducedB += sharedA * i;
    }

    // Note - to get these values you can just compile and run natively
    int expectedReducedA = 1000;
    double expectedReducedB = 522225.0;

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

    printf("OpenMP single thread reduction checks complete\n");
}
