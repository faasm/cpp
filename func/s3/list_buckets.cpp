extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int numBuckets = __faasm_s3_get_num_buckets();

    char* bucketsBuffer[numBuckets];
    int bucketsBufferLens[numBuckets];
    __faasm_s3_list_buckets(bucketsBuffer, bucketsBufferLens);

    int totalSize = 0;
    for (int i = 0; i < numBuckets; i++) {
        totalSize += bucketsBufferLens[i];
    }
    totalSize += numBuckets - 1;

    // Prepare the output: instead of a newline use a '|' character
    char outBuffer[totalSize];

    printf("Got %i buckets!\n", numBuckets);
    int offset = 0;
    for (int i = 0; i < numBuckets; i++) {
        strncpy(outBuffer + offset, bucketsBuffer[i], bucketsBufferLens[i]);
        offset += bucketsBufferLens[i];
        if (i < numBuckets - 1) {
            outBuffer[offset] = (char) '|';
            offset += 1;
        }
        printf("Bucket %i: %s\n", i, bucketsBuffer[i]);
    }

    faasmSetOutput(outBuffer, totalSize);

    return 0;
}
