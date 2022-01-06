#include <faasm/faasm.h>
#include <omp.h>
#include <stdio.h>

void printThreadInfo(const char* label)
{
    int thisThread = omp_get_thread_num();
    unsigned int totalThreads = omp_get_num_threads();
    printf("%s %d of %d\n", label, thisThread, totalThreads);
}

int main(int argc, char* argv[])
{
    int nThreads = 6;

    printThreadInfo("Main before");

#pragma omp parallel num_threads(nThreads) default(none)
    {
        printThreadInfo("OMP thread");
    }

    printThreadInfo("Main after");
    return 0;
}
