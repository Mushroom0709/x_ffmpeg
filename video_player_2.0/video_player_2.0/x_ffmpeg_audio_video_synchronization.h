#ifndef _X_FFMPEG_AUDIO_VIDEO_SYNCHRONIZATION_H_
#define _X_FFMPEG_AUDIO_VIDEO_SYNCHRONIZATION_H_

#include <cmath>
#include <string>

#include "x_ffmpeg_common.h"

#define X_FFMPEG_DYNAMIC_COEFFICIENT 0.0160119  // ��̬֡���㷨��ϵ�� �ⷽ�� (1+x)^6 = 1.1 ��
// �����ʱ��(ffmepgʱ��) Ϊ 6λ����ʱ�򣬿���
// ֡�ʵ���ʱ���ڱ�׼��ʱ�����ӻ�������ʱ���
// (1.1-1)��

#define X_FFMPEG_DYNAMIC_THRESHOLD 0.003        // ����Ƶͬ����̬֡�ʽ��и�Ԥ�Ķ��ߵ�ǰʱ������ֵ

#define X_FFMPEG_SKIP_FRAME -0x1001

namespace x_ffmpeg
{
	// ����Ƶͬ��ģ�飬����ƵPTSΪ��ʱ�ӣ���Ƶͬ����Ƶ
	class AudioVideoSynchronization
	{
	private:
		volatile double audio_clock_;			//��Ƶʱ�ӣ���ʱ��
		volatile double last_video_pts_;		//��һ֡����ƵPTS
		volatile double video_show_start_time_;	//��Ƶ֡��ʾ���ڵ���ʼʱ��
	public:
		AudioVideoSynchronization();
		~AudioVideoSynchronization();
	public:
		// �趨��ǰ��Ƶʱ��
		void SetAudioClock(double _pts);

		// ��ȡ������ͼ����ʾ����ʼʱ��
		void SetCycleStart();

		// �������֡�ʿ��Ƶ��ӳ�(΢��)
		int64_t ComputationalDelay(double _pts);
	};
}

#endif //_X_FFMPEG_AUDIO_VIDEO_SYNCHRONIZATION_H_