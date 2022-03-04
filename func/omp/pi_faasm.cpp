#include <cstdio>
#include <omp.h>
#include <random>

#include <faasm/input.h>
#include <faasm/shared_mem.h>

#define CHUNK_SIZE 100000
#define PI 3.14159

unsigned long genSeed()
{
    int threadNum = omp_get_thread_num();
    return threadNum * threadNum * 77 - 22 * threadNum + 1927;
}

int main(int argc, char** argv)
{
    std::string inputData = faasm::getStringInput("4");
    int nWorkers = std::stoi(inputData);

    long result = 0;
    int nTotal = nWorkers * CHUNK_SIZE;

    FAASM_REDUCE(result, FAASM_TYPE_LONG, FAASM_OP_SUM)
#pragma omp parallel num_threads(nWorkers) default(none) \
        shared(nTotal) \
        reduction(+ : result)
    {
        // Different seed per thread
        std::uniform_real_distribution<double> unif(0, 1);
        std::mt19937_64 generator(genSeed());

#pragma omp for
        for (int i = 0; i < nTotal; i++) {
            double x = unif(generator);
            double y = unif(generator);

            if (x * x + y * y <= 1.0) {
                result++;
            }
        }
    }

    float pi = 4 * ((float)result / (nTotal));

    float error = abs(PI - pi);

    std::string output = "Pi estimate: " + std::to_string(pi) + " (error " +
                         std::to_string(error) + ")\n";
    printf("%s", output.c_str());
    faasm::setStringOutput(output.c_str());

    return 0;
}
