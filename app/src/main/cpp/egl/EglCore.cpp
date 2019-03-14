//
// Created by nicky on 2019/3/14.
//

#include "EglCore.h"
#include "../common/zzr_common.h"
// static method
void EglCore::logCurrentEglState()
{
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;

    display = eglGetCurrentDisplay();
    context = eglGetCurrentContext();
    surface = eglGetCurrentSurface(EGL_DRAW);

    LOGI("Current EGL state : display=%p, context=%p, surface=%p",display,context,surface);
}



EglCore::EglCore() {
    mEglDisplay = EGL_NO_DISPLAY;
    mEglContext = EGL_NO_CONTEXT;
    mEglSurface = EGL_NO_SURFACE;
}

EglCore::~EglCore() {

}




void EglCore::logThisEglState()
{
    LOGI("Current EGL state : display=%p, context=%p, surface=%p",this->mEglDisplay,this->mEglContext,this->mEglSurface);
}

int EglCore::initEgl(EGLNativeWindowType window, int flags)
{
    if (mEglDisplay != EGL_NO_DISPLAY) {
        LOGE("EGL already set up");
        return -1;
    }

    mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(mEglDisplay == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay wrong");
        return -1;
    }

    EGLint *version = new EGLint[2];
    if (!eglInitialize(mEglDisplay, &version[0], &version[1]) ) {
        mEglDisplay = NULL;
        LOGE("unable to initialize");
        return -1;
    }

    if ((flags & FLAG_TRY_GLES3) != 0) {
        EGLConfig config = getConfig(flags, 3);
        if (config != NULL) {
            const EGLint attrib3_list[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };
            EGLContext context = eglCreateContext(mEglDisplay, config, EGL_NO_CONTEXT, attrib3_list);
            if (eglGetError() == EGL_SUCCESS) {
                LOGD("Got GLES 3 config");
                mEGLConfig = config;
                mEglContext = context;
                mEglVersion = 3;
            }
        }
    }

    return 0;
}


/**
 * 从本地设备中寻找合适的 EGLConfig.
 */
EGLConfig EglCore::getConfig(int flags, int version)
{
    return NULL;
}