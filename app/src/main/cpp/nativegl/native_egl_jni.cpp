#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <malloc.h>
#include "../egl/GLThread.h"
#include "NativeGLRender.h"
#include "../common/zzr_common.h"
#include "../include/zip.h"
#include "GL3DRender.h"
#include "LightRender.h"
#include "ShadowFBORender.h"
#include "OptimizationRender.h"
#include <sys/stat.h>
#include <unistd.h>

GLThread* glThread = NULL;
//NativeGLRender* renderer = NULL;
//GL3DRender* m3dRenderer = NULL;
//LightRender* lightRenderer = NULL;
//ShadowFBORender* shadowRender = NULL;
OptimizationRender* render = NULL;

void unzipAssetsResFile(const char *compressed_apk_path_cstr, const char *release_res_path_cstr)
{
    int iErr = 0;
    struct zip* apkArchive = zip_open(compressed_apk_path_cstr, ZIP_CHECKCONS, &iErr);
    if(!apkArchive) {
        LOGE("zip open failed:%d\n", iErr);
        return;
    }
    struct zip_stat fstat;
    zip_stat_init(&fstat);

    int numFiles = zip_get_num_files(apkArchive);
    // 指定要解压出"assets/mipmap/"这类前缀的文件
    const char *prefix = "assets/mipmap/";
    for (int i=0; i<numFiles; i++) {
        const char* name = zip_get_name(apkArchive, i, 0);
        if (name == NULL) {
            LOGE("Error reading zip file name at index %i : %s", i, zip_strerror(apkArchive));
            return;
        }
        zip_stat(apkArchive,name,0,&fstat);
        //LOGD("Index %i:%s，Uncompressed Size:%d，Compressed Size:%d", i, fstat.name, fstat.size, fstat.comp_size);
        // 查找指定前缀的资源文件
        const char * ret = strstr(fstat.name, prefix);
        if( ret ) {
            LOGI("find it! : %s",ret);
            char dest_path[250]={0};
            strcat (dest_path, release_res_path_cstr);
            strcat (dest_path, "/");
            strcat (dest_path, prefix);
            // create all level path
            int iPathLength=strlen(dest_path);
            int iLeaveLength=0;
            int iCreatedLength=0;
            char szPathTemp[250]={0};
            for (int j=0; (NULL!=strchr(dest_path+iCreatedLength,'/')); j++)
            {
                iLeaveLength = strlen(strchr(dest_path+iCreatedLength,'/'))-1;
                iCreatedLength = iPathLength - iLeaveLength;
                strncpy(szPathTemp, dest_path, iCreatedLength);
                if (access(szPathTemp, F_OK) != 0){
                    mkdir(szPathTemp, 0777);
                    LOGI("mkdir : %s\n", szPathTemp);
                }
            }
            if (access(dest_path, F_OK) != 0){
                mkdir(dest_path, 0777);
                LOGI("mkdir : %s\n", dest_path);
            }
            // unzip file
            char dest_file[250]={0};
            strcat (dest_file, release_res_path_cstr);
            strcat (dest_file, "/");
            strcat (dest_file, fstat.name);
            //LOGI("uncompressed file : %s\n", dest_file);
            if(access(dest_file, F_OK) == 0) {
                LOGI("unzip %s has exist.\n", dest_file);
                continue;
            }
            FILE *fp = fopen(dest_file, "w+");
            if (!fp) {
                LOGE("Create unzip file failed.");
                break;
            }

            ssize_t iRead = 0;
            size_t iLen = 0;
            char buf[1024];
            struct zip_file* file = zip_fopen(apkArchive, fstat.name, 0);
            while(iLen < fstat.size)
            {
                iRead = zip_fread(file, buf, 1024);
                if (iRead < 0) {
                    LOGE("zip_fread file failed");
                    break;
                }
                fwrite(buf, 1, static_cast<size_t>(iRead), fp);
                iLen += iRead;
            }
            fclose(fp);
            LOGI("Create unzip file : %s done!", dest_file);
            zip_fclose(file);
        }
    }
    zip_close(apkArchive);
}


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

    // 解压纹理资源文件
    unzipAssetsResFile(compressed_apk_path_cstr, release_res_path_cstr);

    render = new OptimizationRender();
    //TODO 这些解压缩逻辑可以放到render里面
    char res_path[250]={0};
    strcat (res_path, release_res_path_cstr);
    strcat (res_path, "/");
    strcat (res_path, "assets/mipmap/");
    render->setResPath(res_path);

    env->ReleaseStringUTFChars(compressed_apk_path_jstr, compressed_apk_path_cstr);
    env->ReleaseStringUTFChars(release_res_path_jstr, release_res_path_cstr);
}

extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_onSurfaceCreate(JNIEnv *env, jobject instance, jobject surface)
{
    ANativeWindow* nativeWindow = ANativeWindow_fromSurface(env, surface);

    glThread = new GLThread();
    glThread->setGLRender(render);
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
    glThread->onSurfaceDestroy();
    glThread->release();
    glThread = NULL;
    if(render !=NULL)delete render;
}





extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleMultiTouch(JNIEnv *env, jobject instance, jfloat distance)
{
    if( render!=NULL) {
        render->handleMultiTouch(distance);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleTouchDown(JNIEnv *env, jobject instance, jfloat x, jfloat y)
{
    if( render!=NULL) {
        render->handleTouchDown(x,y);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleTouchDrag(JNIEnv *env, jobject instance, jfloat x, jfloat y)
{
    if( render!=NULL) {
        render->handleTouchDrag(x,y);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_org_zzrblog_nativecpp_NativeEGL_handleTouchUp(JNIEnv *env, jobject instance, jfloat x, jfloat y)
{
    if( render!=NULL) {
        render->handleTouchUp(x,y);
    }
}