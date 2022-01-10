#include <faasm/compare.h>
#include <faasm/faasm.h>
#include <mpi.h>
#include <stdio.h>
#include <string.h>
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

int doAllReduce(int rank, int worldSize)
{
    // Create an array of three numbers for this process
    int numsThisProc[3] = { rank, 10 * rank, 100 * rank };
    int* expected = new int[3];
    int* result = new int[3];

    // Build expectation
    memset(expected, 0, 3 * sizeof(int));
    for (int r = 0; r < worldSize; r++) {
        expected[0] += r;
        expected[1] += 10 * r;
        expected[2] += 100 * r;
    }

    // Call allreduce
    MPI_Allreduce(numsThisProc, result, 3, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Check vs. expectation
    if (!faasm::compareArrays<int>(result, expected, 3)) {
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);

    int rank;
    int worldSize;
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

    // Force a migration point in all-reduce
    if (doAllReduce(rank, worldSize) != 0) {
        return 1;
    }

    printf("Rank %i: Allreduce as expected\n", rank);

    // Do an all-to-all message
    if (doAlltoAll(rank, worldSize) != 0) {
        return 1;
    }

    printf("Rank %i: second alltoall as expected\n", rank);

    MPI_Finalize();

    return MPI_SUCCESS;
}
