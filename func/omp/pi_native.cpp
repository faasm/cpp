#include <cstdio>
#include <math.h>
#include <omp.h>
#include <random>

#define CHUNK_SIZE 1000000
#define PI 3.14159

unsigned long genSeed()
{
    int threadNum = omp_get_thread_num();
    return threadNum * threadNum * 77 - 22 * threadNum + 1927;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Specify number of workers\n");
        return 1;
    }

    int nWorkers = std::stoi(argv[1]);
    long result = 0;
    int nTotal = nWorkers * CHUNK_SIZE;

    printf("Estimating Pi with %i workers\n", nWorkers);

#pragma omp parallel num_threads(nWorkers) default(none) shared(nTotal)        \
  reduction(+ : result)
    {
        // Different seed per thread
        std::uniform_real_distribution<double> unif(0, 1);
        std::mt19937_64 generator(genSeed());

#pragma omp for
        for (int i = 0; i < nTotal; i++) {
            double x = unif(generator);
            double y = unif(generator);

            if ((x * x + y * y) <= 1.0) {
                result++;
            }
        }
    }

    float pi = 4 * (((float)result) / (float)nTotal);
    float error = abs(PI - pi);
    printf("Pi estimate: %.5f (error %.5f)\n", pi, error);

    return 0;
}
