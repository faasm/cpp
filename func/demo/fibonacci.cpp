#include <faasm/input.h>
#include <string>

/**
 * Deliberately inefficient fibonacci calculation for testing CPU hogs
 */
int doFib(int n)
{
    if (n == 0) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return doFib(n - 1) + doFib(n - 2);
    }
}

int main(int argc, char* argv[])
{
    const char* inputStr = faasm::getStringInput("4");
    int fibNum = std::stoi(inputStr);

    printf("Running fibonacci with %i\n", fibNum);
    int result = doFib(fibNum);

    std::string output =
      "Fibonacci " + std::to_string(fibNum) + " = " + std::to_string(result);
    faasmSetOutput(output.c_str(), output.size());

    return 0;
}
