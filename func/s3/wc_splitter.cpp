#ifdef __faasm
extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#endif

#include <stdio.h>
#include <string>
#include <vector>

/* Spliiter Function - MapReduce Workflow
 *
 * This function takes as an input an S3 path, and invokes one mapper function
 * for each file in the S3 path. The chaining is asynchronous.
 *
 * The function returns a comma-separated list of the message ids corresponding
 * to all the invoked functions.
 */
int main(int argc, char** argv)
{
    // TODO: the bucket name is currently hardcoded
    std::string bucketName = "tless";
    std::string s3dir;

#ifdef __faasm
    // Get the object key as an input
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

    int totalSize = 0;
    for (int i = 0; i < numKeys; i++) {
        std::string tmpString;
        tmpString.assign(keysBuffer[i], keysBuffer[i] + keysBufferLens[i]);

        // Filter by prefix
        if (tmpString.rfind(s3dir, 0) == 0) {
            // Filter-out sub-directories to store results
            if (tmpString.find("results") == std::string::npos) {
                s3files.push_back(tmpString);
            }
        }
    }
#endif

    // Chain to one mapper function per file, and store the message id to be
    // able to wait on it
    std::vector<int> splitterCallIds;
    for (const auto& s3file : s3files) {
#ifdef __faasm
        printf("word-count(splitter): chaining to mapper with file %s\n", s3file.c_str());
        int splitterId = faasmChainNamed("wc_mapper", (uint8_t*) s3file.c_str(), s3file.size());
        splitterCallIds.push_back(splitterId);
#endif
    }


    // Prepare the output: comma separated list of message ids
    std::string outputStr;
    for (const auto& splitterId : splitterCallIds) {
        outputStr += std::to_string(splitterId) + ",";
    }
    outputStr.pop_back();

#ifdef __faasm
    printf("word-count(splitter): my output is %s\n", outputStr.c_str());
    faasmSetOutput(outputStr.c_str(), outputStr.size());
#endif

    return 0;
}
