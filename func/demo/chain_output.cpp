#include <faasm/faasm.h>
#include <stdio.h>

#include <string>

int otherA()
{
    std::string outputA = "expected A";
    faasmSetOutput(outputA.c_str(), outputA.size());
    return 0;
}

int otherB()
{
    std::string outputB = "longer expected B";
    faasmSetOutput(outputB.c_str(), outputB.size());
    return 0;
}

/**
 * Checks getting output from chained functions
 */
int main(int argc, char* argv[])
{
    unsigned int callIdA = faasmChain(otherA, nullptr, 0);
    unsigned int callIdB = faasmChain(otherB, nullptr, 0);

    std::string expectedA = "expected A";
    std::string actualA;
    actualA.reserve(expectedA.size());

    std::string expectedB = "longer expected B";
    std::string actualB;
    actualB.reserve(expectedB.size());

    unsigned int resA = faasmAwaitCallOutput(callIdA, actualA.c_str(), actualA.size());
    unsigned int resB = faasmAwaitCallOutput(callIdB, actualB.c_str(), actualB.size());

    if (resA != 0 || resB != 0) {
        printf("One or more chained calls failed: %i %i\n", resA, resB);
        return 1;
    }

    if (actualA != expectedA) {
        return 1;
    }

    if (actualB != expectedB) {
        return 1;
    }

    printf("Chained outputs as expected\n");

    return 0;
}
