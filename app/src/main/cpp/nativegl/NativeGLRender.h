//
// Created by nicky on 2019/3/15.
//

#ifndef NATIVECPPAPP_NATIVEGLRENDER_H
#define NATIVECPPAPP_NATIVEGLRENDER_H


#include "../egl/GLRender.hpp"
#include "../common/constructormagic.h"
#include "../egl/EglCore.h"
#include "../egl/WindowSurface.h"
#include "../objects/CubeIndex.h"

class NativeGLRender : public GLRender{

public:
    NativeGLRender();
    ~NativeGLRender();

    void surfaceCreated(ANativeWindow *window) ;
    void surfaceChanged(int width, int height) ;
    void renderOnDraw(double elpasedInMilliSec) ;
    void surfaceDestroyed(void) ;

    void handleMultiTouch(float distance);
    void handleTouchDown(float x, float y);
    void handleTouchDrag(float x, float y);
    void handleTouchUp(float x, float y);


    void setResPath(char *string);

private:
    float * modelViewProjectionMatrix;
    float * viewProjectionMatrix;
    float * projectionMatrix;
    float * viewMatrix;

    char  * res_path;
    GLuint  animation_texure;

    EglCore * mEglCore;
    WindowSurface * mWindowSurface;
    CubeIndex * cube;
    CubeShaderProgram * cubeShaderProgram;
private:
    DISALLOW_EVIL_CONSTRUCTORS(NativeGLRender);

    void init();
};


#endif //NATIVECPPAPP_NATIVEGLRENDER_H
