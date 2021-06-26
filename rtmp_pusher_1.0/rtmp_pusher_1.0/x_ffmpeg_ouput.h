#ifndef _X_FFMPEG_OUPUT_H_
#define _X_FFMPEG_OUPUT_H_

#include <string>

#include "x_ffmpeg_common.h"
#include "x_ffmpeg_demultiplexer.h"

namespace x_ffmpeg
{
	class Output
	{
	public:
		AVOutputFormat* fmt_;
		AVFormatContext* fmt_ctx_;
	private:
		std::string url_;
	private:
		bool copy_to_new_stream(AVStream* _input_stream);
	public:
		Output();
		~Output();
	public:
		bool Initialize(const char* _url, const char* _format, Demultiplexer& _input);
		bool WriteHeader();
		bool WriteTrailer();
		bool WriteFrame(AVPacket* _pkt);
		void Destroy();
	};
}

#endif //_X_FFMPEG_OUPUT_H_