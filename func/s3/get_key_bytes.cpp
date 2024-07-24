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
        printf("error: must invoke function with two arguments: bucketName keyName\n");
        return 1;
    }

    char* bucketName = argv[1];
    char* keyName = argv[2];

    uint8_t* keyBytes;
    int keyBytesLen;

    int ret = __faasm_s3_get_key_bytes(bucketName, keyName, &keyBytes, &keyBytesLen);
    printf("Got %s/%s: %s\n", bucketName, keyName, (char*) keyBytes);
    faasmSetOutput((char*) keyBytes, keyBytesLen);

    return ret;
}
