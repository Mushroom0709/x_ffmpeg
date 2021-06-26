#include "x_ffmpeg_audio_video_synchronization.h"

namespace x_ffmpeg
{
	AudioVideoSynchronization::AudioVideoSynchronization()
	{
		audio_clock_ = 0.0;
		last_video_pts_ = 0.0;
		video_show_start_time_ = 0.0;
	}
	AudioVideoSynchronization::~AudioVideoSynchronization()
	{

	}

	// �趨��ǰ��Ƶʱ��
	void AudioVideoSynchronization::SetAudioClock(double _pts)
	{
		audio_clock_ = _pts;
	}

	// ��ȡ������ͼ����ʾ����ʼʱ��
	void AudioVideoSynchronization::SetCycleStart()
	{
		video_show_start_time_ = av_gettime_relative() / AV_TIME_BASE * 1.0;
	}

	// �������֡�ʿ��Ƶ��ӳ�(΢��)
	int64_t AudioVideoSynchronization::ComputationalDelay(double _pts)
	{
		int64_t i64_delay = 0; // ��������ʱ��(΢��)
		double elapsed_time = 0.0;

		if (video_show_start_time_ == 0.0)
			SetCycleStart(); // ���� 0ֵ������ֵӰ���һ֡����

		double theoretical_difference = _pts - audio_clock_;	   //���㵱ǰ��Ƶ֡ʱ������ʱ�����۲�ֵ
		double delay = _pts - last_video_pts_; //���㱾֡��������Ҫ����һ֡����ʱ��֮������ʾ
		int series = std::to_string(static_cast<int64_t>(theoretical_difference * AV_TIME_BASE)).size();

		last_video_pts_ = _pts; //��¼��һ֡��PTS

		if (theoretical_difference > X_FFMPEG_DYNAMIC_THRESHOLD)
		{
			// ��� ʱ�Ӳ�ֵΪ���������ʾ��Ƶ������ǰ����ȡ��ֵ�� 
			// ��((1.0 + X_AVSYNC_DYNAMIC_COEFFICIENT)^series - 1.0)��
			// ��Ϊ����֡�ʵ���ʱʱ������������� ʵ���ӳ� = ������ʱ + ������;
			theoretical_difference = theoretical_difference * (std::pow(1.0 + X_FFMPEG_DYNAMIC_COEFFICIENT, series) - 1.0);
		}
		else if (theoretical_difference < -X_FFMPEG_DYNAMIC_THRESHOLD)
		{
			// ��� ʱ�Ӳ�ֵΪ���������ʾ��Ƶ������ǰ����ȡ��ֵ�� 
			// ��((1.0 + X_AVSYNC_DYNAMIC_COEFFICIENT)^series - 1.0)��
			// ��Ϊ����֡�ʵ���ʱʱ������������� ʵ���ӳ� = ������ʱ - |������|;
			theoretical_difference = theoretical_difference * (std::pow(1.0 + X_FFMPEG_DYNAMIC_COEFFICIENT, series) - 1.0);
		}
		// ���ˣ�������������㷨���Լ����Դ���ģ�������*0.1Ҳ��

		if (delay > 0.0 && (delay + theoretical_difference) > 0.0)
		{
			delay += theoretical_difference;

			//����ʾʱ������������ʱ�� = ������ʾ��Ƶ֡ʱ��-������ʼʱ��
			elapsed_time = av_gettime_relative() / AV_TIME_BASE * 1.0 - video_show_start_time_;

			//�����ӳ�ʱ�� = ʵ���ӳ�ʱ�� - �Ѿ�����ʱ�䡣��ת��Ϊ΢��
			i64_delay = static_cast<int64_t>((delay - elapsed_time) * AV_TIME_BASE);
		}
		else
		{
			// ��������ӳٻ���ʵ���ӳ�Ϊ����������Ҫ������֡����
			i64_delay = X_FFMPEG_SKIP_FRAME;
		}
		//printf("%lf\t%lf\n", delay, theoretical_difference);
		return i64_delay;
	}
}