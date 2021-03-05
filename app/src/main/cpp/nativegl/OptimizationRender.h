//
// Created by nicky on 2021/3/3.
//
#ifndef OPTIMIZATION_RENDER_H
#define OPTIMIZATION_RENDER_H

#include <cstring>
#include "../egl/GLRender.hpp"
#include "../egl/EglCore.h"
#include "../egl/WindowSurface.h"
#include "../objects/Screen2D.hpp"

class OptimizationRender : public GLRender {

public:
    OptimizationRender();
    ~OptimizationRender();

    void surfaceCreated(ANativeWindow *window) ;
    void surfaceChanged(int width, int height) ;
    void renderOnDraw(double elpasedInMilliSec) ;
    void surfaceDestroyed(void) ;

    void handleMultiTouch(float distance);
    void handleTouchDown(float x, float y);
    void handleTouchDrag(float x, float y);
    void handleTouchUp(float x, float y);

    void inline setResPath(char *path)
    {
        if( res_path==NULL) {
            res_path = new char[250];
        } else {
            memset(res_path, 0, strlen(res_path));
        }
        strcpy(res_path, path);
        //LOGI("setResPath : %s\n", res_path);
    }

private:
    EglCore*        mEglCore;
    WindowSurface*  mWindowSurface;
    char*           res_path;

    Screen2D        screen;

    float       mLastX;
    float       mLastY;
    int         mViewWidth;
    int         mViewHeight;

private:
    DISALLOW_EVIL_CONSTRUCTORS(OptimizationRender);
};

#endif //OPTIMIZATION_RENDER_H
