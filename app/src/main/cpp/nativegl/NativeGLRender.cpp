//
// Created by nicky on 2019/3/15.
//

#include <assert.h>
#include <GLES3/gl3.h>
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
    if(res_path==NULL) {
        res_path = new char[250];
    } else {
        memset(res_path, 0, strlen(res_path));
    }
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

    cube = new CubeIndex(0.5f);
    cubeShaderProgram = new CubeShaderProgram();

    char res_name[250]={0};
    sprintf(res_name, "%s%s", res_path, "test.jpg");
    texture_0_id = TextureHelper::createTextureFromImage(res_name);
    sprintf(res_name, "%s%s", res_path, "land.jpg");
    texture_1_id = TextureHelper::createTextureFromImage(res_name);
}

void NativeGLRender::surfaceChanged(int width, int height)
{
    mWindowSurface->makeCurrent();
    LOGD("render surface change ... update MVP!");
    glViewport(0,0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    CELL::Matrix::perspectiveM(projectionMatrix, 45.0f, (float)width/(float)height, 1.0f, 100.0f);
    CELL::Matrix::setLookAtM(viewMatrix, 0,
                             10.0f, 10.0f, 10.0f,
                             0.0f,  0.0f,  0.0f,
                             0.0f,  1.0f,  0.0f);
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

    // GPUAnimationProgram所需
    int     frame   =   int(elpasedInMilliSec/1000 * 16)%16;
    // GPUMixShaderProgram和GPUFlatSlidingProgram所需
    double _hasElasped = elpasedInMilliSec/1000 * 0.1f;

    // 画正方体
    cubeShaderProgram->ShaderProgram::userProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_0_id);
    glUniform1i(cubeShaderProgram->uTextureUnit, 0);
    CELL::Matrix::multiplyMM(modelViewProjectionMatrix, viewProjectionMatrix, cube->modelMatrix);
    cubeShaderProgram->setMVPUniforms(modelViewProjectionMatrix);
    //gpuAnimationProgram->setAnimUniforms(4,4,frame);
    //gpuFlatSlidingProgram->setOffsetUniform(_hasElasped);
    //gpuMixShaderProgram->setMixUniform(_hasElasped);
    cube->bindData(cubeShaderProgram);
    cube->draw();


    mWindowSurface->swapBuffers();
}

void NativeGLRender::surfaceDestroyed(void)
{
    // 清空自定义模型，纹理，各种BufferObject
    if (cubeShaderProgram!=NULL) {
        delete cubeShaderProgram;
        cubeShaderProgram = NULL;
    }
    if (cube!=NULL) {
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
        //CELL::Matrix::scaleM(cube->modelMatrix,0, OBJECT_SCALE_FLOAT,OBJECT_SCALE_FLOAT,OBJECT_SCALE_FLOAT);
    } else {
        //CELL::Matrix::scaleM(cube->modelMatrix,0, 1/OBJECT_SCALE_FLOAT,1/OBJECT_SCALE_FLOAT,1/OBJECT_SCALE_FLOAT);
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




