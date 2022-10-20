#include <faasm/faasm.h>
#include <string>

int main(int argc, char* argv[])
{
    // Read the results from input, as they may contain spaces
    char inputBuf[512];
    faasmGetInput(reinterpret_cast<uint8_t*>(inputBuf), 512);

    std::string inputStr(inputBuf, strlen(inputBuf));
    printf("Got input: %s\n", inputStr.c_str());
}
