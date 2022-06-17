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
    std::vector<uint8_t> input = { 0, 1, 2 };

    std::string name("noop");

    bool success = true;
    success &= doChainedCall(nameB, input);

    if (!success) {
        return 1;
    } else {
        return 0;
    }
}
