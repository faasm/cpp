#ifdef __faasm
#include <faasm/core.h>
#endif

#include <stdio.h>
#include <string>
#include <vector>

std::vector<std::string> splitByDelimiter(std::string stringCopy, const std::string& delimiter)
{
    std::vector<std::string> splitString;

    size_t pos = 0;
    std::string token;
    while ((pos = stringCopy.find(delimiter)) != std::string::npos) {
        splitString.push_back(stringCopy.substr(0, pos));
        stringCopy.erase(0, pos + delimiter.length());
    }
    splitString.push_back(stringCopy);

    return splitString;
}

/* Driver Function - MapReduce workflow
 *
 * This function acts as a "coordinator" for the MapReduce workflow. It
 * reduces the amount of workflow-specific logic that we need to implement in
 * Faasm.
 *
 * In a TLess context, the coordinator can be interpreted as "all the things
 * that could go wrong" during execution of a confidential serverless workflow.
 *
 * As an input, this workflow gets the S3 path read data from.
 */
int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("word-count(driver): error: workflow must be invoked with one parameter: <s3_prefix>\n");
        return 1;
    }
    std::string s3prefix = argv[1];

    // 1. Invoke one instance of the splitter function with an S3 path as an
    // input. The splitter function will, in turn, traverse the S3 directory
    // and invoke one mapper function per file in the directory.
    //
    // The return value of the splitter function is a list of message ids for
    // the mapper function
    printf("word-count(driver): invoking one splitter function\n");
#ifdef __faasm
    // Call splitter
    int splitterId = faasmChainNamed("wc_splitter", (uint8_t*) s3prefix.c_str(), s3prefix.size());
#endif

    char* splitterOutput;
    int splitterOutputLen;
#ifdef __faasm
    int result = faasmAwaitCallOutput(splitterId, &splitterOutput, &splitterOutputLen);
    if (result != 0) {
        printf("error: splitter execution failed with rc %i\n", result);
        return 1;
    }
#endif

    // Get all message ids from output
    std::vector<std::string>  mapperIds = splitByDelimiter(splitterOutput, ",");

    // 2. Wait for all mapper functions to have finished
    printf("word-count(driver): waiting for %zu mapper functions...\n", mapperIds.size());
    for (auto mapperIdStr : mapperIds) {
        int mapperId = std::stoi(mapperIdStr);
#ifdef __faasm
        result = faasmAwaitCall(mapperId);
        if (result != 0) {
            printf("error: mapper execution (id: %i) failed with rc %i\n", mapperId, result);
            return 1;
        }
#endif
    }

    // 3. Invoke one reducer function to aggreagate all results
    std::string s3result = s3prefix + "/mapper-results";
    printf("word-count(driver): invoking one reducer function\n");
#ifdef __faasm
    // Call reducer and await
    int reducerId = faasmChainNamed("wc_reducer", (uint8_t*) s3result.c_str(), s3result.size());
    result = faasmAwaitCall(reducerId);
    if (result != 0) {
        printf("error: reducer failed with rc %i\n", result);
        return 1;
    }
#endif

    return 0;
}
