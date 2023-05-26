extern "C"
{
#include "faasm/host_interface.h"
#include "faasm/host_interface_test.h"
}

#include <stdlib.h>

// This function tests that exceptions can be propagated from host interface
// calls to the Faasm runtime
int main(int argc, char* argv[])
{
    __faasm_host_interface_test(HostInterfaceTest::ExceptionPropagationTest);
}
