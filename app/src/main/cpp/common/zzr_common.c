
//
// Created by zzr on 2019/1/4.
// 公共函数库

#include <jni.h>
#include <assert.h>
#include <stdio.h>
#include "zzr_common.h"

JavaVM * gJavaVM;


JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved)
{
    LOGW("%s\n", "JNI_OnLoad startup ...");
    gJavaVM = vm;
    JNIEnv *env = NULL;
    jint result;

    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) == JNI_OK) {
        LOGI("Catch JNI_VERSION_1_6\n");
        result = JNI_VERSION_1_6;
    } else if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_4) == JNI_OK) {
        LOGI("Catch JNI_VERSION_1_4\n");
        result = JNI_VERSION_1_4;
    } else {
        LOGI("Default JNI_VERSION_1_2\n");
        result = JNI_VERSION_1_2;
    }

    assert(env != NULL);
    // 动态注册native函数 ...
    return result;
}

void ffmpeg_custom_log(void *ptr, int level, const char* fmt, va_list vl)
{
    FILE *fp=fopen("/storage/emulated/0/av_log.txt","a+");
    if(fp){
        vfprintf(fp,fmt,vl);
        fflush(fp);
        fclose(fp);
    }
}

int isEmptyStr(const char * str)
{
    if(strlen(str)==0) return -1;
    if(*str == '\0') return -1;
    return 0;
}