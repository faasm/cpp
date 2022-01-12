#include <mpi.h>
#include <stdio.h>

#include <faasm/faasm.h>
#include <faasm/migrate.h>

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

        // Send message to rank before
        int recipient = rank == 0 ? worldSize - 1 : rank - 1;
        int sentNumber = 123;
        MPI_Send(&sentNumber, 1, MPI_INT, recipient, 0, MPI_COMM_WORLD);

        // Migration point, which may or may not resume the
        // benchmark on another host for the remaining iterations.
        // This would eventually be MPI_Barrier
        __faasm_migrate_point(&doBenchmark, i);
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