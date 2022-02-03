#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include <faasm/compare.h>
#include <faasm/faasm.h>
#include <faasm/migrate.h>

#define NUM_LOOPS 50000
int checkEvery = 1;

int doAlltoAll(int rank, int worldSize, int i, int nLoops, int checkEvery)
{
    int retVal = 0;
    int chunkSize = 2;
    int fullSize = worldSize * chunkSize;

    // Arrays for sending and receiving
    int* sendBuf = new int[fullSize];
    int* expected = new int[fullSize];
    int* actual = new int[fullSize];

    // Populate data
    for (int i = 0; i < fullSize; i++) {
        // Send buffer from this rank
        sendBuf[i] = (rank * 10) + i;

        // Work out which rank this chunk of the expectation will come from
        int rankOffset = (rank * chunkSize) + (i % chunkSize);
        int recvRank = i / chunkSize;
        expected[i] = (recvRank * 10) + rankOffset;
    }

    MPI_Alltoall(
      sendBuf, chunkSize, MPI_INT, actual, chunkSize, MPI_INT, MPI_COMM_WORLD);

    if (!faasm::compareArrays<int>(actual, expected, fullSize)) {
        retVal = 1;
    }

    delete[] sendBuf;
    delete[] actual;
    delete[] expected;

    return retVal;
}

// Outer wrapper, and re-entry point after migration
void doBenchmark(int nLoops)
{
    bool mustCheck = nLoops == NUM_LOOPS;

    // Initialisation
    int res = MPI_Init(NULL, NULL);
    if (res != MPI_SUCCESS) {
        printf("Failed on MPI init\n");
        return;
    }

    int rank;
    int worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    for (int i = 0; i < nLoops; i++) {
        if (rank == 0 && i % (nLoops / 10) == 0) {
            printf("Starting iteration %i/%i\n", i, nLoops);
        }

        // Make sure everyone is in sync (including those ranks that have been
        // migrated)
        MPI_Barrier(MPI_COMM_WORLD);

        doAlltoAll(rank, worldSize, i, nLoops, checkEvery);

        if (mustCheck && i % checkEvery == 0 && i / checkEvery > 0) {
            mustCheck = false;
            if (rank == 0) {
                printf(
                  "Checking for migrations at iteration %i/%i\n", i, nLoops);
            }
            // Migration point, which may or may not resume the
            // benchmark on another host for the remaining iterations.
            // This would eventually be MPI_Barrier
            MPI_Barrier(MPI_COMM_WORLD);
            __faasm_migrate_point(&doBenchmark, (nLoops - i - 1));
        }
    }

    printf("Rank %i exitting the loop\n", rank);
    MPI_Barrier(MPI_COMM_WORLD);

    // Shutdown
    MPI_Finalize();
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Must provide one input argument: <check_period>\n");
        return 1;
    }

    // Filthy hack to set the check period without modifying the function
    // signature. Note that the migrated functions won't see the updated
    // value as we don't migrate global variables, but that's OK as we don't
    // support migrating a function twice.
    int checkEveryIn = atoi(argv[1]);
    int* checkEveryPtr = &checkEvery;
    *checkEveryPtr = (int)(NUM_LOOPS * ((float)checkEveryIn / 10.0));

    printf(
      "Starting MPI migration checking at iter %i/%i\n", checkEvery, NUM_LOOPS);

    doBenchmark(NUM_LOOPS);

    printf("MPI migration benchmark finished succesfully\n");
}
