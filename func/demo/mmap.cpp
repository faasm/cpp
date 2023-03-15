#include "faasm/faasm.h"

#include <array>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/mman.h>

/* mmap seems to be corrupting the stack
int main(int argc, char* argv[])
{
    int a = 1;

    // Prints 1
    printf("A's value: %i\n", a);
    a += 1;

    // Prints 2
    printf("A's value: %i\n", a);

    size_t memLen = 64 * 1024;
    void* p = mmap(nullptr, memLen, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    // Prints 0
    printf("A's value: %i\n", a);
    return 0;
}
*/

/*
int main(int argc, char* argv[])
{
    int* a = (int*) malloc(sizeof(int));;
    memset((void*) a, 0, sizeof(int));

    // Prints 0
    printf("A's value: %i\n", *a);
    *a += 1;

    // Prints 1
    printf("A's value: %i\n", *a);

    size_t memLen = 64 * 1024;
    void* p = mmap(nullptr, memLen, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    // Prints 0
    printf("A's value: %i\n", *a);
    return 0;
}
*/

int main(int argc, char* argv[])
{
    // Make this multiple pages not on a page boundary
    size_t nPages = 3;
    size_t memLen = (nPages * 64 * 1024) + 1234;

    uint8_t outputBuf[1];

    char* memPtrs[10];
    std::array<std::string, 10> expected;

    for (int i = 0; i < 10; i++) {
        // Map some memory
        printf("Mapping memory %i\n", i);
        void* p = mmap(nullptr,
                       memLen,
                       PROT_READ | PROT_WRITE,
                       MAP_ANON | MAP_PRIVATE,
                       -1,
                       0);
        if (p == MAP_FAILED) {
            printf("ERROR - mmap failed: %s\n", strerror(errno));
            return 1;
        }

        memPtrs[i] = (char*)p;

        // Write something to it
        std::string output = std::string("Output ") + std::to_string(i);
        strcpy(memPtrs[i], output.c_str());

        expected[i] = output;
    }

    // Check the outputs are still in place
    for (int i = 0; i < 10; i++) {
        std::string thisExpected = expected.at(i);
        std::string thisActual(memPtrs[i]);

        if (thisExpected != thisActual) {
            printf("Expected %s but got %s\n",
                   thisExpected.c_str(),
                   thisActual.c_str());
	    printf("Check %i failure\n", i);
            return 1;
        } else {
	    printf("Check %i success\n", i);
	}
    }

    outputBuf[0] = 1;
    faasmSetOutput(outputBuf, 1);

    return 0;
}
