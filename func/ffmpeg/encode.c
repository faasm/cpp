/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <faasm/faasm.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile)
{
    int ret;

    /* send the frame to the encoder */
    if (frame)
        printf("Send frame %3"PRId64"\n", frame->pts);

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

        printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}

static void doChainedCalls(int num_files, const char* filename_base)
{
    char filename[80];
    char filename_out[80];
    char args[256];

    // First, chain to all the filtering functions
    int nf = 0;
    for (; nf < num_files / 2; nf++) {
        // For some reason we need to add the extra 'file:' prefix when calling
        // the filtering function
        sprintf(filename, "file:%s_%i.mpeg", filename_base, nf);
        sprintf(filename_out, "file:%s_%i_out.mpeg", filename_base, nf);
        sprintf(args, "%s %s", filename, filename_out);
        unsigned int call_id = faasmChainNamed("filter", (const uint8_t*) args, strlen(args));
    }

    // Second, chain to all the image recognition functions
    // TODO - for the moment this function also does filtering. Update if time
    for (; nf < num_files; nf++) {
        // For some reason we need to add the extra 'file:' prefix when calling
        // the filtering function
        sprintf(filename, "file:%s_%i.mpeg", filename_base, nf);
        sprintf(filename_out, "file:%s_%i_out.mpeg", filename_base, nf);
        sprintf(args, "%s %s", filename, filename_out);
        unsigned int call_id = faasmChainNamed("recognition", (const uint8_t*) args, strlen(args));
    }

}

int main(int argc, char **argv)
{
    const char *codec_name;
    char filename[80];
    const AVCodec *codec;
    int i, ret, x, y;
    FILE *f;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    if (argc <= 2) {
        fprintf(stderr, "Usage: %s <num_files> <length_in_secs> <output file base>\n", argv[0]);
        exit(0);
    }
    int num_files = atoi(argv[1]);
    int videoLengthSecs = atoi(argv[2]);
    char* filename_base = argv[3];
    codec_name = "mpeg4";

    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name(codec_name);
    if (!codec) {
        fprintf(stderr, "Codec '%s' not found\n", codec_name);
        exit(1);
    }

    for (int nf = 0; nf < num_files; nf++) {
        AVCodecContext *c= NULL;
        AVFrame *frame;
        AVPacket *pkt;

        printf("Processing file %i/%i\n", nf + 1, num_files);
        sprintf(filename, "%s_%i.mpeg", filename_base, nf);
        printf("Filename: %s\n", filename);

        c = avcodec_alloc_context3(codec);
        if (!c) {
            fprintf(stderr, "Could not allocate video codec context\n");
            exit(1);
        }

        /* put sample parameters */
        c->bit_rate = 400000;
        /* resolution must be a multiple of two */
        c->width = 352;
        c->height = 288;
        /* frames per second */
        c->time_base = (AVRational){1, 25};
        c->framerate = (AVRational){25, 1};

        /* emit one intra frame every ten frames
         * check frame pict_type before passing frame
         * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
         * then gop_size is ignored and the output of encoder
         * will always be I frame irrespective to gop_size
         */
        c->gop_size = 10;
        c->max_b_frames = 1;
        c->pix_fmt = AV_PIX_FMT_YUV420P;

        if (codec->id == AV_CODEC_ID_H264)
            av_opt_set(c->priv_data, "preset", "slow", 0);

        /* open it */
        ret = avcodec_open2(c, codec, NULL);
        if (ret < 0) {
            fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
            exit(1);
        }

        f = fopen(filename, "wb+");
        if (!f) {
            fprintf(stderr, "Could not open %s\n", filename);
            exit(1);
        }

        pkt = av_packet_alloc();
        if (!pkt) {
            fprintf(stderr, "Could not allocate packet\n");
            exit(1);
        }

        frame = av_frame_alloc();
        if (!frame) {
            fprintf(stderr, "Could not allocate video frame\n");
            exit(1);
        }
        frame->format = c->pix_fmt;
        frame->width  = c->width;
        frame->height = c->height;

        ret = av_frame_get_buffer(frame, 0);
        if (ret < 0) {
            fprintf(stderr, "Could not allocate the video frame data\n");
            exit(1);
        }

        for (int i = 0; i < 25 * videoLengthSecs; i++) {
            printf("Generating frame %i of %i\n", i+1, 25 * videoLengthSecs);

            fflush(stdout);

            /* make sure the frame data is writable */
            ret = av_frame_make_writable(frame);
            if (ret < 0)
                exit(1);

            /* prepare a dummy image */
            /* Y */
            for (y = 0; y < c->height; y++) {
                for (x = 0; x < c->width; x++) {
                    frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
                }
            }

            /* Cb and Cr */
            for (y = 0; y < c->height/2; y++) {
                for (x = 0; x < c->width/2; x++) {
                    frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                    frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
                }
            }

            frame->pts = i;

            /* encode the image */
            encode(c, frame, pkt, f);
        }

        /* flush the encoder */
        encode(c, NULL, pkt, f);

        /* add sequence end code to have a real MPEG file */
        if (codec->id == AV_CODEC_ID_MPEG1VIDEO || codec->id == AV_CODEC_ID_MPEG2VIDEO)
            fwrite(endcode, 1, sizeof(endcode), f);
        fclose(f);

        av_frame_free(&frame);
        av_packet_free(&pkt);
        avcodec_free_context(&c);
    }

    doChainedCalls(num_files, filename_base);

    return 0;
}
