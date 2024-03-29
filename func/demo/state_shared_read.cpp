#include <cstdio>
#include <faasm/faasm.h>
#include <string>

int main(int argc, char* argv[])
{
    const char* key = "state_shared_example";
    size_t stateSize = faasmReadStateSize(key);

    size_t expectedSize = 7;
    if (stateSize != expectedSize) {
        printf("Expected size %li but got %li\n", expectedSize, stateSize);
        return 1;
    }

    // Check that state has expected value
    uint8_t* actual = faasmReadStatePtr(key, stateSize);
    uint8_t expected[7] = { 5, 5, 5, 5, 5, 5, 5 };
    bool valuesMatch = true;
    for (int i = 0; i < 7; i++) {
        valuesMatch &= (actual[i] == expected[i]);
    }

    std::string successStr = "success";
    if (!valuesMatch) {
        successStr = "failure";
    }
    faasmSetOutput(successStr.c_str(), successStr.size());

    return 0;
}
