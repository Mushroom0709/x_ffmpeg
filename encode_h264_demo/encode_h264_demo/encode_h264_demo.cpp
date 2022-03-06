// encode_h264_demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

std::string av_error_code_2_string(int err_code)
{
#define ERROR_CODE_2_STRING_MAX_LENGTH 128
    char buffer[ERROR_CODE_2_STRING_MAX_LENGTH] = { 0 };
    av_strerror(err_code, buffer, ERROR_CODE_2_STRING_MAX_LENGTH);
    return std::string(buffer);
}

int main()
{
    std::string filename = "400_300_25.h265";
    int ret = 0;

    FILE* fp = fopen(filename.c_str(), "wb");
    if (fp == NULL)
        return -1;

    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H265);
    if (codec == NULL)
        return -1;

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx == NULL)
        return -1;

    codec_ctx->width = 400;
    codec_ctx->height = 300;
    codec_ctx->time_base = { 1,25 };
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    codec_ctx->thread_count = 16;


    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret != 0)
    {
        std::cout << av_error_code_2_string(ret) << std::endl;
        return -1;
    }

    AVPacket* pkt = av_packet_alloc();

    AVFrame* frame = av_frame_alloc();
    frame->width = codec_ctx->width;
    frame->height = codec_ctx->height;
    frame->format = codec_ctx->pix_fmt;
    ret = av_frame_get_buffer(frame, 0);
    if (ret != 0)
    {
        std::cout << av_error_code_2_string(ret) << std::endl;
        return -1;
    }


    for (size_t i = 0; i < 250; i++)
    {
        for (size_t y = 0; y < codec_ctx->height; y++)
        {
            for (size_t x = 0; x < codec_ctx->width; x++)
            {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }

        for (size_t y = 0; y < codec_ctx->height / 2; y++)
        {
            for (size_t x = 0; x < codec_ctx->width / 2; x++)
            {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }

        frame->pts = i;

        ret = avcodec_send_frame(codec_ctx, frame);
        if (ret != 0)
        {
            std::cout << av_error_code_2_string(ret) << std::endl;
            break;
        }

        while (ret >= 0)
        {
            ret = avcodec_receive_packet(codec_ctx, pkt);
            if (ret < 0)
            {
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                {
                    break;
                }
                else
                {
                    std::cout << av_error_code_2_string(ret) << std::endl;
                    return -1;
                }
            }
            std::cout << "packet size:" << pkt->size << std::endl;
            fwrite((char*)pkt->data, pkt->size, 1, fp);
            fflush(fp);
            av_packet_unref(pkt);
        }
    }

    fclose(fp);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    return 0;
}
