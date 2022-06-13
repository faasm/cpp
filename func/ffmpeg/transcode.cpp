extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>
}

// TODO - make this eventually an input parameter
#define INPUT_FILE_PATH "file:faasm://ffmpeg/sample_video.mp4"
#define FILTER_DESC "scale=78:24,transpose=cclock"

/**
 * Apply a color correction filter to a set of frames. Adapted from one of the
 * examples in the FFmpeg repo:
 * https://ffmpeg.org/doxygen/trunk/filtering_video_8c-example.html
 */
int main()
{
    // TODO - eventually split in different functions but without all the
    // static variables in the example.
    // ---------- Initialise input file ----------

    // Open the media file
    AVFormatContext* pFormatContext = avformat_alloc_context();
    int ret = avformat_open_input(&pFormatContext, INPUT_FILE_PATH, NULL, NULL);
    if (ret != 0) {
        printf("Could not open the file in path: %s\n", INPUT_FILE_PATH);
        printf("Error code: %i\n", ret);
        return -1;
    }

    // Get information on the input streams
    ret = avformat_find_stream_info(pFormatContext,  NULL);
    if (ret < 0) {
        printf("Could not get streams info\nError code: %i\n", ret);
        return -1;
    } else {
        printf("Detected %i streams\n", pFormatContext->nb_streams);
    }

    // Select the video stream
    const AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters =  NULL;
    int videoStreamIdx = -1;
    ret = av_find_best_stream(pFormatContext,
                              AVMEDIA_TYPE_VIDEO,
                              -1,
                              -1,
                              &pCodec,
                              0);
    if (ret < 0) {
        printf("Cannot find a video stream in the input file (ret. code: %i)\n",
               ret);
        return -1;
    }
    videoStreamIdx = ret;

    // Create decoding context
    AVCodecContext *codecCtx = avcodec_alloc_context3(pCodec);
    if (codecCtx == NULL) {
        printf("Error allocating codec context\n");
        return -1;
    }

    // Initialise the video decoder
    ret = avcodec_open2(codecCtx, pCodec, NULL);
    if (ret < 0) {
        printf("Error initialising the video decoder: %i\n", ret);
        return -1;
    }

    // --------------------------------------------------------
    // Initialise color correction filter
    // --------------------------------------------------------

    char args[512];
    AVFilterContext *bufferSinkCtx;
    AVFilterContext *bufferSrcCtx;
    const AVFilter *bufferSrc  = avfilter_get_by_name("buffer");
    const AVFilter *bufferSink = avfilter_get_by_name("buffersink");
    AVRational timeBase = pFormatContext->streams[videoStreamIdx]->time_base;
    enum AVPixelFormat pixelFmts[] = { AV_PIX_FMT_GRAY8, AV_PIX_FMT_NONE };

    // Allocate filter graph, inputs, and outputs
    AVFilterGraph *filterGraph = avfilter_graph_alloc();
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    if (!outputs || !inputs || !filterGraph) {
        printf("Error allocating memory for the AV filters\n");
        return -1;
    }

    // Create video source buffer: the decoded frames are inserted here
    ret = avfilter_graph_create_filter(&bufferSrcCtx, bufferSrc, "in",
                                       args, NULL, filterGraph);
    if (ret < 0) {
        printf("Error creating buffer source: %i\n", ret);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        return -1;
    }

    // Create sink: this ends the filter chain
    ret = avfilter_graph_create_filter(&bufferSinkCtx,
                                       bufferSink,
                                       "out",
                                       NULL,
                                       NULL,
                                       filterGraph);
    if (ret < 0) {
        printf("Error creating buffer sink: %i\n", ret);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        return -1;
    }

    ret = av_opt_set_int_list(bufferSinkCtx, "pix_fmts", pixelFmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        printf("Error setting output pixel format: %i\n", ret);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        return -1;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = bufferSrcCtx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = bufferSinkCtx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    ret = avfilter_graph_parse_ptr(filterGraph, FILTER_DESC, &inputs, &outputs, NULL);
    if (ret < 0) {
        printf("Error parsing filter graph from filter description: %i\n", ret);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        return -1;
    }

    ret = avfilter_graph_config(filterGraph, NULL);
    if (ret < 0) {
        printf("Error configuring filter graph: %i\n", ret);
        avfilter_inout_free(&inputs);
        avfilter_inout_free(&outputs);
        return -1;
    }

    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    printf("Finished succesfully\n");

    avcodec_free_context(&codecCtx);
    avformat_close_input(&pFormatContext);
    return 0;
}
