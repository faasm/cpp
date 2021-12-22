#include <cstdio>
#include <omp.h>

#include <faasm/shared_mem.h>

int main()
{
    int total = 0;
    int expected = 12;

    omp_set_max_active_levels(2);

    FAASM_SHARED_VAR(total, FAASM_TYPE_INT)
#pragma omp parallel num_threads(2) default(none) shared(total)
    {
#pragma omp critical
        {
            total += 1;
        }

        FAASM_SHARED_VAR(total, FAASM_TYPE_INT)
#pragma omp parallel num_threads(5) default(none) shared(total)
        {
#pragma omp critical
            {
                total += 1;
            }
        }
    }

    if (total != expected) {
        printf("Bad nested parallel sections. Expected %d, got %d\n",
               expected,
               total);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
