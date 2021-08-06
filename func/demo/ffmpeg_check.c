#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

/**
 * Check FFmpeg support
 *
 * Example inspired from:
 * https://github.com/leandromoreira/ffmpeg-libav-tutorial/blob/master/0_hello_world.c
 */
int main()
{
    AVFormatContext *pFormatContext = avformat_alloc_context();

    avformat_close_input(&pFormatContext);
    return 0;
}
