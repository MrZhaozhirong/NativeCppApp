//
// Created by nicky on 2019/3/15.
//

#ifndef NATIVECPPAPP_EGLSURFACEBASE_H
#define NATIVECPPAPP_EGLSURFACEBASE_H


#include "../common/constructormagic.h"
#include "EglCore.h"

class EglSurfaceBase {

public:
    EglSurfaceBase(EglCore *eglCore);
    // 创建要使用的渲染表面surface
    void createWindowSurface(ANativeWindow *nativeWindow);
    // 创建离屏的EGLSurface，不过FBO比它好多了
    // Deprecated
    void createOffScreenSurface(int width, int height);
    // 返回surface的width长度, 单位是pixels.
    int getWidth();
    // 返回surface的height长度, 单位是pixels.
    int getHeight();
    // 获取当前eglSurface对象
    EGLSurface getEglSurface();
    // 释放 EGL surface.
    void releaseEglSurface();
    // Makes our EGL context and surface current.
    void makeCurrent();
    // Makes our EGL context and surface current for drawing,
    // using the supplied surface for reading.
    void makeCurrentReadFrom(EglSurfaceBase readSurface);
    // Calls eglSwapBuffers.  Use this to "publish" the current frame.
    bool swapBuffers();
    // Sends the presentation time stamp to EGL.
    void setPresentationTime(EGLnsecsANDROID nsecs);
    // 原java版本是直接写文件，void saveFrame(File file)，
    // 然后经过思考glReadPixels是耗时操作，还是直接返回数据好了
    char * getFrame();

protected:
    EglCore* mEglCore;
    EGLSurface mEglSurface;
    int mWidth;
    int mHeight;



private:
    DISALLOW_EVIL_CONSTRUCTORS(EglSurfaceBase);
};


#endif //NATIVECPPAPP_EGLSURFACEBASE_H
