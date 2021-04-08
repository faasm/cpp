#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int main(int argc, char* argv[])
{
    size_t memLen = 196608;

    // Map some memory
    void* memPtr =
      mmap(nullptr, memLen, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (memPtr == MAP_FAILED) {
        printf("mmap failed\n");
        return 1;
    }

    // Write something to it
    strcpy((char*)memPtr, "Hi there!");
    auto actual = (const char*)memPtr;
    if (strcmp(actual, "Hi there!") != 0) {
        printf("ERROR - not written to mmapped region as expected\n");
        return 1;
    }

    // Now unmap that memory
    int res = munmap(memPtr, memLen);
    if (res != 0) {
        printf("munmap failed\n");
        return 1;
    }

    printf("mmap/ munmap succeeded\n");

    return 0;
}
