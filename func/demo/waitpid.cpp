#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    // Check that waitpid doesn't error, but also doesn't block
    pid_t somePid = 123;
    int status;

    int waitRes = waitpid(somePid, &status, WNOHANG);

    if (waitRes != somePid) {
        printf("waitpid failed: %s", strerror(errno));
        return 1;
    }

    printf("child exited with status of %d\n", WEXITSTATUS(status));

    return 0;
}
