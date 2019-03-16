#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "../egl/GLThread.h"
#include "NativeGLRender.h"


GLThread *glThread = NULL;
NativeGLRender* testRender = NULL;


extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_onSurfaceCreate(JNIEnv *env, jobject instance, jobject surface)
{
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
    glThread = new GLThread();
    testRender = new NativeGLRender();
    glThread->setGLRender(testRender);
    glThread->onSurfaceCreate(nativeWindow);

}


extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_onSurfaceChange(JNIEnv *env, jobject instance,
                                                     jint width, jint height)
{

    glThread->onSurfaceChange(width, height);
}


extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_onSurfaceDestroy(JNIEnv *env, jobject instance)
{
    glThread->setGLRender(NULL);
    glThread->onSurfaceDestroy();
    glThread->release();

    delete testRender;
    delete glThread;
}
