extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#include <stdio.h>
#include <string>

int main(int argc, char* argv[])
{
    // Get the bucket name as an input
    int inputSize = faasmGetInputSize();
    char bucketName[inputSize];
    faasmGetInput((uint8_t*) bucketName, inputSize);

    int numKeys = __faasm_s3_get_num_keys(bucketName);

    printf("Bucket %s has %i keys!\n", bucketName, numKeys);

    std::string numKeysStr = std::to_string(numKeys);
    faasmSetOutput(numKeysStr.c_str(), numKeysStr.size());

    return 0;
}
