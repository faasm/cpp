#include "utils.h"

#include "faabric_utils.h"
#include <faasm/emulator.h>

namespace tests {
void cleanCppTests()
{
    cleanFaabric();

    setEmulatorUser("tests");
}
}
