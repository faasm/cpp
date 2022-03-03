#include <faasm/counter.h>
#include <faasm/faasm.h>
#include <faasm/input.h>
#include <faasm/random.h>

#include <stdio.h>
#include <string>
#include <vector>

int piStep()
{
    auto sum = faasm::AtomicInt("pi");
    int chunkSize = faasm::getIntInput();

    int count = 0;
    for (int i = 0; i < chunkSize; i++) {
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
    int nWorkers = faasm::getIntInput();
    int nGuesses = 1000000;
    int chunkSize = nGuesses / nWorkers;

    // Initialise the sum
    auto sum = faasm::AtomicInt("pi");
    sum.reset();

    // Dispatch chained calls
    auto inputData = BYTES(&chunkSize);
    faasmChainBatch(piStep, inputData, sizeof(int), nWorkers);

    // Get the final result and estimate Pi
    int finalCount = sum.get();
    float piEstimate = 4 * ((float)finalCount / (nGuesses));

    std::string output = "Pi estimate: " + std::to_string(piEstimate) + "\n";
    printf("%s", output.c_str());
    faasm::setStringOutput(output.c_str());

    return 0;
}
