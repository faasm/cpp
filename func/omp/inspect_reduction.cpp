#include <cstdio>
#include <omp.h>

int inspectAdder(int a, int b)
{
#ifdef __wasm__
    printf("A = %i B=%i\n", &a, &b);
#else
    printf("A = %p B=%p\n", &a, &b);
#endif
    return a + b;
}

#pragma omp declare reduction(inspectAdd:int                                   \
                              : omp_out = inspectAdder(omp_out, omp_in))       \
  initializer(omp_priv = 0)

int main()
{
    int x = 0;
    int y = 0;

#ifdef __wasm__
    printf("Result before: %i (%i) %i (%i)\n", x, &x, y, &y);
#else
    printf("Result before: %i (%p) %i (%p)\n", x, &x, y, &y);
#endif

#pragma omp parallel num_threads(10) reduction(inspectAdd : x, y)
    {
        x += 1;
        y += 2;
    }

    if (x != 10) {
        printf("Reduction x result unexpected: %i\n", x);
        return 1;
    }
    if (y != 20) {
        printf("Reduction y result unexpected: %i\n", y);
        return 1;
    }

#ifdef __wasm__
    printf("Result after: %i (%i) %i (%i)\n", x, &x, y, &y);
#else
    printf("Result after: %i (%p) %i (%p)\n", x, &x, y, &y);
#endif

    return 0;
}
