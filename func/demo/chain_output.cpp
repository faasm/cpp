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

    std::string expectedB = "longer expected B";

    std::string actualA;
    char* actualABuf;
    int actualABufSize;
    unsigned int resA =
      faasmAwaitCallOutput(callIdA, &actualABuf, &actualABufSize);
    actualA.assign(actualABuf, actualABuf + actualABufSize);

    std::string actualB;
    char* actualBBuf;
    int actualBBufSize;
    unsigned int resB =
      faasmAwaitCallOutput(callIdB, &actualBBuf, &actualBBufSize);
    actualB.assign(actualBBuf, actualBBuf + actualBBufSize);

    if (resA != 0 || resB != 0) {
        printf("One or more chained calls failed: %i %i\n", resA, resB);
        return 1;
    }

    if (actualA != expectedA) {
        printf(
          "Output mismatch: %s != %s\n", actualA.c_str(), expectedA.c_str());
        return 1;
    }

    if (actualB != expectedB) {
        printf(
          "Output mismatch: %s != %s\n", actualB.c_str(), expectedB.c_str());
        return 1;
    }

    printf("Chained outputs as expected\n");

    return 0;
}
