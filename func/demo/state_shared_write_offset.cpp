#include <faasm/faasm.h>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    const char* key = "state_shared_offset_example";

    // Write an empty value
    std::vector<uint8_t> bytes = { 0, 0, 0, 0, 0, 0, 0 };
    unsigned long bytesLen = bytes.size();
    faasmWriteState(key, bytes.data(), bytesLen);

    uint8_t* chunkA = faasmReadStateOffsetPtr(key, bytesLen, 2, 3);
    uint8_t* chunkB = faasmReadStateOffsetPtr(key, bytesLen, 2, 3);

    // Write to memory only
    chunkA[0] = 6;
    chunkA[1] = 7;
    chunkA[2] = 8;

    // Check both chunks see update
    bool success = chunkA == chunkB;
    success &= chunkB[0] == 6;
    success &= chunkB[1] == 7;
    success &= chunkB[2] == 8;

    std::string successStr = "success";
    if (!success) {
        successStr = "failure";
    }
    faasmSetOutput(successStr.c_str(), successStr.size());

    return 0;
}
