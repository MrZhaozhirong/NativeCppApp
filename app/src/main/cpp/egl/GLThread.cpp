//
// Created by nicky on 2019/3/15.
//

#include <pthread.h>
#include "GLThread.h"
#include "../common/zzr_common.h"
#include "../common/CELLTimeCounter.hpp"
#include <unistd.h>

GLThread::GLThread() {
    isExit = false;
    isCreate = false;
    isChange = false;
    isStart = false;
    window = NULL;
}

GLThread::~GLThread() { }


void *glThreadImpl(void *context)
{
    GLThread *glThread = static_cast<GLThread *>(context);
    glThread->isExit = false;

    CELL::TimeCounter tm;
    while(true)
    {
        if (glThread->isExit)
        {
            LOGD("GLThread onDestroy.");
            if(glThread->mRender!=NULL)
            {
                glThread->mRender->surfaceDestroyed();
            }
            break;
        }
        usleep(16000);  // 1s/0.016s = 62.5 fps
        //onCreate
        if (glThread->isCreate)
        {
            glThread->isCreate = false;
            LOGD("GLThread onCreate.");
            if(glThread->mRender!=NULL)
            {
                glThread->mRender->surfaceCreated(glThread->window);
            }
        }
        //onChange
        if(glThread->isChange)
        {
            glThread->isChange = false;
            glThread->isStart = true;
            LOGD("GLThread onChange.");
            if(glThread->mRender!=NULL)
            {
                glThread->mRender->surfaceChanged(glThread->width,glThread->height);
            }
        }
        //onDraw
        double  second  =   tm.getElapsedTimeInMilliSec();
        if(glThread->isStart)
        {
            //LOGD("GLThread onDraw.");
            glThread->mRender->renderOnDraw(second);
        }
        tm.update();
    }
    LOGD("GLThread exist.");
    return 0;
}


void GLThread::onSurfaceCreate(ANativeWindow* window)
{
    this->window = window;
    this->isCreate = true;
    pthread_create(&mThreadImpl, NULL, glThreadImpl, this);
}

void GLThread::onSurfaceChange(int w, int h)
{
    this->width = w;
    this->height = h;
    this->isChange = true;
}

void GLThread::onSurfaceDestroy() {
    this->isExit = true;
}

void GLThread::setGLRender(GLRender * render) {
    this->mRender = render;
}

void GLThread::release() {
    void* reval;
    pthread_join(mThreadImpl, &reval);
    LOGD("GLThread release %d",(int)reval);
}