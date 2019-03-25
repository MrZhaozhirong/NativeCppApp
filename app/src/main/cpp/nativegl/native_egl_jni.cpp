#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "../egl/GLThread.h"
#include "NativeGLRender.h"
#include "../common/zzr_common.h"
#include "../include/zip.h"

//////// 声明全局变量，在zzr_common.c的JNI_OnLoad定义
extern JavaVM * gJavaVM;

GLThread *glThread = NULL;
NativeGLRender* testRender = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_initBeforeEGL(JNIEnv *env, jobject instance, jobject context)
{
    jclass j_native_egl_class = env->GetObjectClass(instance);
    jmethodID getResAbsolutePath_mid = env->GetMethodID(j_native_egl_class, "getPackageResourceAPK",
                                                        "()Ljava/lang/String;");
    jstring compressed_apk_path_jstr = static_cast<jstring>(env->CallObjectMethod(instance, getResAbsolutePath_mid));
    const char *compressed_apk_path_cstr = env->GetStringUTFChars(compressed_apk_path_jstr, GL_FALSE);
    LOGI("资源压缩apk文件：%s", compressed_apk_path_cstr);

    jmethodID getResourceCacheDir_mid = env->GetMethodID(j_native_egl_class, "getResourceCacheDir",
                                                        "()Ljava/lang/String;");
    jstring release_res_path_jstr = static_cast<jstring>(env->CallObjectMethod(instance, getResourceCacheDir_mid));
    const char *release_res_path_cstr = env->GetStringUTFChars(release_res_path_jstr, GL_FALSE);
    LOGI("本地解压缩路径：%s", release_res_path_cstr);


    int iErr = 0;
    struct zip* apkArchive = zip_open(compressed_apk_path_cstr, ZIP_CHECKCONS, &iErr);
    if(!apkArchive) {
        LOGE("zip open failed:%d\n", iErr);
        env->ReleaseStringUTFChars(compressed_apk_path_jstr, compressed_apk_path_cstr);
        env->ReleaseStringUTFChars(release_res_path_jstr, release_res_path_cstr);
        return;
    }
    struct zip_stat fstat;
    zip_stat_init(&fstat);

    int numFiles = zip_get_num_files(apkArchive);
    LOGI("File numFiles %i \n",numFiles);
    for (int i=0; i<numFiles; i++) {
        const char* name = zip_get_name(apkArchive, i, 0);
        if (name == NULL) {
            LOGE("Error reading zip file name at index %i : %s", i, zip_strerror(apkArchive));
            return;
        }
        zip_stat(apkArchive,name,0,&fstat);
        LOGD("Index %i:%s      Uncompressed Size:%d    Compressed Size:%d", i, fstat.name, fstat.size, fstat.comp_size);

    }
    env->ReleaseStringUTFChars(compressed_apk_path_jstr, compressed_apk_path_cstr);
    env->ReleaseStringUTFChars(release_res_path_jstr, release_res_path_cstr);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_onSurfaceCreate(JNIEnv *env, jobject instance, jobject surface)
{
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