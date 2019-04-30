//
// Created by nicky on 2019/4/18.
//

#ifndef CELLTIMER_H
#define CELLTIMER_H


#include <sys/types.h>

namespace CELL {

    class CELLTimerHandler {
    public:
        virtual void handlerCallback()=0;

        virtual ~CELLTimerHandler() {};
    };

    class CELLTimer {
    private:
        long m_second, m_microsecond;

        pthread_t thread_timer;

        static void *OnTimer_stub(void *p) {
            (static_cast<CELLTimer *>(p))->thread_proc();
            return NULL;
        }

        void thread_proc();

        CELLTimerHandler *m_handler;
        bool isStop;

    public:
        CELLTimer();

        CELLTimer(long second, long microsecond);

        virtual ~CELLTimer();

        void setTimer(long second, long microsecond, CELLTimerHandler *handler);

        void startTimer();

        void stopTimer();
    };
}

#endif // CELLTIMER_H
