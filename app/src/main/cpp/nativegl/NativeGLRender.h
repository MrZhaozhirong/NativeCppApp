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

    void surfaceCreated(ANativeWindow *window) override;
    void surfaceChanged(int width, int height) override;
    void renderOnDraw() override;
    void surfaceDestroyed(void) override;

private:
    int r_count;

    EglCore * mEglCore;
    WindowSurface * mWindowSurface;

    CubeIndex * cube;
    CubeShaderProgram * cubeShaderProgram;

    DISALLOW_EVIL_CONSTRUCTORS(NativeGLRender);
};


#endif //NATIVECPPAPP_NATIVEGLRENDER_H
