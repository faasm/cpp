#include "faasm/faasm.h"

int main(int argc, char* argv[])
{
    const char* message = "Hello TLess!";

    faasmSetOutput((uint8_t*)message, 12);

    return 0;
}
