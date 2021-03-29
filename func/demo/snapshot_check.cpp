#include "faasm/faasm.h"
#include "faasm/input.h"

#include <stdio.h>
#include <string>

int globalVar = 100;

/**
 * Checks behaviour of snapshot and restore. Sets a value when in "snapshot"
 * mode, then checks that the write has been restored when in "restore" mode.
 */
int main(int argc, char* argv[])
{
    std::string mode(faasm::getStringInput(""));

    int expected = 200;
    if (mode == "snapshot") {
        globalVar = expected;
        printf("Snapshot setting to %i (%p)\n", globalVar, &globalVar);
    } else {
        if (globalVar != expected) {
            printf("Restore expected %i but was %i (%p)\n",
                   expected,
                   globalVar,
                   &globalVar);
            return 1;
        }
    }

    return 0;
}
