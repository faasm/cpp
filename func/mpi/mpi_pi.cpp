#include <math.h>
#include <mpi.h>
#include <random>
#include <stdio.h>
#include <string.h>

#include <faasm/input.h>

#define CHUNK_SIZE 100000
#define PI 3.14159

int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);

    srand(12345);

    int rank;
    int worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int nTotal = worldSize * CHUNK_SIZE;

    int result = 0;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;

        if ((x * x + y * y) <= 1.0) {
            result++;
        }
    }

    printf("RANK %i RESULT: %i\n", rank, result);
    MPI_Reduce(
      &result, MPI_IN_PLACE, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    if (rank == 0) {
        float pi = 4 * (((float)result) / (float)nTotal);
        float error = abs(PI - pi);

        std::string output = "Pi estimate: " + std::to_string(pi) + "(error " +
                             std::to_string(error) + ")\n";
        printf("%s", output.c_str());
        faasm::setStringOutput(output.c_str());
    }

    return MPI_SUCCESS;
}
