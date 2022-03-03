#include <cstdio>
#include <omp.h>
#include <random>

#define CHUNK_SIZE 1000000

static std::random_device rd;
static std::mt19937 rng(rd());

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Specify number of workers\n");
        return 1;
    }

    int nWorkers = std::stoi(argv[1]);
    long result = 0;
    int nTotal = nWorkers * CHUNK_SIZE;
    std::uniform_real_distribution<float> uni(0, 1);

#pragma omp parallel for num_threads(nWorkers) default(none) shared(rng, uni) \
    reduction(+ : result)
    for (int i = 0; i < CHUNK_SIZE; i++) {
        float x = uni(rng);
        float y = uni(rng);

        if (x * x + y * y <= 1.0) {
            result++;
        }
    }

    float pi = 4 * ((float)result / (nTotal));

    std::string output = "Pi estimate: " + std::to_string(pi) + "\n";
    printf("%s", output.c_str());

    return 0;
}
