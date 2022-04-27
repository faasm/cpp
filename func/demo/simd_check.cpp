#include <cstring>
#include <iostream>

#define ARRAY_SIZE 1024

int main()
{
    int a[ARRAY_SIZE];
    int b[ARRAY_SIZE];
    int c[ARRAY_SIZE];

    ::memset(a, 10, ARRAY_SIZE);
    ::memset(b, 20, ARRAY_SIZE);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        c[i] = a[i] * b[i];
    }

    int expected = 10 * 20;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int actual = c[i];

        if (actual != expected) {
            printf(
              "Unexpected multiplication result: %i != %i\n", actual, expected);
            return 1;
        }
    }

    return 0;
}
