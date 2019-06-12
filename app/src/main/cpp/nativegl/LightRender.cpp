//
// Created by zzr on 2019/6/6.
//
#include <GLES3/gl3.h>
#include "LightRender.h"
#include "../common/zzr_common.h"
#include "../utils/TextureHelper.h"

LightRender::LightRender() {
    mEglCore = NULL;
    mWindowSurface = NULL;
    res_path = NULL;
}


LightRender::~LightRender() {
    if(res_path!=NULL) {
        delete res_path;
        res_path = NULL;
    }
}


void LightRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_TRY_GLES3);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");
    mWindowSurface->makeCurrent();

    const GLubyte * glslVersionStr = glGetString(GL_SHADING_LANGUAGE_VERSION);
    LOGW("GL_SHADING_LANGUAGE_VERSION : %s", glslVersionStr);

    // 加载需要用到的资源文件
    char res_name[250]={0};
    sprintf(res_name, "%s%s", res_path, "test.jpg");
    GLuint  texture_cube_id = TextureHelper::createTextureFromImage(res_name);

    lightCube.init(CELL::float3(2,2,2), texture_cube_id);
}


void LightRender::surfaceChanged(int width, int height)
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

void LightRender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore == NULL || mWindowSurface == NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    mWindowSurface->makeCurrent();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    lightCube.render(mCamera3D);

    mWindowSurface->swapBuffers();
}

void LightRender::surfaceDestroyed(void)
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










void LightRender::handleMultiTouch(float distance) {
    LOGD("handleMultiTouch distance:%f", distance);
    real    present =   distance > 0 ? 0.9f : 1.1f;
    real3 target = mCamera3D.getTarget();
    mCamera3D.scaleCameraByPos(target, present);
}

void LightRender::handleTouchDown(float x, float y) {
    this->mLastX = x;
    this->mLastY = y;
}

void LightRender::handleTouchDrag(float x, float y) {
    float offsetX = this->mLastX - x;
    offsetX /= 10;
    mCamera3D.rotateViewY(offsetX);

    float offsetY = this->mLastY - y;
    offsetY /= 50;
    mCamera3D.rotateViewX(offsetY);

    this->mLastX = x;
    this->mLastY = y;
}

void LightRender::handleTouchUp(float x, float y) {
    this->mLastX = 0;
    this->mLastY = 0;
}
