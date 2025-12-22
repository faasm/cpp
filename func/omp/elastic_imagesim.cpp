#include <faasm/faasm.h>
#include <faasm/input.h>
#include <faasm/shared_mem.h>

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <omp.h>
#include <string>
#include <unistd.h>

#define BUCKET_NAME "faasm"

std::vector<std::string> listS3Keys(const std::string& path)
{
    std::vector<std::string> s3files;
    int numKeys = __faasm_s3_get_num_keys_with_prefix(BUCKET_NAME, path.c_str());

    char **keysBuffer = (char **)malloc(numKeys * sizeof(char *));
    int *keysBufferLens = (int *)malloc(numKeys * sizeof(int32_t));

    __faasm_s3_list_keys_with_prefix(BUCKET_NAME, path.c_str(),
                                     keysBuffer, keysBufferLens);

    for (int i = 0; i < numKeys; i++) {
        std::string tmpString;
        tmpString.assign(keysBuffer[i], keysBuffer[i] + keysBufferLens[i]);
        s3files.push_back(tmpString);
    }

    return s3files;
}

/* This method performs an image similarity search with an elastic number of
 * threads
 */
bool doImageSim(const std::vector<std::string>& images, int numThreads)
{
    // Split image array into even slices.
    auto iters = 2 * numThreads;
    const std::size_t n = images.size();
    const std::size_t base = n / static_cast<std::size_t>(iters);
    const std::size_t rem  = n % static_cast<std::size_t>(iters);

    std::cout << "Hello - n: " << n << " - base: " << base << " - rem: " << rem << std::endl;

#pragma omp parallel for num_threads(numThreads)
    // We need to give the for loop room to potentially grow when numThreads is
    // internally scaled-up by the runtime. Here we hard-code to a situation
    // where we initially run with numThreads = nproc / 2 and elastically scale
    // to numThreads = nproc
    for (int i = 0; i < iters; i++) {
        usleep(2 * 1000 * 1000);

        const std::size_t ui = static_cast<std::size_t>(i);
        const std::size_t start = ui * base + std::min(ui, rem);
        const std::size_t len   = base + (ui < rem ? 1u : 0u);

        std::cout << "Thread processing " << len << " images" << std::endl;
        for (auto j = 0; j < len; j++) {
            const std::size_t idx = start + j;
            int callId = -1;
            while (callId == -1) {
                callId = faasmChainNamed("imagesim", images.at(idx).c_str(), nullptr, 0);

                if (callId == -1) {
                    printf("ERROR: executing image-similarity (no hosts?)\n");
                    usleep(1 * 1000 * 1000);
                }
            }

            faasmAwaitCall(callId);
        }
    }

    return 0;
}

void doSleep(int numThreads)
{
#pragma omp parallel for num_threads(numThreads)
    for (int i = 0; i < numThreads; i++) {
        usleep(2 * 1000 * 1000);
    }
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        printf("ERROR: usage: <program> <bucket_name> <num_threads> <iters_per_thread>\n");
        return 1;
    }
    std::string imageBucket(argv[1]);
    int numThreads = std::stoi(argv[2]);
    int itersPerThread = std::stoi(argv[3]);

    // If we pass an itersPerThread of 0, it means that we don't run the image
    // simulation workload, instead we just sleep on each thread for a while to
    // take up resources
    if (itersPerThread == 0) {
        doSleep(numThreads);
    }

    // Otherwise, we first need to download all keys in the bucket.
    auto images = listS3Keys(imageBucket);
    std::vector<std::string> imageNames;
    for (const auto& image : images) {
        auto imageName = std::filesystem::path(image).filename().string();
        __faasm_s3_download_key(BUCKET_NAME, image.c_str(), imageName.c_str());
        imageNames.push_back(imageName);
    }
    // FIXME: this images yield a runtime error in the KNN search.

    std::vector<std::string> imageNamesLocal;
    for (const auto& entry : std::filesystem::directory_iterator("data/test_embeddings/")) {
        if (entry.is_regular_file()) {
            std::cout << "adding file: " << entry.path() << std::endl;
            imageNamesLocal.push_back(entry.path());
            // std::cout << entry.path() << '\n';
        }
    }

    for (int i = 0; i < itersPerThread; i++) {
        doImageSim(imageNamesLocal, numThreads);
    }

    return 0;
}
