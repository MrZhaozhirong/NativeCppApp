//
// Created by nicky on 2019/8/9.
//

#include <GLES3/gl3.h>
#include <assert.h>
#include "ShadowFBORender.h"
#include "../common/zzr_common.h"
#include "../utils/TextureHelper.h"

GLuint texture_id;

ShadowFBORender::ShadowFBORender() {
    mEglCore = NULL;
    mWindowSurface = NULL;
    res_path = NULL;
}

ShadowFBORender::~ShadowFBORender() {
    if( res_path!= NULL) {
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
    sprintf(res_name, "%s%s", res_path, "land.jpg");
    GLuint land_texture_id = TextureHelper::createTextureFromImage(res_name);
    sprintf(res_name, "%s%s", res_path, "test.jpg");
    GLuint texture_cube_id = TextureHelper::createTextureFromImage(res_name);

    texture_id = texture_cube_id;

    lightCube.init(CELL::float3(1,1,1), texture_cube_id);
    land.init(10, -1, land_texture_id);

    //cube = new CubeIndex(1.0f);
    //cubeShaderProgram = new CubeShaderProgram();
}

void ShadowFBORender::surfaceChanged(int width, int height)
{
    mWindowSurface->makeCurrent();
    LOGD("render surface change ... update MVP!");

    this->mViewHeight = height;
    this->mViewWidth = width;

    glViewport(0,0, width, height);
    mCamera3D.setViewPort(width, height);
    mCamera3D.perspective(45, (float)width/(float)height, 0.1f, 1000.0f);
    mCamera3D.setEye(CELL::real3(0,15,15));
    mCamera3D.setTarget(CELL::real3(0,0,0));
    mCamera3D.setUp(CELL::float3(0,1,0));
    mCamera3D.setRight(CELL::float3(1,0,0));
    mCamera3D.update();

    GLint depth_bits;
    glGetIntegerv(GL_DEPTH_BITS, &depth_bits);
    LOGW("OS.GL_DEPTH_BITS : %d", depth_bits);

    depthFBO.setup(width, height, FBO_RGBA);
    pip.init(width, height);

    mWindowSurface->swapBuffers();
}

void ShadowFBORender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore == NULL || mWindowSurface == NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    mWindowSurface->makeCurrent();
    CELL::matrix4   vp =   mCamera3D.getProject() * mCamera3D.getView();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0,0, mViewWidth, mViewHeight);
    lightCube.render(mCamera3D);
    land.render(mCamera3D);

    depthFBO.begin();
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        lightCube.render(mCamera3D);
        land.render(mCamera3D);
    }
    depthFBO.end();

    pip.setTextureId(depthFBO.getTextureId());
    pip.render();

    //renderDepthFBO();
    mWindowSurface->swapBuffers();
}

void ShadowFBORender::surfaceDestroyed(void)
{
    land.destroy();
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

void ShadowFBORender::renderDepthFBO() {
    depthFBO.begin();
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        lightCube.render(mCamera3D);
        land.render(mCamera3D);
    }
    depthFBO.end();

    pip.setTextureId(depthFBO.getTextureId());
    pip.render();
}
