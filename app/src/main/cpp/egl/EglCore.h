//
// Created by nicky on 2019/3/14.
//

#ifndef NATIVECPPAPP_EGLCORE_H
#define NATIVECPPAPP_EGLCORE_H

#include <EGL/egl.h>
#include "../common/constructormagic.h"

#define FLAG_RECORDABLE 0x01
#define FLAG_TRY_GLES2 0x02
#define FLAG_TRY_GLES3 0x04

class EglCore {

public:
    EGLDisplay mEglDisplay;
    EGLSurface mEglSurface;
    EGLContext mEglContext;

    int mEglVersion = -1;
    EGLConfig mEGLConfig;

public:
    EglCore();
    ~EglCore();

    int initEgl(EGLNativeWindowType window, int flags);
    int swapBuffers();
    void destoryEgl();

    void logThisEglState();

    static void logCurrentEglState();




private:
    EGLConfig getConfig(int flags, int version);

private:
    DISALLOW_EVIL_CONSTRUCTORS(EglCore);
};


#endif //NATIVECPPAPP_EGLCORE_H
