//
// Created by nicky on 2019/8/9.
//

#include <GLES3/gl3.h>
#include "ShadowFBORender.h"
#include "../common/zzr_common.h"
#include "../utils/TextureHelper.h"

ShadowFBORender::ShadowFBORender() {
    mEglCore = NULL;
    mWindowSurface = NULL;
    res_path = NULL;
}

ShadowFBORender::~ShadowFBORender() {
    if(res_path!=NULL) {
        delete res_path;
        res_path = NULL;
    }
}

void ShadowFBORender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_TRY_GLES3);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");
    mWindowSurface->makeCurrent();

    char res_name[250]={0};
    sprintf(res_name, "%s%s", res_path, "z3.jpg");
    GLuint  texture_cube_id = TextureHelper::createTextureFromImage(res_name);
}

void ShadowFBORender::surfaceChanged(int width, int height)
{
    mWindowSurface->makeCurrent();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    LOGD("render surface change ... update MVP!");

    glViewport(0,0, width, height);
    mCamera3D.setViewPort(width, height);
    mCamera3D.perspective(45, (float)width/(float)height, 0.1f, 1000.0f);
    mCamera3D.setEye(CELL::real3(0,20,20));
    mCamera3D.setTarget(CELL::real3(0,0,0));
    mCamera3D.setUp(CELL::float3(0,1,0));
    mCamera3D.setRight(CELL::float3(1,0,0));
    mCamera3D.update();

    mWindowSurface->swapBuffers();
}

void ShadowFBORender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore == NULL || mWindowSurface == NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    mWindowSurface->makeCurrent();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // draw something
    mWindowSurface->swapBuffers();
}

void ShadowFBORender::surfaceDestroyed(void)
{
    // 清空自定义模型，纹理，各种BufferObject
    if (mWindowSurface) {
        mWindowSurface->release();
        delete mWindowSurface;
        mWindowSurface = NULL;
    }
    if (mEglCore) {
        mEglCore->release();
        delete mEglCore;
        mEglCore = NULL;
    }
}


void ShadowFBORender::handleMultiTouch(float distance) {
    LOGD("handleMultiTouch distance:%f", distance);
    real    present =   distance > 0 ? 0.9f : 1.1f;
    real3 target = mCamera3D.getTarget();
    mCamera3D.scaleCameraByPos(target, present);
}

void ShadowFBORender::handleTouchDown(float x, float y) {
    this->mLastX = x;
    this->mLastY = y;
}

void ShadowFBORender::handleTouchDrag(float x, float y) {
    float offsetX = this->mLastX - x;
    offsetX /= 10;
    mCamera3D.rotateViewY(offsetX);

    float offsetY = this->mLastY - y;
    offsetY /= 50;
    mCamera3D.rotateViewX(offsetY);

    this->mLastX = x;
    this->mLastY = y;
}

void ShadowFBORender::handleTouchUp(float x, float y) {
    this->mLastX = 0;
    this->mLastY = 0;
}
