#include <cstdio>
#include <omp.h>

struct complex_t
{
    int real;
    int imag;
};

complex_t complex_add(complex_t a, complex_t b)
{
    complex_t c;
    c.real = a.real + b.real;
    c.imag = a.imag + b.imag;
    return c;
}

#pragma omp declare reduction(cmplxAdd:complex_t                               \
                              : omp_out = complex_add(omp_out, omp_in))        \
  initializer(omp_priv = { 0, 0 })

int main()
{
    complex_t x = { 0, 0 };

#pragma omp parallel num_threads(10) reduction(cmplxAdd : x)
    {
        x = (complex_t){ 1, -1 };
    }

    if (x.real != 10 || x.imag != -10) {
        printf("Reduction result unexpected: %i %ii\n", x.real, x.imag);
        return 1;
    }

    printf("Reduction result: %i %ii\n", x.real, x.imag);

    return 0;
}
