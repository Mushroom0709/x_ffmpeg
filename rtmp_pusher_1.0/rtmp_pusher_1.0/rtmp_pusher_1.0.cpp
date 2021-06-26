// rtmp_pusher_1.0.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <thread>
#include <chrono>

#include "x_ffmpeg_demultiplexer.h"
#include "x_ffmpeg_ouput.h"
#include "x_ffmpeg_delay_calculator.h"

#define X_ERROR -0x9999

int main(int argc, char* argv[])
{
    int ret = 0;
    int nb_stream = 0;
    AVMediaType type_ = AVMEDIA_TYPE_UNKNOWN;
    int stream_index_ = -1;
    int status_ = 0;
    AVPacket* pkt_ = av_packet_alloc();

    x_ffmpeg::Demultiplexer input_;
    x_ffmpeg::Output output_;
    x_ffmpeg::DelayCalculator delay_alculator_;

    avformat_network_init();

    if (input_.Initialize("../../videos/连名带姓-黄霄云.flv", nb_stream) == false)
        return X_ERROR;

    if(output_.Initialize("rtmp://127.0.0.1:1935/live/test","flv", input_) == false)
        return X_ERROR;

    if (output_.WriteHeader() == false)
        return X_ERROR;

    delay_alculator_.Initialize();
    while (input_.GetPacket(pkt_, type_, stream_index_, status_))
    {
        if (status_ == 0 &&
            ((type_ == AVMEDIA_TYPE_VIDEO && stream_index_ == input_.VideoIndexs()[0]) ||
                (type_ == AVMEDIA_TYPE_AUDIO && stream_index_ == input_.AudioIndexs()[0])))
        {
            if (type_ == AVMEDIA_TYPE_VIDEO)
            {
                int64_t delay = delay_alculator_.Calculator(
                    av_q2d(input_.FormatContext()->streams[stream_index_]->time_base) * pkt_->pts);

                if (delay > 0)
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(delay));
                }

                printf("[LOG] [VIDEO] [PTS:%lld]\n", pkt_->pts);
            }
            else
            {
                printf("[LOG] [AUDIO] [PTS:%lld]\n", pkt_->pts);
            }

            if (output_.WriteFrame(pkt_) == false)
                break;

            av_packet_unref(pkt_);
        }
    }

    if (output_.WriteTrailer() == false)
        return X_ERROR;

    av_packet_free(&pkt_);

    return 0;
}
