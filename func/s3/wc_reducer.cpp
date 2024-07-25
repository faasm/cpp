#ifdef __faasm
extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#endif
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

/* Reducer Function - Word Count Workflow
 *
 * This function takes a path to a directory as an input, reads the serialised
 * counts from each file in the directory, and then aggreagates the results
 * to one final count.
 */
int main(int argc, char** argv)
{
    // TODO: the bucket name is currently hardcoded
    std::string bucketName = "tless";
    std::string s3dir;

#ifdef __faasm
    // Get the results dir as an input
    int inputSize = faasmGetInputSize();
    char s3dirChar[inputSize];
    faasmGetInput((uint8_t*)s3dirChar, inputSize);
    s3dir.assign(s3dirChar, s3dirChar + inputSize);
#endif

    // Get the list of files in the s3 dir
    std::vector<std::string> s3files;

#ifdef __faasm
    // In Faasm we need to do a bit of work because: (i) we can not pass
    // structured objects (i.e. vectors) through the WASM calling interface,
    // and (ii) we have not implmented prefix listing, so we need to filter
    // out entries manually
    int numKeys = __faasm_s3_get_num_keys(bucketName.c_str());

    char* keysBuffer[numKeys];
    int keysBufferLens[numKeys];
    __faasm_s3_list_keys(bucketName.c_str(), keysBuffer, keysBufferLens);

    for (int i = 0; i < numKeys; i++) {
        std::string tmpString;
        tmpString.assign(keysBuffer[i], keysBuffer[i] + keysBufferLens[i]);

        // Filter by prefix
        if (tmpString.rfind(s3dir, 0) == 0) {
            printf("word-count(reducer): using this output file %s\n", tmpString.c_str());
            s3files.push_back(tmpString);
        }
    }
#endif

    // TODO: for each output file, de-serialise results and aggreagate

    return 0;
}
