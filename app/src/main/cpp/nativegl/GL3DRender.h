//
// Created by nicky on 2019/3/15.
//

#ifndef NATIVECPPAPP_GL3DRENDER_H
#define NATIVECPPAPP_GL3DRENDER_H


#include "../egl/GLRender.hpp"
#include "../common/constructormagic.h"
#include "../egl/EglCore.h"
#include "../egl/WindowSurface.h"
#include "../objects/CubeIndex.h"
#include "../program/GPUAnimationProgram.h"
#include "../program/GPUMixShaderProgram.h"
#include "../program/GPUFlatSlidingProgram.h"
#include "../program/GrasslandProgram.hpp"
#include "../objects/Grassland.h"
#include "../common/Camera3D.hpp"

class GL3DRender : public GLRender{

public:
    GL3DRender();
    ~GL3DRender();

    void surfaceCreated(ANativeWindow *window) ;
    void surfaceChanged(int width, int height) ;
    void renderOnDraw(double elpasedInMilliSec) ;
    void surfaceDestroyed(void) ;

    void handleMultiTouch(float distance);
    void handleTouchDown(float x, float y);
    void handleTouchDrag(float x, float y);
    void handleTouchUp(float x, float y);

    void setResPath(char *path);

private:
    Camera3D mCamera3D;
    float mLastX;
    float mLastY;

private:

    char  * res_path;
    GLuint  texture_0_id;
    GLuint  texture_1_id;

    EglCore * mEglCore;
    WindowSurface * mWindowSurface;

    CubeIndex * cube;
    CubeShaderProgram * cubeShaderProgram;
    Grassland * land;
    GrasslandProgram * landProgram;

private:
    DISALLOW_EVIL_CONSTRUCTORS(GL3DRender);

    void init();
};


#endif //NATIVECPPAPP_GL3DRENDER_H
