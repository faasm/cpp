#include "faasm/faasm.h"
#include "faasm/input.h"

#include <stdio.h>
#include <string.h>

/**
 * Writes the input to the output
 */
int main(int argc, char* argv[])
{
    const char* inputStr = faasm::getStringInput("");
    size_t inputLen = strlen(inputStr);

    // Handle empty input
    if (inputLen == 0) {
        const char* output = "Nothing to echo";
        faasmSetOutput(output, strlen(output));
        return 0;
    }

    printf("Echoing %s\n", inputStr);

    faasmSetOutput(inputStr, inputLen);

    return 0;
}
