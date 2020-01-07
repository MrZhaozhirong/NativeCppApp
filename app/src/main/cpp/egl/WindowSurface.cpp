//
// Created by nicky on 2019/3/15.
//

#include "WindowSurface.h"
#include "../common/zzr_common.h"
#include <android/native_window.h>


WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface)
        : EglSurfaceBase(eglCore) {
    createWindowSurface(window);
    ANativeWindow_acquire(mSurface);
    mSurface = window;
    mReleaseSurface = releaseSurface;
}


WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window)
        : EglSurfaceBase(eglCore) {
    createWindowSurface(window);
    ANativeWindow_acquire(mSurface);
}

//释放当前EGL上下文 关联 的 surface
void WindowSurface::release() {
    releaseEglSurface();
    if (mSurface != NULL
        && mReleaseSurface) {
        ANativeWindow_release(mSurface);
        mSurface = NULL;
    }
}

void WindowSurface::recreate(EglCore *eglCore) {
    if (mSurface == NULL) {
        LOGE("not yet implemented ANativeWindow");
        return;
    }
    mEglCore = eglCore;
    createWindowSurface(mSurface);
    ANativeWindow_acquire(mSurface);
}