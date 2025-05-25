#include <faasm/faasm.h>
#include <faasm/input.h>
#include <faasm/shared_mem.h>

#include <cstdio>
#include <omp.h>
#include <string>
#include <unistd.h>

/* This method performs an image similarity search with an elastic number of
 * threads
 */
bool doImageSim(int numThreads)
{
#pragma omp parallel for num_threads(numThreads)
    // We need to give the for loop room to potentially grow when numThreads is
    // internally scaled-up by the runtime. Here we hard-code to a situation
    // where we initially run with numThreads = nproc / 2 and elastically scale
    // to numThreads = nproc
    for (int i = 0; i < numThreads * 2; i++) {
        usleep(2 * 1000 * 1000);

        int callId = -1;
        while (callId == -1) {
            callId = faasmChainNamed("imagesim", nullptr, 0);

            if (callId == -1) {
                printf("ERROR: executing image-similarity (no hosts?)\n");
                usleep(1 * 1000 * 1000);
            }
        }

        faasmAwaitCall(callId);
    }

    return 0;
}

void doSleep(int numThreads)
{
#pragma omp parallel for num_threads(numThreads)
    for (int i = 0; i < numThreads; i++) {
        usleep(2 * 1000 * 1000);
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("ERROR: usage: <program> <num_threads> <iters_per_thread>\n");
        return 1;
    }
    int numThreads = std::stoi(argv[1]);
    int itersPerThread = std::stoi(argv[2]);

    // If we pass an itersPerThread of 0, it means that we don't run the image
    // simulation workload, instead we just sleep on each thread for a while to
    // take up resources
    if (itersPerThread == 0) {
        doSleep(numThreads);
    }

    for (int i = 0; i < itersPerThread; i++) {
        doImageSim(numThreads);
    }

    return 0;
}
