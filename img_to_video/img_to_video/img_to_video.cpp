// img_to_video.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
}

void print_error_code(int _code)
{
    char buf[1024] = { 0 };
    av_strerror(_code, buf, 1024);
    printf("%s\n", buf);
}

#define IMG_SIZE_WIDTH 1920
#define IMG_SIZE_HEIGHT 1080
#define IMG_FMT AV_PIX_FMT_YUV420P

#define YUV_INPUT_FILE_NAME "yuv420p.yuv"
#define H264_OUTPUT_FILE_NAME "H264.h264"

bool xfread(FILE* _fp, uint8_t* _buf, size_t _size)
{
    size_t rs = 0;
    size_t ret = 0;
    while (rs < _size)
    {
        ret == fread(_buf + rs, 1, _size - rs > 4096 ? 4096 : _size - rs, _fp);
        if (ret <= 0)
            return false;
        rs += ret;
    }
    return true;
}

int main()
{
    int ret = 0;

    FILE* input = NULL;
    FILE* output = NULL;

    AVFrame* frame = NULL;
    AVPacket* pkt = NULL;

    AVCodec* codec = NULL;
    AVCodecContext* codec_ctx = NULL;

    frame = av_frame_alloc();
    pkt = av_packet_alloc();

    frame->width = IMG_SIZE_WIDTH;
    frame->height = IMG_SIZE_HEIGHT;
    frame->format = IMG_FMT;

    ret = av_frame_get_buffer(frame, 0);
    if (ret != 0)
    {
        print_error_code(ret);
        return -1;
    }

    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (codec == NULL)
        return -1;
    codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx == NULL)
        return -1;

    codec_ctx->width = IMG_SIZE_WIDTH;
    codec_ctx->height = IMG_SIZE_HEIGHT;
    codec_ctx->pix_fmt = IMG_FMT;
    codec_ctx->time_base = { 1,25 };
    codec_ctx->thread_count = 12;

    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret != 0)
    {
        print_error_code(ret);
        return -1;
    }



    ret = fopen_s(&input, YUV_INPUT_FILE_NAME, "rb");
    if (input == NULL)
        return -1;

    ret = fopen_s(&output, H264_OUTPUT_FILE_NAME, "wb+");
    if (output == NULL)
        return -1;

    int index = 0;
    while (true)
    {
        if (1 != fread(frame->data[0], frame->linesize[0] * frame->height, 1, input))
            break;
        if (1 != fread(frame->data[1], frame->linesize[1] * frame->height / 2, 1, input))
            break;
        if (1 != fread(frame->data[2], frame->linesize[2] * frame->height / 2, 1, input))
            break;

        frame->pts = index++ * 90000 / 25;

        ret = avcodec_send_frame(codec_ctx, frame);
        if (ret != 0)
        {
            print_error_code(ret);
            return -1;
        }

        while (ret >= 0)
        {
            ret = avcodec_receive_packet(codec_ctx, pkt);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            {
                break;
            }
            else if (ret < 0)
            {
                print_error_code(ret);
                return -1;
            }
            else if (ret == 0)
            {
                printf("Write packet data:%d\n", pkt->size);
                fwrite(pkt->data, pkt->size, 1, output);
                fflush(output);
                av_packet_unref(pkt);
            }
        }
    }

    fclose(input);
    fclose(output);

    av_frame_free(&frame);
    av_packet_free(&pkt);

    avcodec_free_context(&codec_ctx);

    return 0;
}
