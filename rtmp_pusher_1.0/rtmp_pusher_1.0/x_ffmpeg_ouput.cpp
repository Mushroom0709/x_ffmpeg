#include "x_ffmpeg_ouput.h"

namespace x_ffmpeg
{
		bool Output::copy_to_new_stream(AVStream* _input_stream)
		{
			int ret = 0;
			AVCodec* codec = avcodec_find_encoder(_input_stream->codecpar->codec_id);
			if (codec == NULL)
				return false;

			AVStream* out_stream = avformat_new_stream(fmt_ctx_, codec);
			if (out_stream == NULL)
				return false;

			out_stream->duration = _input_stream->duration;

			AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
			ret = avcodec_parameters_to_context(codec_ctx, _input_stream->codecpar);
			if (ret < 0)
				return false;

			codec_ctx->codec_tag = 0;

			if (fmt_ctx_->oformat->flags & AVFMT_GLOBALHEADER)
				codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

			ret = avcodec_parameters_from_context(out_stream->codecpar, codec_ctx);
			if (ret < 0)
				return false;

			return true;
		}

		bool Output::Initialize(const char* _url, const char* _format, Demultiplexer& _input)
		{
			int ret = 0;
			url_ = _url;
			ret = avformat_alloc_output_context2(&fmt_ctx_, NULL, _format, _url);
			if (fmt_ctx_ == NULL)
				return false;

			fmt_ = fmt_ctx_->oformat;

			copy_to_new_stream(_input.FormatContext()->streams[_input.VideoIndexs()[0]]);
			copy_to_new_stream(_input.FormatContext()->streams[_input.AudioIndexs()[0]]);

			av_dump_format(fmt_ctx_, 0, NULL, 1);

			if (!(fmt_ctx_->flags & AVFMT_NOFILE))
			{
				ret = avio_open(&(fmt_ctx_->pb), _url, AVIO_FLAG_WRITE);
				if (ret < 0)
				{
					//av_strerror();
					return false;
				}
			}

			fmt_ctx_->duration = _input.FormatContext()->duration;

			return true;
		}
		bool Output::WriteHeader()
		{
			AVDictionary* opts = NULL;
			av_dict_set(&opts, "flvflags", "no_duration_filesize", 0);
			int ret = avformat_write_header(fmt_ctx_, &opts);
			av_dict_free(&opts);
			if (ret < 0)
				return false;
			return true;
		}
		bool Output::WriteTrailer()
		{
			int ret = av_write_trailer(fmt_ctx_);
			if (ret < 0)
				return false;
			return true;
		}
		bool Output::WriteFrame(AVPacket* _pkt)
		{
			int ret = av_interleaved_write_frame(fmt_ctx_, _pkt);
			if (ret < 0)
				return false;
			return true;
		}
		void Output::Destroy()
		{
			if (fmt_ctx_ != NULL)
			{
				if (!(fmt_ctx_->flags & AVFMT_NOFILE))
				{
					avio_close(fmt_ctx_->pb);
				}

				avformat_free_context(fmt_ctx_);
				fmt_ctx_ = NULL;
				fmt_ = NULL;
			}
		}

		Output::Output()
		{
			fmt_ctx_ = NULL;
			fmt_ = NULL;
		}
		Output::~Output()
		{
			Destroy();
		}
}