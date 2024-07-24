#ifdef __faasm
extern "C"
{
#include "faasm/host_interface.h"
}

#include <faasm/faasm.h>
#endif

#include <map>
#include <sstream>
#include <string>

std::map<std::string, int> wordCount = {
    {"JavaScript", 0},
    {"Java", 0},
    {"PHP", 0},
    {"Python", 0},
    {"C#", 0},
    {"C++", 0},
    {"Ruby", 0},
    {"CSS", 0},
    {"Objective-C", 0},
    {"Perl", 0},
    {"Scala", 0},
    {"Haskell", 0},
    {"MATLAB", 0},
    {"Clojure", 0},
    {"Groovy", 0}
};

std::string serialiseWordCount()
{
    std::string result;

    for (const auto& [key, val] : wordCount) {
        result += key + ":" + std::to_string(val) + ",";
    }
    result.pop_back();

    return result;
}

/* Mapper Function - Step 2 of MapReduce Workflow
 *
 * The mapper function taks as an input an S3 path, and, as an output, writes
 * a serialized JSON to S3 with the partial counts of different programming
 * languages.
 */
int main(int argc, char** argv)
{
    // TODO: this is currently hardcoded
    std::string bucketName = "tless";
    std::string s3ObjectKey;

#ifdef __faasm
    // Get the object key as an input
    int inputSize = faasmGetInputSize();
    char keyName[inputSize];
    faasmGetInput((uint8_t*)keyName, inputSize);
    s3ObjectKey.assign(keyName, keyName + inputSize);
#endif

    // Read object from S3
    uint8_t* keyBytes;
#ifdef __faasm
    int keyBytesLen;

    int ret =
      __faasm_s3_get_key_bytes(bucketName.c_str(), s3ObjectKey.c_str(), &keyBytes, &keyBytesLen);
    if (ret != 0) {
        printf("error: error getting bytes from key: %s (bucket: %s)\n",
               s3ObjectKey.c_str(),
               bucketName.c_str());
    }
#endif

    std::stringstream stringStream((char*) keyBytes);
    std::string currentLine;
    while (std::getline(stringStream, currentLine, '\n')) {
        for (auto& [key, val] : wordCount) {
            if (currentLine.find(key) != std::string::npos) {
                val += 1;
            }
        }
    }

    printf("final count:\n");
    for (const auto& [key, val] : wordCount) {
        printf("\t- %s: %i\n", key.c_str(), val);
    }

    // Work-out the serialised payload and directory
    auto serialisedWordCount = serialiseWordCount();
    size_t lastSlash = s3ObjectKey.rfind("/");
    if (lastSlash != std::string::npos) {
        s3ObjectKey.insert(lastSlash + 1, "mapper-results/");
    } else {
        s3ObjectKey = "mapper-results/" + s3ObjectKey;
    }

    printf("would write result to: %s\n", s3ObjectKey.c_str());
    // TODO: write serialised result
/*
#ifdef __faasm
    int ret =
      __faasm_s3_add_key_bytes(bucketName.c_str(),
                               s3ObjectKey.c_str(),
                               (void*) serialiseWordCount.c_str(),
                               serialiseWordCount.size());
#endif
*/

    return 0;
}
