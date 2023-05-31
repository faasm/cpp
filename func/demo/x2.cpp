#include "faasm/faasm.h"
#include <string>

/**
 * Multiplies its array input by two
 */
int main(int argc, char* argv[])
{
    long inputSize = faasmGetInputSize();
    auto inputBuffer = new uint8_t[inputSize];
    faasmGetInput(inputBuffer, inputSize);

    auto output = new uint8_t[inputSize];

    for (int i = 0; i < inputSize; i++) {
        output[i] = inputBuffer[i] * 2;
    }

    std::string outputStr = "success";
    faasmSetOutput(outputStr.c_str(), outputStr.size());

    return 0;
}
