#include "faasm/counter.h"
#include "faasm/faasm.h"

#include <string>

#include <stdio.h>

#define KEY "incr_example"
#define N_WORKERS 4

int checkCount()
{
    faasm::incrementCounter(KEY);
    return 0;
}

int main(int argc, char* argv[])
{
    faasm::initCounter(KEY);

    // Dispatch chained calls
    faasmChainBatch(checkCount, "", N_WORKERS);

    int count = faasm::getCounter(KEY);

    if (count != N_WORKERS) {
        printf("Count not as expected: %i != %i\n", count, N_WORKERS);
        return 1;
    }

    return 0;
}
