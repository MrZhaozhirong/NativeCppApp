//
// Created by nicky on 2019/3/15.
//

#ifndef NATIVECPPAPP_GLTHREAD_H
#define NATIVECPPAPP_GLTHREAD_H

#include <android/native_window.h>
#include <sys/types.h>
#include "../common/constructormagic.h"
#include "GLRender.hpp"

class GLThread {

public:
    pthread_t mThreadImpl;
    bool isExit;
    bool isCreate;
    bool isChange;
    bool isStart;

    ANativeWindow* window = NULL;
    int width;
    int height;
    GLRender * mRender;

public:
    GLThread();
    ~GLThread();

    void onSurfaceCreate(ANativeWindow* window);
    void onSurfaceChange(int w, int h);
    void onSurfaceDestroy();
    void release();
    //void notify();
    void setGLRender(GLRender * render);


private:
    DISALLOW_EVIL_CONSTRUCTORS(GLThread);
};

#endif //NATIVECPPAPP_GLTHREAD_H
