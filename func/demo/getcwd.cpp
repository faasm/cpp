#include "faasm/faasm.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// 09/02/2022 - wasi-libc implements getcwd now, so we don't need to implement
// it ourselves in Faasm. Thus, this test is now redundant as it is essentially
// testing wasi-libc's functionality. We keep it for backwards-compatibility.
int main(int argc, char* argv[])
{
    char buf[1024];

    char* res = getcwd(buf, 1024);
    if (res != buf) {
        printf("Expected to get buf back (%p), but got (%p)\n", buf, res);
        return 1;
    }

    const char* expected = "/";
    if (strcmp(res, expected) != 0) {
        printf("Unexpected working dir (expected %s, got %s)\n", expected, res);
        return 1;
    } else {
        printf("Got expected working dir %s\n", res);
    }

    return 0;
}
