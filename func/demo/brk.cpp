#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    int pageSize = 65536;
    void* brkInitial = sbrk(0);

#ifdef __wasm__
    printf("Initial = %i\n", (int) brkInitial);
#endif

    void* newRegion = sbrk(pageSize);

#ifdef __wasm__
    printf("New = %i\n", (int) newRegion);
#endif

    if (brkInitial != newRegion) {
        printf("sbrk not behaving as expected\n");
        return 1;
    }

    void* newRegionB = sbrk(pageSize);
#ifdef __wasm__
    printf("New = %i\n", (int) newRegionB);
#endif

    int diff = ((int*)newRegionB - (int*)newRegion) * sizeof(int);
    if (diff != pageSize) {
        printf("sbrk region not size as expected (expected %i but was %i)\n",
               pageSize,
               diff);
        return 1;
    }

    return 0;
}
