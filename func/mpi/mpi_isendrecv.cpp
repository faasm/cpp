#include <faasm/faasm.h>
#include <mpi.h>
#include <stdio.h>

#include <unistd.h>

int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);

    int rank;
    int worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // Send and receive messages asynchronously in a ring
    int right = (rank + 1) % worldSize;
    int maxRank = worldSize - 1;
    int left = rank > 0 ? rank - 1 : maxRank;
    int sendValue = rank;
    int recvValue = -1;

    // Debug
    if (rank == 0) {
        MPI_Request sendRequest;
        MPI_Isend(&sendValue, 1, MPI_INT, right, 0, MPI_COMM_WORLD, &sendRequest);
    } else {
        recvValue = -1;
        MPI_Recv(
          &recvValue, 1, MPI_INT, left, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Check the received value is as expected
        if (recvValue != left) {
            printf("Rank %i - async not working properly (got %i expected %i)\n",
                   rank,
                   recvValue,
                   left);
            return 1;
        }
        printf("Rank %i - isend working properly\n", rank);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    int numIter = 1;

    for (int i = 0; i < numIter; i++) {
        printf("Starting iteration %i/%i\n", i + 1, numIter);

        // Isend

        // Asynchronously send to the right
        MPI_Request sendRequest;
        MPI_Isend(&sendValue, 1, MPI_INT, right, 0, MPI_COMM_WORLD, &sendRequest);

        // Receive from the left
        recvValue = -1;
        MPI_Recv(
          &recvValue, 1, MPI_INT, left, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Wait for both
        MPI_Wait(&sendRequest, MPI_STATUS_IGNORE);

        // Check the received value is as expected
        if (recvValue != left) {
            printf("Rank %i - async not working properly (got %i expected %i)\n",
                   rank,
                   recvValue,
                   left);
            return 1;
        }
        printf("Rank %i - isend working properly\n", rank);
        MPI_Barrier(MPI_COMM_WORLD);

        // Irecv

        // Send to the right
        MPI_Send(&sendValue, 1, MPI_INT, right, 0, MPI_COMM_WORLD);

        // Asynchronously receive from the left
        recvValue = -1;
        MPI_Request recvRequest;
        MPI_Irecv(&recvValue, 1, MPI_INT, left, 0, MPI_COMM_WORLD, &recvRequest);

        // Wait
        MPI_Wait(&recvRequest, MPI_STATUS_IGNORE);

        // Check the received value is as expected
        if (recvValue != left) {
            printf("Rank %i - irecv not working properly (got %i expected %i)\n",
                   rank,
                   recvValue,
                   left);
            return 1;
        }
        printf("Rank %i - irecv working properly\n", rank);
    }

    MPI_Finalize();

    return 0;
}
