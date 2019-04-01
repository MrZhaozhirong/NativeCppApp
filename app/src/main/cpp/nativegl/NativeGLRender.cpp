//
// Created by nicky on 2019/3/15.
//

#include <assert.h>
#include <GLES2/gl2.h>
#include <jni.h>
#include <cstdio>
#include "NativeGLRender.h"
#include "../common/zzr_common.h"
#include "../common/CELLMath.hpp"
#include "../utils/TextureHelper.h"


NativeGLRender::NativeGLRender() {
    init();
}

void NativeGLRender::init() {
    mEglCore = NULL;
    mWindowSurface = NULL;
    res_path = NULL;
    // 视图矩阵V
    viewMatrix = new float[16];
    CELL::Matrix::setIdentityM(viewMatrix, 0);
    // 投影矩阵P
    projectionMatrix = new float[16];
    CELL::Matrix::setIdentityM(projectionMatrix, 0);
    // 投影矩阵P x 视图矩阵V = VP
    viewProjectionMatrix = new float[16];
    memset(viewProjectionMatrix, 0, sizeof(float) * 16);
    // (投影矩阵P x 视图矩阵V) x 具体模型的模型矩阵M = MVP
    modelViewProjectionMatrix = new float[16];
    memset(modelViewProjectionMatrix, 0, sizeof(float) * 16);
}

NativeGLRender::~NativeGLRender() {
    if(viewMatrix!=NULL) {
        delete [] viewMatrix;
        viewMatrix = NULL;
    }
    if(projectionMatrix!=NULL) {
        delete [] projectionMatrix;
        projectionMatrix = NULL;
    }
    if(viewProjectionMatrix!=NULL) {
        delete [] viewProjectionMatrix;
        viewProjectionMatrix = NULL;
    }
    if(modelViewProjectionMatrix!=NULL) {
        delete [] modelViewProjectionMatrix;
        modelViewProjectionMatrix = NULL;
    }
    if(res_path!=NULL) {
        delete res_path;
        res_path = NULL;
    }
}

void NativeGLRender::setResPath(char *string) {
    res_path = new char[250];
    strcpy(res_path, string);
    LOGI("setResPath : %s\n", res_path);
}

void NativeGLRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_RECORDABLE);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");
    mWindowSurface->makeCurrent();

    cube = new CubeIndex();
    cubeShaderProgram = new CubeShaderProgram();

    char _res_name[250]={0};
    sprintf(_res_name, "%s%s", res_path, "test.jpg");
    // 输入资源文件的资源路径，创建纹理，返回纹理id
    animation_texure = TextureHelper::createTextureFromImage(_res_name);
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
void NativeGLRender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore==NULL || mWindowSurface==NULL) {
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
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, animation_texure);
    glUniform1i(cubeShaderProgram->uTextureUnit, 0);
    CELL::Matrix::multiplyMM(modelViewProjectionMatrix, viewProjectionMatrix, cube->modelMatrix);
    cubeShaderProgram->setUniforms(modelViewProjectionMatrix);
    cube->bindData(cubeShaderProgram);
    cube->draw();
    mWindowSurface->swapBuffers();
}
void NativeGLRender::surfaceDestroyed(void)
{
    // 清空自定义模型，纹理，各种BufferObject
    if(cubeShaderProgram!=NULL) {
        delete cubeShaderProgram;
        cubeShaderProgram = NULL;
    }
    if(cube!=NULL) {
        delete cube;
        cube = NULL;
    }
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






void NativeGLRender::handleMultiTouch(float distance) {
    float OBJECT_SCALE_FLOAT = 1.1f;
    if(distance > 0) {
        CELL::Matrix::scaleM(cube->modelMatrix,0, OBJECT_SCALE_FLOAT,OBJECT_SCALE_FLOAT,OBJECT_SCALE_FLOAT);
    } else {
        CELL::Matrix::scaleM(cube->modelMatrix,0, 1/OBJECT_SCALE_FLOAT,1/OBJECT_SCALE_FLOAT,1/OBJECT_SCALE_FLOAT);
    }
}

void NativeGLRender::handleTouchDown(float x, float y) {
    // TODO
}

void NativeGLRender::handleTouchDrag(float x, float y) {
    // TODO
}

void NativeGLRender::handleTouchUp(float x, float y) {
    // TODO
}




