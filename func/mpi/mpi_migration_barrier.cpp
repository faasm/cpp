#include <faasm/compare.h>
#include <faasm/faasm.h>
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int doAlltoAll(int rank, int worldSize)
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

int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);

    int rank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // Before starting, sleep to give the scheduler a chance to detect the
    // migration opportunity
    sleep(5);

    // Do an all-to-all message
    if (doAlltoAll(rank, worldSize) != 0) {
        return 1;
    }

    printf("Rank %i: first alltoall as expected\n", rank);

    // Force a migration here
    MPI_Barrier(MPI_COMM_WORLD);

    // Do an all-to-all message again
    if (doAlltoAll(rank, worldSize) != 0) {
        return 1;
    }

    printf("Rank %i: second alltoall as expected\n", rank);

    MPI_Finalize();

    return 0;
}
