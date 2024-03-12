#include <faasm/faasm.h>
#include <pthread.h>
#include <stdio.h>

int globalVar = 10;

#define SUCCESS (void*)100
#define FAILURE (void*)101

// Effects of zygote should be seen by main function by default
FAASM_ZYGOTE()
{
    globalVar = 20;

    return 0;
}

// Thread should see global changes made in the main thread
void* threadFunc(void* arg)
{
    printf("Thread global var %i (%p)\n", globalVar, &globalVar);

    if (globalVar != 30) {
        printf("Expected thread to see 30 but got %i\n", globalVar);
        return FAILURE;
    } else {
        printf("Threads sees %i as expected\n", globalVar);
    }

    return SUCCESS;
}

int main(int argc, char* argv[])
{
    if (globalVar != 20) {
        printf("Expected main thread to see 20 but got %i\n", globalVar);
        return 1;
    }

    // Update the global var, this should be captured in the snapshot
    globalVar = 30;
    printf("Main global var %i (%p)\n", globalVar, &globalVar);

    // Run a thread
    pthread_t t;
    int ret = pthread_create(&t, NULL, threadFunc, nullptr);
    if (ret != 0) {
        printf("Error creating thread (%i)\n", ret);
        return 1;
    }

    // Join the thread
    int* res;
    if (pthread_join(t, (void**)&res)) {
        printf("Error joining thread\n");
        return 1;
    }

    // Check result of the thread
    if (res != SUCCESS) {
        printf("Thread execution failed\n");
        return 1;
    }

    return 0;
}
