//
// Created by zzr on 2019/6/6.
//

#ifndef NATIVECPPAPP_LIGHTRENDER_H
#define NATIVECPPAPP_LIGHTRENDER_H


#include <GLES3/gl3.h>
#include "../egl/GLRender.hpp"
#include "../common/constructormagic.h"
#include "../egl/EglCore.h"
#include "../egl/WindowSurface.h"
#include "../common/Camera3D.hpp"
#include "../objects/CubeIlluminate.hpp"
#include "../objects/CubeTBN.hpp"

class LightRender : public GLRender{

public:
    LightRender();
    ~LightRender();

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
        if(res_path==NULL) {
            res_path = new char[250];
        } else {
            memset(res_path, 0, strlen(res_path));
        }
        strcpy(res_path, path);
        //LOGI("setResPath : %s\n", res_path);
    }

private:
    EglCore * mEglCore;
    WindowSurface * mWindowSurface;

    Camera3D mCamera3D;
    float mLastX;
    float mLastY;
private:
    char  * res_path;

    CubeIlluminate lightCube;
    CubeTBN        tbnCube;
private:
    DISALLOW_EVIL_CONSTRUCTORS(LightRender);
};


#endif //NATIVECPPAPP_LIGHTRENDER_H
