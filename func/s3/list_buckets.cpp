#include "faasm/faasm.h"

// TODO: get the number of expected buckets as input
int main(int argc, char* argv[])
{
    int numBuckets = __faasm_s3_get_num_buckets();

    printf("Got %i buckets!", numBuckets);

    return 0;
}
