#include <cmath>
#include <faasm/compare.h>
#include <faasm/faasm.h>
#include <mpi.h>
#include <stdio.h>
#include <vector>

int main(int argc, char* argv[])
{
    MPI_Init(NULL, NULL);

    int rank;
    int worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    // Prepare arguments
    // Note that the grid size is always 5x1x1 because the tests are always
    // ran with MPI_WORLD_SIZE = 5
    const int ndims = 3;
    int dims[ndims] = { 5, 1, 1 };
    int periods[ndims] = { 0, 0, 0 };
    int reorder = 0;
    MPI_Comm cart;

    // Expected arguments
    std::vector<std::vector<int>> expectedCoords = {
        { 0, 0, 0 }, { 1, 0, 0 }, { 2, 0, 0 }, { 3, 0, 0 }, { 4, 0, 0 },
    };
    std::vector<std::vector<int>> expectedShift = {
        { 4, 1, 0, 0, 0, 0 }, { 0, 2, 1, 1, 1, 1 }, { 1, 3, 2, 2, 2, 2 },
        { 2, 4, 3, 3, 3, 3 }, { 3, 0, 4, 4, 4, 4 },
    };

    // Test MPI_Cart_create
    if (MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, reorder, &cart) !=
        MPI_SUCCESS) {
        printf("MPI_Cart_init failed!\n");
        return 1;
    }
    printf("MPI_Cart_create succesful check.\n");

    // Test MPI_Cart_get
    int coords[ndims];
    if (MPI_Cart_get(cart, ndims, dims, periods, coords) != MPI_SUCCESS) {
        printf("MPI_Cart_get failed!\n");
        return 1;
    }

    // Test integrity of results
    if (!faasm::compareArrays<int>(
          coords, expectedCoords[rank].data(), ndims)) {
        printf("Wrong cartesian coordinates.\n");
        return 1;
    }
    printf("MPI_Cart_get succesful check.\n");

    // Test MPI_Cart_rank
    // Retrieve original rank from cartesian coordinates
    int nRank;
    if (MPI_Cart_rank(cart, coords, &nRank) != MPI_SUCCESS) {
        printf("MPI_Cart_rank failed!\n");
        return 1;
    }
    if (rank != nRank) {
        printf("Wrong rank from cartesian coordinates. Expected: %i Got: %i",
               rank,
               nRank);
        return 1;
    }
    printf("MPI_Cart_rank succesful check.\n");

    printf("MPI Cartesian Topology Checks Succesful.\n");

    // Test MPI_Cart_shift
    // Shift cartesian coordinates along first axis
    int src, dst;
    for (int axis = 0; axis < 3; axis++) {
        if (MPI_Cart_shift(cart, axis, 1, &src, &dst) != MPI_SUCCESS) {
            printf("MPI_Cart_shift failed!\n");
            return 1;
        }
        if (src != expectedShift[rank][2 * axis] or
            dst != expectedShift[rank][2 * axis + 1]) {
            printf("Wrong (src, dst) pair in cart shift. Expected (%i, %i) got "
                   "(%i, %i)",
                   expectedShift[rank][2 * axis],
                   expectedShift[rank][2 * axis + 1],
                   src,
                   dst);
            return 1;
        }
    }
    printf("MPI_Cart_shift succesful check.\n");

    MPI_Finalize();

    return 0;
}
