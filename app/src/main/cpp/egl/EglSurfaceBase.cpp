//
// Created by nicky on 2019/3/15.
//

#include <GLES3/gl3.h>
#include "EglSurfaceBase.h"
#include "../common/zzr_common.h"


EglSurfaceBase::EglSurfaceBase(EglCore *eglCore)
// 右边这样写是cpp的语法，习惯java模式也可以        : mEglCore(eglCore)
{
    mEglCore = eglCore;
    mEglSurface = EGL_NO_SURFACE;
}


/**
 * 创建显示的Surface
 * @param nativeWindow
 */
void EglSurfaceBase::createWindowSurface(ANativeWindow *nativeWindow) {
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGE("surface already created\n");
        return;
    }
    mEglSurface = mEglCore->createWindowSurface(nativeWindow);
}

/**
 * 创建离屏surface
 * @param width
 * @param height
 */
void EglSurfaceBase::createOffScreenSurface(int width, int height) {
    if (mEglSurface != EGL_NO_SURFACE) {
        LOGE("surface already created\n");
        return;
    }
    mEglSurface = mEglCore->createOffscreenSurface(width, height);
    mWidth = width;
    mHeight = height;
}

/**
 * 返回surface的width长度, 单位是pixels.
 * @return
 */
int EglSurfaceBase::getWidth() {
    if (mWidth < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_WIDTH);
    } else {
        return mWidth;
    }
}
/**
 * 返回surface的height长度, 单位是pixels.
 * @return
 */
int EglSurfaceBase::getHeight() {
    if (mHeight < 0) {
        return mEglCore->querySurface(mEglSurface, EGL_HEIGHT);
    } else {
        return mHeight;
    }
}
/**
 * 获取当前EglSurface对象
 * @return
 */
EGLSurface EglSurfaceBase::getEglSurface() {
    return this->mEglSurface;
}
/**
 * 释放EGLSurface
 */
void EglSurfaceBase::releaseEglSurface() {
    mEglCore->makeNothingCurrent();
    mEglCore->releaseSurface(mEglSurface);
    mEglSurface = EGL_NO_SURFACE;
    mWidth = mHeight = -1;
}

/**
 * Makes our EGL context and surface current.
 */
void EglSurfaceBase::makeCurrent() {
    mEglCore->makeCurrent(mEglSurface);
}

/**
 * Makes our EGL context and surface current for drawing,
 * using the supplied surface for reading.
 * @param readSurface
 */
void EglSurfaceBase::makeCurrentReadFrom(EglSurfaceBase readSurface) {
    mEglCore->makeCurrent(mEglSurface, readSurface.getEglSurface());
}

/**
 * 交换到前台显示
 * @return
 */
bool EglSurfaceBase::swapBuffers() {
    bool result = mEglCore->swapBuffers(mEglSurface);
    if (!result) {
        LOGW("WARNING: swapBuffers() failed");
    }
    return result;
}

/**
 * 设置当前时间戳
 * @param nsecs
 */
void EglSurfaceBase::setPresentationTime(long nsecs) {
    mEglCore->setPresentationTime(mEglSurface, nsecs);
}

/**
 * 获取当前像素
 * @return
 */
char* EglSurfaceBase::getFrame() {
    char *pixels = NULL;
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return pixels;
}