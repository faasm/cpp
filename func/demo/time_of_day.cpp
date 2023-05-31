#include "faasm/faasm.h"

#include <stdio.h>
#include <sys/time.h>

int main(int argc, char* argv[])
{
    timeval tv{};
    gettimeofday(&tv, nullptr);

    size_t strLen = 8 + sizeof(long int);
    char str[strLen];
    sprintf(str, "Seconds: %li", tv.tv_sec);

    faasmSetOutput(str, strLen);

    return 0;
}
