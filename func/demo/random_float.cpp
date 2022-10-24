#include <inttypes.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

const int MAX_NUM_THREADS = 10;
const int CACHE_LINE_SIZE = 64;

const int32_t Xa1 = 40014;
const int32_t Xa2 = 40692;

const int32_t Xm1 = 2147483563;
const int32_t Xm2 = 2147483399;

int32_t *Xcg1, *Xcg2;

double randFloatSimple(int tn)
{
    return (float)rand() / RAND_MAX;
}

double randFloatComplex(int tn)
{
    int32_t k, s1, s2, z;
    int curntg;

    curntg = CACHE_LINE_SIZE * tn;
    s1 = Xcg1[curntg];
    s2 = Xcg2[curntg];
    k = s1 / 53668;
    s1 = Xa1 * (s1 - k * 53668) - k * 12211;
    if (s1 < 0) {
        s1 += Xm1;
    }
    k = s2 / 52774;
    s2 = Xa2 * (s2 - k * 52774) - k * 3791;
    if (s2 < 0) {
        s2 += Xm2;
    }
    Xcg1[curntg] = s1;
    Xcg2[curntg] = s2;
    z = s1 - s2;
    if (z < 1) {
        z += (Xm1 - 1);
    }
    return ((double)z) / Xm1;
}

int main()
{
    Xcg1 =
      (int32_t*)::calloc(MAX_NUM_THREADS * CACHE_LINE_SIZE, sizeof(int32_t));
    Xcg2 =
      (int32_t*)::calloc(MAX_NUM_THREADS * CACHE_LINE_SIZE, sizeof(int32_t));

    for (int i = 0; i < 100; i++) {
        printf("RAND: %.2f\n", randFloatSimple(i % 10));
    }

    return 0;
}
