//
// Created by nicky on 2019/11/27.
//
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "../egl/GLThread.h"

GLThread* glThread = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpuimage_GpuFilterEncoder_onSurfaceCreate(JNIEnv *env, jobject instance, jobject surface)
{
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);

    glThread = new GLThread();
    glThread->setGLRender(NULL);
    glThread->onSurfaceCreate(nativeWindow);
}


extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpuimage_GpuFilterEncoder_onSurfaceChange(JNIEnv *env, jobject instance, jobject surface,
                                                           jint width, jint height)
{
    glThread->onSurfaceChange(width, height);
}


extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpuimage_GpuFilterEncoder_onSurfaceDestroy(JNIEnv *env, jobject instance, jobject surface)
{
    glThread->onSurfaceDestroy();
    glThread->release();
    glThread = NULL;
}