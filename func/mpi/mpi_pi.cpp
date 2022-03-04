#include <math.h>
#include <mpi.h>
#include <random>
#include <stdio.h>
#include <string.h>

#include <faasm/input.h>

#define CHUNK_SIZE 100000
#define PI 3.14159

unsigned long genSeed(int rank)
{
    return rank * rank * 77 - 22 * rank + 1927;
}

int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);

    int rank;
    int worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int nTotal = worldSize * CHUNK_SIZE;

    // Different seed per thread
    std::uniform_real_distribution<double> unif(0, 1);
    std::mt19937_64 generator(genSeed(rank));

    int result = 0;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        double x = unif(generator);
        double y = unif(generator);

        if ((x * x + y * y) <= 1.0) {
            result++;
        }
    }

    MPI_Reduce(MPI_IN_PLACE, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Finalize();

    if (rank == 0) {
        float pi = 4 * (((float)result) / (float)nTotal);
        float error = abs(PI - pi);

        std::string output = "Pi estimate: " + std::to_string(pi) + " (error " +
                             std::to_string(error) + ")\n";
        printf("%s", output.c_str());
        faasm::setStringOutput(output.c_str());
    }

    return MPI_SUCCESS;
}
