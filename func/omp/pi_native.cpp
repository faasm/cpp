#include <cstdio>
#include <math.h>
#include <omp.h>
#include <random>

#define CHUNK_SIZE 100000
#define PI 3.14159

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Specify number of workers\n");
        return 1;
    }

    srand(12345);

    int nWorkers = std::stoi(argv[1]);
    long result = 0;
    int nTotal = nWorkers * CHUNK_SIZE;

    printf("Estimating Pi with %i workers\n", nWorkers);

#pragma omp parallel for num_threads(nWorkers) default(none) \
    shared(nTotal) \
    reduction(+ : result)
    for (int i = 0; i < nTotal; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;

        if ((x * x + y * y) <= 1.0) {
            result++;
        }
    }

    float pi = 4 * (((float)result) / (float)nTotal);
    float error = abs(PI - pi);
    printf("Pi estimate: %.5f (error %.5f)\n", pi, error);

    return 0;
}
