#ifndef _X_FFMPEG_DELAY_CALCULATOR_H_
#define _X_FFMPEG_DELAY_CALCULATOR_H_

#include "x_ffmpeg_common.h"

namespace x_ffmpeg
{
    class DelayCalculator
    {
    private:
        double clock_start_time_;
    public:
        void Initialize();
        int64_t Calculator(double _now_time);
    public:
        DelayCalculator();
        ~DelayCalculator();
    };
}

#endif //_X_FFMPEG_DELAY_CALCULATOR_H_