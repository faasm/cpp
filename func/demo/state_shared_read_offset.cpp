#include <faasm/faasm.h>
#include <string>

int main(int argc, char* argv[])
{
    const char* key = "state_shared_offset_example";
    uint8_t* actual = faasmReadStateOffsetPtr(key, 7, 2, 3);

    // Check that state has expected value
    uint8_t expected[7] = { 6, 7, 8 };
    bool valuesMatch = true;
    for (int i = 0; i < 3; i++) {
        if (actual[i] != expected[i]) {
            valuesMatch = false;
            break;
        }
    }

    std::string successStr = "success";
    if (!valuesMatch) {
        successStr = "failure";
    }
    faasmSetOutput(successStr.c_str(), successStr.size());

    return 0;
}
