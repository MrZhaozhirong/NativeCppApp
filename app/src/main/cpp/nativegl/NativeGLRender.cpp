//
// Created by nicky on 2019/3/15.
//

#include <assert.h>
#include <GLES2/gl2.h>
#include "NativeGLRender.h"
#include "../common/zzr_common.h"
#include "../common/CELLMath.hpp"


NativeGLRender::NativeGLRender() {
    mEglCore = NULL;
    mWindowSurface = NULL;

    viewMatrix = new float[16];
    CELL::Matrix::setIdentityM(viewMatrix, 0);
    projectionMatrix = new float[16];
    CELL::Matrix::setIdentityM(projectionMatrix, 0);

    viewProjectionMatrix = new float[16];
    memset(viewProjectionMatrix, 0, sizeof(float) * 16);
}

NativeGLRender::~NativeGLRender() {
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
    delete [] viewMatrix;
    delete [] projectionMatrix;
    delete [] viewProjectionMatrix;
}

void NativeGLRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_RECORDABLE);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");

    cube = new CubeIndex();
    cubeShaderProgram = new CubeShaderProgram();
}
void NativeGLRender::surfaceChanged(int width, int height)
{
    mWindowSurface->makeCurrent();
    LOGD("render surface change ... update MVP!");
    glViewport(0,0, width, height);
    glEnable(GL_DEPTH_TEST);

    CELL::Matrix::perspectiveM(projectionMatrix, 45.0f, (float)width/(float)height, 1.0f, 100.0f);
    CELL::Matrix::setLookAtM(viewMatrix, 0,
                             4.0f, 4.0f, 4.0f,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f);
    CELL::Matrix::multiplyMM(viewProjectionMatrix,  projectionMatrix, viewMatrix);

    mWindowSurface->swapBuffers();
}
void NativeGLRender::renderOnDraw()
{
    if (mEglCore == NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    mWindowSurface->makeCurrent();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    //static int r_count = 0;
    //r_count++;
    //if(r_count > 255) {
    //    r_count = 0;
    //}
    //glClearColor((r_count / 100.0), 0.6, (1.0 - r_count / 100.0), 1.0);
    cubeShaderProgram->ShaderProgram::userProgram();
    cube->bindData(cubeShaderProgram);
    cube->draw();
    mWindowSurface->swapBuffers();
}
void NativeGLRender::surfaceDestroyed(void)
{
    // 清空自定义模型，纹理，各种BufferObject
}


