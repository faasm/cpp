#include "faasm/faasm.h"

#include <stdio.h>

#include <string>

int main(int argc, char* argv[])
{
    // Parse input from the encode function: "<input_file> <output_file>"
    long inputSize = faasmGetInputSize();
    char inputBuffer[inputSize];
    faasmGetInput((uint8_t*)inputBuffer, inputSize);
    // We look for the whitespace, replace it by a newline, and take advantace
    // of the C-style string initialisation in C++
    char* c = inputBuffer;
    while (*c != ' ') { c += sizeof(char); }
    *c = '\0';
    char* nextString = c + sizeof(char);
    std::string inputFile(inputBuffer);
    std::string outputFile(nextString);
    printf("in: %s\nout: %s\n", inputFile.c_str(), outputFile.c_str());

    const char* message = "Hello Faasm!";
    faasmSetOutput((uint8_t*)message, 12);

    return 0;
}
