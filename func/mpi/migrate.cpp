#include <mpi.h>
#include <stdio.h>

#include <faasm/faasm.h>
#include <faasm/migrate.h>

#define RANK_THRESHOLD 0.5
#define LOOP_THRESHOLD 100

// Outer wrapper, and re-entry point after migration
void doBenchmark(int nLoops)
{
    for (int i = 0; i < nLoops; i++) {
        // ---------------------------------------
        // Do benchmark work here, e.g. with MPI_Allreduce
        // ---------------------------------------
        int rank;
        int worldSize;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

        // Make sure everyone is in sync (including those ranks that have been
        // migrated)
        MPI_Barrier(MPI_COMM_WORLD);

        // Send message to rank before
        int sender = rank == worldSize - 1 ? 0 : rank + 1;
        int recipient = rank == 0 ? worldSize - 1 : rank - 1;
        int sentNumber = 123;
        MPI_Send(&sentNumber, 1, MPI_INT, recipient, 0, MPI_COMM_WORLD);

        int receivedNumber;
        MPI_Recv(&receivedNumber,
                 sender,
                 MPI_INT,
                 0,
                 0,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        int rankThreshold = worldSize  * RANK_THRESHOLD;
        if (nLoops % LOOP_THRESHOLD == 0) {
            // Migration point, which may or may not resume the
            // benchmark on another host for the remaining iterations.
            // This would eventually be MPI_Barrier
            MPI_Barrier(MPI_COMM_WORLD);
            if (rank > rankThreshold) {
                __faasm_migrate_point(&doBenchmark, i);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    // Initialisation
    int res = MPI_Init(NULL, NULL);
    if (res != MPI_SUCCESS) {
        printf("Failed on MPI init\n");
        return 1;
    }

    // Number of benchmark loops, can be configurable
    int nLoops = 1000;
    doBenchmark(nLoops);

    // Shutdown
    MPI_Finalize();
}
