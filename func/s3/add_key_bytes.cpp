extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    // Get bucket and key from command line
    if (argc != 3) {
        printf("error: must invoke function with two arguments: bucketName "
               "keyName\n");
        return 1;
    }

    char* bucketName = argv[1];
    char* keyName = argv[2];

    // Get the bytes to add as input
    int inputSize = faasmGetInputSize();
    uint8_t keyBytes[inputSize];
    faasmGetInput(keyBytes, inputSize);

    int ret =
      __faasm_s3_add_key_bytes(bucketName, keyName, (void*)keyBytes, inputSize);

    return ret;
}
