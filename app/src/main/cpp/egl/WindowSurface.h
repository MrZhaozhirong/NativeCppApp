//
// Created by nicky on 2019/3/15.
//

#ifndef NATIVECPPAPP_WINDOWSURFACE_H
#define NATIVECPPAPP_WINDOWSURFACE_H

#include "EglSurfaceBase.h"

class WindowSurface : public EglSurfaceBase{

public:
    WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface);
    WindowSurface(EglCore *eglCore, ANativeWindow *window);

    // 释放资源
    void release();
    // 重新创建
    void recreate(EglCore *eglCore);

private:
    ANativeWindow * mSurface;
    bool mReleaseSurface;
private:
    DISALLOW_EVIL_CONSTRUCTORS(WindowSurface);
};


#endif //NATIVECPPAPP_WINDOWSURFACE_H
