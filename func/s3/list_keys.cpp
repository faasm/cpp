extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    // Get the bucket name as an input
    int inputSize = faasmGetInputSize();
    char bucketName[inputSize];
    faasmGetInput((uint8_t*) bucketName, inputSize);

    int numKeys = __faasm_s3_get_num_keys(bucketName);

    char* keysBuffer[numKeys];
    int keysBufferLens[numKeys];
    __faasm_s3_list_keys(bucketName, keysBuffer, keysBufferLens);

    int totalSize = 0;
    for (int i = 0; i < numKeys; i++) {
        totalSize += keysBufferLens[i];
    }
    totalSize += numKeys - 1;

    // Prepare the output: instead of a newline use a '|' character
    char outBuffer[totalSize];

    printf("Bucket %s has %i keys!\n", bucketName, numKeys);
    int offset = 0;
    for (int i = 0; i < numKeys; i++) {
        strncpy(outBuffer + offset, keysBuffer[i], keysBufferLens[i]);
        offset += keysBufferLens[i];
        if (i < numKeys - 1) {
            outBuffer[offset] = (char) '|';
            offset += 1;
        }
        printf("Key %i: %s\n", i, keysBuffer[i]);
    }

    faasmSetOutput(outBuffer, totalSize);

    return 0;
}
