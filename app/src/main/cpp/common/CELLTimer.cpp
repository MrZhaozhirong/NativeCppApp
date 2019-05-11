//
// Created by nicky on 2019/4/18.
//

#include "CELLTimer.h"
#include "zzr_common.h"

#include <sys/select.h>
#include <pthread.h>

namespace CELL {
//////////////////////////public methods//////////////////////////
    CELLTimer::CELLTimer() :
            m_second(0), m_microsecond(0) {
        thread_timer = -1;
    }

    CELLTimer::CELLTimer(long second, long microsecond) :
            m_second(second), m_microsecond(microsecond) {
        thread_timer = -1;
    }

    CELLTimer::~CELLTimer() {
    }

    void CELLTimer::setTimer(long second, long microsecond,
                             CELLTimerHandler *handler) {
        m_second = second;
        m_microsecond = microsecond;
        m_handler = handler;
    }

    void CELLTimer::startTimer() {
        if (thread_timer == -1)
            pthread_create(&thread_timer, NULL, OnTimer_stub, this);
    }

    void CELLTimer::stopTimer() {
        isStop = true;
        //pthread_join(thread_timer, NULL); //wait the thread stopped
        thread_timer = -1;
    }

//////////////////////////private methods//////////////////////////
    void CELLTimer::thread_proc() {
        while (!isStop) {
            if (m_handler != NULL)
                m_handler->handlerCallback();
            if (isStop) break;
            struct timeval tempval;
            tempval.tv_sec = m_second;
            tempval.tv_usec = m_microsecond;
            select(0, NULL, NULL, NULL, &tempval);
        }
        //LOGD("CELLTimer--thread_proc exit ... ");
    }

}