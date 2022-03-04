#include <faasm/counter.h>
#include <faasm/faasm.h>
#include <faasm/input.h>
#include <faasm/random.h>

#include <stdio.h>
#include <string>
#include <vector>

#define CHUNK_SIZE 100000
#define PI 3.14159

int piStep()
{
    auto sum = faasm::AtomicInt("pi");

    int count = 0;
    for (int i = 0; i < CHUNK_SIZE; i++) {
        // Two random points
        double x = faasm::randomFloat();
        double y = faasm::randomFloat();

        // Check if within unit circle
        if (x * x + y * y <= 1.0) {
            count = count + 1;
        }
    }

    sum += count;

    return 0;
}

/**
 * Parallel estimation of pi
 */
int main(int argc, char* argv[])
{
    std::string inputData = faasm::getStringInput("4");
    int nWorkers = std::stoi(inputData);
    int nTotal = CHUNK_SIZE * nWorkers;

    // Initialise the sum
    auto sum = faasm::AtomicInt("pi");
    sum.reset();

    // Dispatch chained calls
    faasmChainBatch(piStep, "", nWorkers);

    // Get the final result
    int finalCount = sum.get();

    // Estimate pi
    float pi = 4 * ((float)finalCount / (nTotal));
    float error = abs(PI - pi);

    std::string output = "Pi estimate: " + std::to_string(pi) + "(error " +
                         std::to_string(error) + ")\n";
    printf("%s", output.c_str());
    faasm::setStringOutput(output.c_str());

    return 0;
}
