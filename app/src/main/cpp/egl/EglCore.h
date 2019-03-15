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

// 参考android-26/android/opengl/EGLxt.java中的定义
#define EGL_OPENGL_ES3_BIT_KHR 0x0040
#define EGL_RECORDABLE_ANDROID 0x3142
// 参考android-26/android/opengl/EGLxt.java中的定义
// egl.h没有 eglPresentationTimeANDROID 的接口，
// 所以只能自己定义函数指针，并通过eglGetProcAddress动态获取其函数地址了
// 使用前记得判断是否为空
typedef EGLBoolean (* EGL_PRESENTATION_TIME_ANDROID_PROC)(EGLDisplay display, EGLSurface surface, khronos_stime_nanoseconds_t time);


class EglCore {

public:
    EGLDisplay mEglDisplay;
    EGLContext mEglContext;

    int mEglVersion = -1;
    EGLConfig mEglConfig;

public:
    EglCore();
    ~EglCore();
    EglCore(EGLContext sharedContext, int flags);


    int initEgl(EGLContext sharedContext, int flags);
    void release();
    //  创建EGLSurface
    EGLSurface createWindowSurface(ANativeWindow * surface);
    // 创建离屏Surface
    EGLSurface createOffscreenSurface(int width, int height);
    // 查询当前surface的状态值。
    int querySurface(EGLSurface eglSurface, int what);
    // 切换到当前上下文
    void makeCurrent(EGLSurface eglSurface);
    // 切换到某个上下文
    void makeCurrent(EGLSurface drawSurface, EGLSurface readSurface);
    // Makes no context current.
    void makeNothingCurrent();
    // 交换缓冲区显示
    bool swapBuffers(EGLSurface eglSurface);
    //判断当前的EGLContext 和 EGLSurface是否同一个EGL
    bool isCurrent(EGLSurface eglSurface);
    // Destroys the specified surface.
    // Note the EGLSurface won't actually be destroyed if it's still current in a context.
    void releaseSurface(EGLSurface eglSurface);
    // 设置pts
    void setPresentationTime(EGLSurface eglSurface, long nsecs);


    static void logCurrentEglState();
    // 动态 设置pts方法
    EGL_PRESENTATION_TIME_ANDROID_PROC eglPresentationTimeANDROID = NULL;
private:
    EGLConfig getConfig(int flags, int version);
    void checkEglError(const char *msg);

private:
    DISALLOW_EVIL_CONSTRUCTORS(EglCore);
};


#endif //NATIVECPPAPP_EGLCORE_H
