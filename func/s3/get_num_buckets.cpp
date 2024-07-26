extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#include <stdio.h>
#include <string>

int main(int argc, char* argv[])
{
    int numBuckets = __faasm_s3_get_num_buckets();

    printf("Got %i buckets!\n", numBuckets);

    std::string numBucketsStr = std::to_string(numBuckets);
    faasmSetOutput(numBucketsStr.c_str(), numBucketsStr.size());

    return 0;
}
