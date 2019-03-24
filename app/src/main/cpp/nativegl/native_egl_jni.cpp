#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "../egl/GLThread.h"
#include "NativeGLRender.h"
#include "../common/zzr_common.h"

//////// 声明全局变量，在zzr_common.c的JNI_OnLoad定义
extern JavaVM * gJavaVM;

GLThread *glThread = NULL;
NativeGLRender* testRender = NULL;



extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_onSurfaceCreate(JNIEnv *env, jobject instance, jobject surface)
{
    jclass j_native_egl_class = env->GetObjectClass(instance);
    jmethodID getResAbsolutePath_mid = env->GetMethodID(j_native_egl_class, "getPackageResourceAPK",
                                                        "()Ljava/lang/String;");
    jstring res_path_jstr = static_cast<jstring>(env->CallObjectMethod(instance, getResAbsolutePath_mid,
                                                 env->NewStringUTF("/assets/mipmap/animation_test.png")));
    const char *res_path_cstr = env->GetStringUTFChars(res_path_jstr, GL_FALSE);
    LOGD("资源压缩文件：%s", res_path_cstr);
    env->ReleaseStringUTFChars(res_path_jstr, res_path_cstr);




    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);
    testRender = new NativeGLRender();
    glThread = new GLThread();
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





extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleMultiTouch(JNIEnv *env, jobject instance, jfloat distance)
{
    if(testRender!=NULL) {
        testRender->handleMultiTouch(distance);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleTouchDown(JNIEnv *env, jobject instance, jfloat x, jfloat y)
{
    if(testRender!=NULL) {
        testRender->handleTouchDown(x,y);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleTouchDrag(JNIEnv *env, jobject instance, jfloat x, jfloat y)
{
    if(testRender!=NULL) {
        testRender->handleTouchDrag(x,y);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleTouchUp(JNIEnv *env, jobject instance, jfloat x, jfloat y)
{
    if(testRender!=NULL) {
        testRender->handleTouchUp(x,y);
    }
}