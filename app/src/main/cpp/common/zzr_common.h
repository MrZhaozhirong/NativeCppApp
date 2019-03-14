#pragma once
//
// Created by nicky on 2018/11/29.
//
#ifndef ZZR_COMMON_H
#define ZZR_COMMON_H

#include <android/log.h>
#include <string.h>

#define DEBUG // DEBUG开关，整行注销就能全局关闭调试日志。

// Windows 和 Linux 这两个宏是在 CMakeLists.txt 通过 ADD_DEFINITIONS 定义的
#ifdef Windows
#define __FILENAME__ (strrchr(__FILE__, '\\') + 1) // Windows下文件目录层级是'\\'
#elif Linux
#define __FILENAME__ (strrchr(__FILE__, '/') + 1) // Linux下文件目录层级是'/'
#else
#define __FILENAME__ (strrchr(__FILE__, '/') + 1) // 默认使用这种方式
#endif


#ifdef DEBUG
#define TAG "ZzrBlogNDK"
#define LOGV(format, ...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,\
        "[%s][%d]: " format, __FILENAME__,  __LINE__, ##__VA_ARGS__);
#define LOGD(format, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG,\
        "[%s][%d]: " format, __FILENAME__,  __LINE__, ##__VA_ARGS__);
#define LOGI(format, ...) __android_log_print(ANDROID_LOG_INFO, TAG,\
        "[%s][%d]: " format, __FILENAME__,  __LINE__, ##__VA_ARGS__);
#define LOGW(format, ...) __android_log_print(ANDROID_LOG_WARN, TAG,\
        "[%s][%d]: " format, __FILENAME__,  __LINE__, ##__VA_ARGS__);
#define LOGE(format, ...) __android_log_print(ANDROID_LOG_ERROR, TAG,\
        "[%s][%d]: " format, __FILENAME__,  __LINE__, ##__VA_ARGS__);
#else
#define LOGV(format, ...)  ;
#define LOGD(format, ...)  ;
#define LOGI(format, ...)  ;
#define LOGW(format, ...)  ;
#define LOGE(format, ...)  ;
#endif // DEBUG


int     isEmptyStr (const char * str);
void    ffmpeg_custom_log (void *ptr, int level, const char* fmt, va_list vl);


#endif // ZZR_COMMON_H
