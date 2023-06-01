#include "faasm/faasm.h"

int main(int argc, char* argv[])
{
    const char* message = "Hello Faasm!";

    faasmSetOutput(message, 12);

    return 0;
}
