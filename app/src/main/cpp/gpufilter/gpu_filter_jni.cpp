//
// Created by nicky on 2019/11/27.
//
#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "../egl/GLThread.h"
#include "render/GpuFilterRender.h"

GLThread* glThread = NULL;
GpuFilterRender* render = NULL;


extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpufilter_GpuFilterRender_onSurfaceCreate(JNIEnv *env, jobject instance, jobject surface)
{
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
    if( render==NULL) {
        render = new GpuFilterRender();
    }
    if( glThread==NULL) {
        glThread = new GLThread();
    }
    glThread->setGLRender(render);
    glThread->onSurfaceCreate(nativeWindow);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpufilter_GpuFilterRender_onSurfaceChange(JNIEnv *env, jobject instance, jobject surface,
                                                           jint width, jint height)
{
    glThread->onSurfaceChange(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpufilter_GpuFilterRender_onSurfaceDestroy(JNIEnv *env, jobject instance, jobject surface)
{
    glThread->onSurfaceDestroy();
    glThread->release();
    delete glThread;
    glThread = NULL;

    if( render==NULL) {
        delete render;
        render = NULL;
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpufilter_GpuFilterRender_setRotationCamera(JNIEnv *env, jobject instance,
                                                            jint rotation, jboolean flipHorizontal, jboolean flipVertical)
{
    // 注意这里flipVertical对应render->setRotationCamera.flipHorizontal
    // 注意这里flipHorizontal对应render->setRotationCamera.flipVertical
    // 因为Android的预览帧数据是横着的，仿照GPUImage的处理方式。
    if( render==NULL) {
        render = new GpuFilterRender();
    }
    render->setRotationCamera(rotation, flipVertical, flipHorizontal);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_gpufilter_GpuFilterRender_feedVideoData(JNIEnv *env, jobject instance,
                                                         jbyteArray array,jint width, jint height)
{
    jbyte* nv21_buffer = env->GetByteArrayElements(array, NULL );
    jsize array_len = env->GetArrayLength(array);
    render->feedVideoData(nv21_buffer, array_len, width, height);
    env->ReleaseByteArrayElements(array, nv21_buffer, 0);
}