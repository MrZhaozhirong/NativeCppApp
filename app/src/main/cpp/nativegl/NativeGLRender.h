//
// Created by nicky on 2019/3/15.
//

#ifndef NATIVECPPAPP_NATIVEGLRENDER_H
#define NATIVECPPAPP_NATIVEGLRENDER_H


#include "../egl/GLRender.hpp"
#include "../common/constructormagic.h"

class NativeGLRender : public GLRender{

public:
    NativeGLRender();

    void surfaceCreated(ANativeWindow *window) override;
    void surfaceChanged(int width, int height) override;
    void renderOnDraw() override;
    void surfaceDestroyed(void) override;

private:
    DISALLOW_EVIL_CONSTRUCTORS(NativeGLRender);
};


#endif //NATIVECPPAPP_NATIVEGLRENDER_H
