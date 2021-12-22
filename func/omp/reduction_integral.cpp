#include <cstdio>
#include <omp.h>

#include <faasm/shared_mem.h>

#define NTHREADS 10

bool checkResult(const char* func, double reduction)
{
    if (reduction < 3 || reduction > 4) {
        printf("Function %s didn't return pi but %f\n", func, reduction);
        return true;
    }
    return false;
}

double roundRobin()
{
    int nthreads;
    long nSteps = 100000000;
    double step = 0;
    double pi = 0.0;
    double sum[NTHREADS];

    step = 1.0 / (double)nSteps;
    double timerStart = omp_get_wtime();
    omp_set_num_threads(NTHREADS);

    FAASM_SHARED_VAR(nthreads, FAASM_TYPE_INT)
    FAASM_SHARED_VAR(nSteps, FAASM_TYPE_LONG)
    FAASM_SHARED_VAR(step, FAASM_TYPE_DOUBLE)
    FAASM_SHARED_ARRAY(sum, FAASM_TYPE_DOUBLE, NTHREADS)

#pragma omp parallel default(none) shared(nthreads, nSteps, step, sum)
    {
        int i, id, lnthreads;
        double x;

        lnthreads = omp_get_num_threads();
        id = omp_get_thread_num();
        if (id == 0) {
            nthreads = lnthreads;
        }

        for (i = id, sum[id] = 0; i < nSteps; i += lnthreads) {
            x = (i + 0.5) * step;
            sum[id] += 4.0 / (1.0 + x * x);
        }
    }
    for (int i = 0; i < nthreads; ++i) {
        pi += sum[i] * step;
    }

    double timerEnd = omp_get_wtime() - timerStart;
    printf("RR took %f, pi: %f\n", timerEnd, pi);
    return pi;
}

double doAtomic()
{
    long nSteps = 100000000;
    double step = 0;
    double pi = 0.0;

    step = 1.0 / (double)nSteps;
    double timerStart = omp_get_wtime();
    omp_set_num_threads(4);

    FAASM_SHARED_VAR(nSteps, FAASM_TYPE_LONG)
    FAASM_SHARED_VAR(step, FAASM_TYPE_DOUBLE)
    FAASM_SHARED_VAR(pi, FAASM_TYPE_DOUBLE)

#pragma omp parallel default(none) shared(nSteps, step, pi)
    {
        int i, id, lnthreads;
        double x, sum = 0;

        lnthreads = omp_get_num_threads();
        id = omp_get_thread_num();

        for (i = id; i < nSteps; i += lnthreads) {
            x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }

#pragma omp critical
        {
            pi += sum * step;
        }
    }

    double timerEnd = omp_get_wtime() - timerStart;
    printf("Atomic took %f, pi: %f\n", timerEnd, pi);
    return pi;
}

double doReduction()
{
    long nSteps = 100000000;
    double step = 0;
    double pi = 0.0;
    double sum = 0;
    int i = 0;

    step = 1.0 / (double)nSteps;

    omp_set_num_threads(NTHREADS);
    double timerStart = omp_get_wtime();

    FAASM_SHARED_VAR(nSteps, FAASM_TYPE_LONG)
    FAASM_SHARED_VAR(step, FAASM_TYPE_DOUBLE)
    FAASM_REDUCE(sum, FAASM_TYPE_DOUBLE, FAASM_OP_SUM)

#pragma omp parallel for default(none) shared(nSteps, step) reduction(+ : sum)
    for (i = 0; i < nSteps; ++i) {
        int x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    pi = sum * step;

    double timerEnd = omp_get_wtime() - timerStart;
    printf("Reduction took %f, pi: %f\n", timerEnd, pi);
    return pi;
}

double doBetterReduction()
{
    // This version is better because it can work in non-threaded environments.
    long nSteps = 100000000;
    double step = 0;
    double pi = 0.0;
    double sum = 0;
    int i = 0, x;

    step = 1.0 / (double)nSteps;

    omp_set_num_threads(NTHREADS);
    double timerStart = omp_get_wtime();

    FAASM_SHARED_VAR(nSteps, FAASM_TYPE_LONG)
    FAASM_SHARED_VAR(step, FAASM_TYPE_DOUBLE)
    FAASM_REDUCE(sum, FAASM_TYPE_DOUBLE, FAASM_OP_SUM)

#pragma omp parallel for private(x) default(none) shared(nSteps, step) reduction(+:sum)
    for (i = 0; i < nSteps; ++i) {
        x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    pi = sum * step;

    double timerEnd = omp_get_wtime() - timerStart;
    printf("Better reduction took %f, pi: %f\n", timerEnd, pi);
    return pi;
}

int main(int argc, char* argv[])
{
    bool failed = false;
    failed |= checkResult("Atomic", doAtomic());
    failed |= checkResult("RR", roundRobin());
    failed |= checkResult("Reduction", doReduction());
    failed |= checkResult("Better reduction", doBetterReduction());

    if (failed) {
        return 1;
    }
    return 0;
}
