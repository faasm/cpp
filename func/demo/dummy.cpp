#include <faasm/faasm.h>

#include <string>

int main(int argc, char* argv[])
{
    std::string output = "dummy";

    faasmSetOutput(output.c_str(), output.size());

    return 0;
}
