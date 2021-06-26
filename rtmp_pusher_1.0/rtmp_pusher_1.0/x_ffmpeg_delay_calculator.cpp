#include "x_ffmpeg_delay_calculator.h"

namespace x_ffmpeg
{

        void DelayCalculator::Initialize()
        {
            clock_start_time_ = av_gettime_relative() / AV_TIME_BASE * 1.0;
        }
        int64_t DelayCalculator::Calculator(double _now_time)
        {
            int64_t delay = 0.0;
            double clock_difference = (av_gettime_relative() / AV_TIME_BASE * 1.0) - clock_start_time_;

            //printf("%lf\t%lf\t%lf\n", _now_time, clock_difference, clock_difference- _now_time);

            if (_now_time <= clock_difference + 0.1)
                return 0;
            else
            {
                return static_cast<int64_t>((_now_time - (clock_difference + 0.1)) * AV_TIME_BASE);
            }
        }

        DelayCalculator::DelayCalculator()
        {
            clock_start_time_ = 0.0;
        }
        DelayCalculator::~DelayCalculator()
        {
            //
        }
}