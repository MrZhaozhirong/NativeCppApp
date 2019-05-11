//
// Created by nicky on 2019/3/15.
//

#include <assert.h>
#include <GLES3/gl3.h>
#include <jni.h>
#include <cstdio>
#include "GL3DRender.h"
#include "../common/zzr_common.h"
#include "../common/CELLMath.hpp"
#include "../utils/TextureHelper.h"


GL3DRender::GL3DRender() {
    init();
}

void GL3DRender::init() {
    mEglCore = NULL;
    mWindowSurface = NULL;
    res_path = NULL;
    //// 视图矩阵V
    //viewMatrix = new float[16];
    //CELL::Matrix::setIdentityM(viewMatrix, 0);
    //// 投影矩阵P
    //projectionMatrix = new float[16];
    //CELL::Matrix::setIdentityM(projectionMatrix, 0);
    //// 投影矩阵P x 视图矩阵V = VP
    //viewProjectionMatrix = new float[16];
    //memset(viewProjectionMatrix, 0, sizeof(float) * 16);
    //// (投影矩阵P x 视图矩阵V) x 具体模型的模型矩阵M = MVP
    //modelViewProjectionMatrix = new float[16];
    //memset(modelViewProjectionMatrix, 0, sizeof(float) * 16);
}

GL3DRender::~GL3DRender() {
    //if(viewMatrix!=NULL) {
    //    delete [] viewMatrix;
    //    viewMatrix = NULL;
    //}
    //if(projectionMatrix!=NULL) {
    //    delete [] projectionMatrix;
    //    projectionMatrix = NULL;
    //}
    //if(viewProjectionMatrix!=NULL) {
    //    delete [] viewProjectionMatrix;
    //    viewProjectionMatrix = NULL;
    //}
    //if(modelViewProjectionMatrix!=NULL) {
    //    delete [] modelViewProjectionMatrix;
    //    modelViewProjectionMatrix = NULL;
    //}
    if(res_path!=NULL) {
        delete res_path;
        res_path = NULL;
    }
}

void GL3DRender::setResPath(char *string) {
    if(res_path==NULL) {
        res_path = new char[250];
    } else {
        memset(res_path, 0, strlen(res_path));
    }
    strcpy(res_path, string);
    LOGI("setResPath : %s\n", res_path);
}

void GL3DRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_TRY_GLES3);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");
    mWindowSurface->makeCurrent();

    char res_name[250]={0};
    sprintf(res_name, "%s%s", res_path, "test.jpg");
    texture_0_id = TextureHelper::createTextureFromImage(res_name);
    sprintf(res_name, "%s%s", res_path, "land.jpg");
    texture_1_id = TextureHelper::createTextureFromImage(res_name);
    sprintf(res_name, "%s%s", res_path, "grass.png");
    texture_greenery_id = TextureHelper::createTextureFromImage(res_name);

    cube = new CubeIndex(1.0f);
    cubeShaderProgram = new CubeShaderProgram();
    land = new Grassland();
    landProgram = new GrasslandProgram();

    greeneryMgr.init(texture_greenery_id);


    const GLubyte * glslVersionStr = glGetString(GL_SHADING_LANGUAGE_VERSION);
    LOGW("GL_SHADING_LANGUAGE_VERSION : %s", glslVersionStr);
}

void GL3DRender::surfaceChanged(int width, int height)
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

void GL3DRender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore==NULL || mWindowSurface==NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    mWindowSurface->makeCurrent();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    CELL::matrix4   vp =   mCamera3D.getProject() * mCamera3D.getView();

    // 画正方体
    cubeShaderProgram->ShaderProgram::userProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_0_id);
    glUniform1i(cubeShaderProgram->uTextureUnit, 0);
    CELL::matrix4 cube_mvp = vp * cube->matModel;
    cubeShaderProgram->setMVPUniforms(const_cast<float *>(cube_mvp.data()));
    cube->bindData(cubeShaderProgram);
    cube->draw();

    // 画草地
    landProgram->ShaderProgram::userProgram();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_1_id);
    glUniform1i(landProgram->uTextureUnit, 1);
    CELL::matrix4 land_mvp = vp * land->matModel;
    landProgram->setMVPUniforms(const_cast<float *>(land_mvp.data()));
    land->bindData(landProgram);
    land->draw();
    land->unbind(landProgram);

    greeneryMgr.render(mCamera3D);

    mWindowSurface->swapBuffers();
}

void GL3DRender::surfaceDestroyed(void)
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






void GL3DRender::handleMultiTouch(float distance) {
    LOGD("handleMultiTouch distance:%f", distance);
    real    present =   distance > 0 ? 0.9f : 1.1f;
    real3 target = mCamera3D.getTarget();
    mCamera3D.scaleCameraByPos(target, present);
}

void GL3DRender::handleTouchDown(float x, float y) {
    this->mLastX = x;
    this->mLastY = y;
}

void GL3DRender::handleTouchDrag(float x, float y) {
    float offsetX = this->mLastX - x;
    offsetX /= 10;
    mCamera3D.rotateViewY(offsetX);

    float offsetY = this->mLastY - y;
    offsetY /= 50;
    mCamera3D.rotateViewX(offsetY);

    this->mLastX = x;
    this->mLastY = y;
}

void GL3DRender::handleTouchUp(float x, float y) {
    this->mLastX = 0;
    this->mLastY = 0;
}




