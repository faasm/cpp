#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <faasm/shared_mem.h>

#define THREAD_SLEEP_MICROS 100 * 1000
#define MAIN_SLEEP_MICROS 50 * 1000

double threadTime = 0;

int main(int argc, char* argv[])
{
    double mainStart = omp_get_wtime();

    FAASM_SHARED_VAR(threadTime, FAASM_TYPE_DOUBLE)

#pragma omp parallel num_threads(1) default(none) shared(threadTime)
    {
        double threadStart = omp_get_wtime();

        usleep(THREAD_SLEEP_MICROS);

        usleep(50 * 1000);

        double threadEnd = omp_get_wtime();

        threadTime = threadEnd - threadStart;
    }

    usleep(MAIN_SLEEP_MICROS);

    usleep(50 * 1000);

    double mainEnd = omp_get_wtime();

    double mainTime = mainEnd - mainStart;

    double expectedThreadTime = ((double)THREAD_SLEEP_MICROS) / (1000 * 1000);
    double expectedMainTime =
      expectedThreadTime + (((double)MAIN_SLEEP_MICROS) / (1000 * 1000));

    if (mainTime < expectedMainTime) {
        printf("Expected main time to be gte than sleep time (%f < %f)\n",
               mainTime,
               expectedMainTime);
        return 1;
    } else if (threadTime < expectedThreadTime) {
        printf("Expected thread sleep to be gte than sleep time (%f < %f)\n",
               threadTime,
               expectedThreadTime);
        return 1;
    } else {
        printf("Times as expected: %f >= %f AND %f >= %f\n",
               mainTime,
               expectedMainTime,
               threadTime,
               expectedThreadTime);
        return 0;
    }
}
