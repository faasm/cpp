#include "faasm/core.h"
#include <faasm/faasm.h>
#include <string>
#include <vector>

bool doChainedCall(const std::string& name, std::vector<uint8_t> inputData)
{
    unsigned int callId =
      faasmChainNamed(name.c_str(), inputData.data(), inputData.size());

    unsigned int result = faasmAwaitCall(callId);

    return result == 0;
}

int main(int argc, char* argv[])
{
    std::vector<uint8_t> inputB1 = { 0, 1, 2 };
    std::vector<uint8_t> inputB2 = { 3, 4, 5 };
    std::vector<uint8_t> inputC1 = { 6, 7 };

    std::string nameB("chain_named_b");
    std::string nameC("chain_named_c");

    bool success = true;
    success &= doChainedCall(nameB, inputB1);
    success &= doChainedCall(nameB, inputB2);
    success &= doChainedCall(nameC, inputC1);

    if (!success) {
        return 1;
    } else {
        return 0;
    }
}
