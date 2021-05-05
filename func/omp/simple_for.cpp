#include <cstdio>
#include <faasm/compare.h>
#include <omp.h>

#define TOTAL 15

int main(int argc, char* argv[])
{
    int expected[TOTAL] = { 0,  1,  2,  3,  4,  10, 12, 14,
                            16, 18, 30, 33, 36, 39, 42 };
    int stackResults[TOTAL];
    auto heapResults = new int[TOTAL];
    bool failed = false;

#pragma omp parallel for num_threads(3) schedule(static, 5) default(none)      \
  shared(stackResults, heapResults)
    for (int i = 0; i < TOTAL; i++) {
        int val = i * (1 + omp_get_thread_num());
        stackResults[i] = val;
        heapResults[i] = val;
    }

    if (!faasm::compareArrays(heapResults, expected, TOTAL)) {
        return 1;
    }

    if (!faasm::compareArrays(stackResults, expected, TOTAL)) {
        return 1;
    }

    delete[] heapResults;
    return 0;
}
