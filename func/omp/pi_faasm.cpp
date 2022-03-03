#include <cstdio>
#include <omp.h>
#include <random>

#include <faasm/input.h>
#include <faasm/random.h>
#include <faasm/shared_mem.h>

#define CHUNK_SIZE 100000

int main(int argc, char** argv)
{
    std::string inputData = faasm::getStringInput("4");
    int nWorkers = std::stoi(inputData);

    long result = 0;
    int nTotal = nWorkers * CHUNK_SIZE;

    FAASM_REDUCE(result, FAASM_TYPE_LONG, FAASM_OP_SUM)
#pragma omp parallel num_threads(nWorkers) default(none) reduction(+ : result)
    {
#pragma omp for
        for (int i = 0; i < CHUNK_SIZE; i++) {
            double x = faasm::randomFloat();
            double y = faasm::randomFloat();

            if (x * x + y * y <= 1.0) {
                result++;
            }
        }
    }

    float pi = 4 * ((float)result / (nTotal));

    std::string output = "Pi estimate: " + std::to_string(pi) + "\n";
    printf("%s", output.c_str());
    faasm::setStringOutput(output.c_str());

    return 0;
}
