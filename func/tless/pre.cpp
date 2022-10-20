#include "faasm/core.h"
#include <faasm/faasm.h>
#include <string>
#include <vector>

bool doChainedCall(const std::string& name, std::string& cmdLine)
{
    unsigned int callId =
      faasmChainNamed(name.c_str(),
                      reinterpret_cast<const uint8_t*>(cmdLine.c_str()),
                      cmdLine.size());

    unsigned int result = faasmAwaitCall(callId);

    return result == 0;
}

int main(int argc, char* argv[])
{
    int numInferenceRounds = 10;

    if (argc == 2) {
        numInferenceRounds = atoi(argv[1]);
    } else {
        printf("Error: need to provide the scale of the problem\n");
        printf("usage: pre <scale>\n");
        return 1;
    }

    printf("\n---------------------------------------------------------------\n");
    printf("----------- Running TLess image processing pipeline -----------\n");
    printf("---------------------------------------------------------------\n\n");

    char cmdBuf[512];
    size_t bufSize = 512;
    int imgId = 1;

    // Prepare ImageMagick command
    std::string imageMagickFunc("imagemagick");
    sprintf(cmdBuf,
            "faasm://tless/sample_image_%i.png -flip faasm://tless/image_out_%i.png",
            imgId,
            imgId);
    std::string imCmd = std::string(cmdBuf, strlen(cmdBuf));

    // Prepare ImageMagick command 2
    memset(cmdBuf, '\0', bufSize);
    sprintf(cmdBuf,
            "faasm://tless/sample_image_%i.png faasm://tless/image_out_%i.bmp",
            imgId,
            imgId);
    std::string imCmdConvert = std::string(cmdBuf, strlen(cmdBuf));

    // Prepare TensorFlow inference command
    // TODO: TF inference doesn't use the provided image (would have to run
    // the convert command beforehand, something is failing)
    memset(cmdBuf, '\0', bufSize);
    std::string inferenceFunc("inference");
    sprintf(cmdBuf, "faasm://tless/sample_image_%i.bmp %i", imgId, numInferenceRounds);
    std::string tfCmd = std::string(cmdBuf, strlen(cmdBuf));

    // Run commands
    bool success = true;

    // Top branch
    success &= doChainedCall(imageMagickFunc, imCmd);

    // Bottom branch
    success &= doChainedCall(inferenceFunc, tfCmd);

    if (!success) {
        printf("Error processing image!\n");
        return 1;
    }

    return 0;

}
