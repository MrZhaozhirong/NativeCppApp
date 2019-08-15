//
// Created by nicky on 2019/8/9.
//
#pragma once
#ifndef SHADOW_FBO_RENDER_H
#define SHADOW_FBO_RENDER_H

#include "../egl/GLRender.hpp"
#include "../common/zzr_common.h"
#include "../common/constructormagic.h"
#include "../common/Camera3D.hpp"
#include "../egl/EglCore.h"
#include "../egl/WindowSurface.h"
#include "../objects/Land.hpp"
#include "../objects/CubeIlluminate.hpp"
#include "../common/FrameBufferObject.hpp"
#include "../program/DepthOrthoShader.hpp"
#include "../objects/PIPicture.hpp"

class ShadowFBORender : public GLRender {

public:
    ShadowFBORender();
    ~ShadowFBORender();

    void surfaceCreated(ANativeWindow *window) ;
    void surfaceChanged(int width, int height) ;
    void renderOnDraw(double elpasedInMilliSec) ;
    void surfaceDestroyed(void) ;

    void handleMultiTouch(float distance);
    void handleTouchDown(float x, float y);
    void handleTouchDrag(float x, float y);
    void handleTouchUp(float x, float y);

    void setResPath(char *path)
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
    Land            land;
    CubeIlluminate  lightCube;

    FrameBufferObject   depthFBO;
    PIPicture           pip;
private:
    EglCore * mEglCore;
    WindowSurface * mWindowSurface;

    Camera3D mCamera3D;
    float mLastX;
    float mLastY;
    int   mViewWidth;
    int   mViewHeight;

    char  * res_path;
private:
    DISALLOW_EVIL_CONSTRUCTORS(ShadowFBORender);

    void renderDepthFBO();
};
#endif //SHADOW_FBO_RENDER_H
