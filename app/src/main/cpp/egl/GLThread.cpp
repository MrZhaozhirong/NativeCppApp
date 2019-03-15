//
// Created by nicky on 2019/3/15.
//

#include <pthread.h>
#include "GLThread.h"
#include "../common/zzr_common.h"
#include <unistd.h>

GLThread::GLThread() {
    isExit = false;
    isCreate = false;
    isChange = false;
    isStart = false;
}

GLThread::~GLThread() { }


void *glThreadImpl(void *context)
{
    GLThread *glThread = static_cast<GLThread *>(context);
    glThread->isExit = false;

    while(true)
    {
        if (glThread->isExit)
        {
            LOGI("GLThread onDestroy.");
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
            LOGI("GLThread onCreate.");
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
            LOGI("GLThread onChange.");
            if(glThread->mRender!=NULL)
            {
                glThread->mRender->surfaceChanged(glThread->width,glThread->height);
            }
        }
        //onDraw
        if(glThread->isStart)
        {
            LOGI("GLThread onDraw.");
            glThread->mRender->renderOnDraw();
        }
    }
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

}