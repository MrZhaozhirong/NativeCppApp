//
// Created by zzr on 2019/4/1.
//
#pragma once
#ifndef NATIVECPPAPP_CELLTIMECOUNTER_H
#define NATIVECPPAPP_CELLTIMECOUNTER_H

#include <sys/time.h>

namespace   CELL
{
    class TimeCounter
    {
    public:
        TimeCounter()
        {
            gettimeofday(&_start, NULL);
        }
        ~TimeCounter()
        {}

        void    update()
        {
            gettimeofday(&_start, NULL);
        }

        // 获取当前秒
        double getElapsedSecond()
        {
            return  getElapsedTimeInMicroSec() * 0.000001;
        }
        // 获取毫秒
        double getElapsedTimeInMilliSec()
        {
            return this->getElapsedTimeInMicroSec() * 0.001;
        }
        // 获取微妙
        double getElapsedTimeInMicroSec()
        {
            gettimeofday(&_end, NULL);

            double  startTimeInMicroSec = (double)_start.tv_sec * 1000 * 1000 + (double)_start.tv_usec;
            double  endTimeInMicroSec   = (double)_end.tv_sec * 1000 * 1000 + (double)_end.tv_usec;

            return  endTimeInMicroSec - startTimeInMicroSec;
        }
    protected:
        struct timeval _start, _end;
    };
}
#endif //NATIVECPPAPP_CELLTIMECOUNTER_H
