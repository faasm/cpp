extern "C"
{
#include <libavformat/avformat.h>
}

#define INPUT_FILE_PATH "file:faasm://ffmpeg/sample_video.mp4"

/**
 * Check FFmpeg support
 *
 * Example inspired from:
 * https://github.com/leandromoreira/ffmpeg-libav-tutorial/blob/master/0_hello_world.c
 */
int main()
{
    AVFormatContext* pFormatContext = avformat_alloc_context();

    int ret = avformat_open_input(&pFormatContext, INPUT_FILE_PATH, NULL, NULL);
    if (ret != 0) {
        printf("Could not open the file in path: %s\n", INPUT_FILE_PATH);
        printf("Error code: %i\n", ret);
        return -1;
    }

    printf("Format %s\nDuration %lld [us]\nBit_rate %lld\n",
           pFormatContext->iformat->name,
           pFormatContext->duration,
           pFormatContext->bit_rate);

    avformat_close_input(&pFormatContext);
    return 0;
}
