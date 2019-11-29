//
// Created by nicky on 2019/11/28.
//

#ifndef GPU_FILTER_RENDER_H
#define GPU_FILTER_RENDER_H


#include "../../egl/GLRender.hpp"
#include "../../egl/EglCore.h"
#include "../../egl/WindowSurface.h"

class GpuFilterRender : public GLRender{
public:
    GpuFilterRender();
    ~GpuFilterRender();

    void surfaceCreated(ANativeWindow *window) ;
    void surfaceChanged(int width, int height) ;
    void renderOnDraw(double elpasedInMilliSec) ;
    void surfaceDestroyed(void) ;
    void setRotationCamera(int rotation, bool flipHorizontal, bool flipVertical) ;

private:
    EglCore*        mEglCore;
    WindowSurface*  mWindowSurface;

    int             mViewWidth;
    int             mViewHeight;

private:
    DISALLOW_EVIL_CONSTRUCTORS(GpuFilterRender);
};


#endif // GPU_FILTER_RENDER_H
