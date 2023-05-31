#include <faasm/faasm.h>
#include <stdio.h>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    const char* key = "state_shared_example";

    // Write an empty value
    std::vector<uint8_t> bytes = { 0, 0, 0, 0, 0, 0, 0 };
    unsigned long bytesLen = bytes.size();
    faasmWriteState(key, bytes.data(), bytesLen);

    // Get two pointers to the memory
    uint8_t* actualA = faasmReadStatePtr(key, bytesLen);
    uint8_t* actualB = faasmReadStatePtr(key, bytesLen);

    // Write to one pointer
    uint8_t newValue = 5;
    for (unsigned long i = 0; i < bytesLen; i++) {
        actualA[i] = newValue;
    }

    // Check update seen by other
    bool success = actualA == actualB;
    for (unsigned long i = 0; i < bytesLen; i++) {
        success &= (actualB[i] == newValue);
    }

    std::string successStr = "success";
    if (!success) {
        successStr = "failure";
    }
    faasmSetOutput(successStr.c_str(), successStr.size());

    return 0;
}
