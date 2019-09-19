//
// Created by nicky on 2019/8/9.
//

#include <GLES3/gl3.h>
#include <assert.h>
#include "ShadowFBORender.h"
#include "../common/zzr_common.h"
#include "../utils/TextureHelper.h"

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

    lightCube.init(CELL::float3(1,1,1), texture_cube_id);
    land.init(10, -1, land_texture_id);

    mLightPosition = CELL::real3(5, 5, 2);
    lightPositionCube.init(CELL::real3(0.15f,0.15f,0.15f), 0);
    lightPositionCube.mModelMatrix.translate(mLightPosition);

    mShadowShader.initialize();
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

    depthFBO.setup(width, height, FBO_DEPTH);
    //pip.init(width, height);

    mWindowSurface->swapBuffers();
}

void ShadowFBORender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore == NULL || mWindowSurface == NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    mWindowSurface->makeCurrent();

    mLightProjectionMatrix = CELL::perspective(45.0f, (float)mViewWidth/(float)mViewHeight, 0.1f, 30.0f);
    mLightViewMatrix       = CELL::lookAt(mLightPosition, CELL::real3(0,0,0), CELL::real3(0,1.0,0));
    renderDepthFBO();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    glViewport(0,0, mViewWidth, mViewHeight);

    renderShadow();

    lightPositionCube.render(mCamera3D);
    land.render(mCamera3D);
    lightCube.render(mCamera3D, getLightDir());
    //renderDepthFBO();
    mWindowSurface->swapBuffers();
}

//user to debug
void ShadowFBORender::renderShadow()
{
    CELL::matrix4   matBias =   CELL::matrix4(0.5f, 0.0f, 0.0f, 0.0f,
                                              0.0f, 0.5f, 0.0f, 0.0f,
                                              0.0f, 0.0f, 0.5f, 0.0f,
                                              0.5f, 0.5f, 0.5f, 1.0f);
    CELL::matrix4   matView   =   mCamera3D.getView();
    CELL::matrix4   matPRJ    =   mCamera3D.getProject();
    CELL::matrix4   matShadow =   matBias * mLightProjectionMatrix * mLightViewMatrix;
    mShadowShader.begin();
    {
        glUniformMatrix4fv(mShadowShader._matShadow, 1, GL_FALSE, matShadow.data());
        glUniformMatrix4fv(mShadowShader._matMV,  1, GL_FALSE, matView.data());
        glUniformMatrix4fv(mShadowShader._matPRJ, 1, GL_FALSE, matPRJ.data());
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthFBO.getDepthTexId());
        glUniform1i(mShadowShader._shadowMap, 1);
        glUniform4f(mShadowShader._color, 0.5f,0.5f,0.5f,1.0f);
        //地板
        glVertexAttribPointer(static_cast<GLuint>(mShadowShader._attrPos), 3, GL_FLOAT, GL_FALSE,
                              sizeof(Land::V3N3UV2), &(land._data[0].x));
        glDrawArrays(GL_TRIANGLES, 0, 2*3);
        //正方体
        glVertexAttribPointer(static_cast<GLuint>(mShadowShader._attrPos), 3, GL_FLOAT, GL_FALSE,
                              sizeof(CubeIlluminate::V3N3T2), &(lightCube._data[0].x));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    mShadowShader.end();
}

void ShadowFBORender::renderDepthFBO()
{
    depthFBO.begin();
    {
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        glClearDepthf(1.0f);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        //land.render(mLightProjectionMatrix, mLightViewMatrix);
        lightCube.renderShadow(mLightProjectionMatrix, mLightViewMatrix, getLightDir());
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);
    }
    depthFBO.end();
    //pip.setTextureId(depthFBO.getDepthTexId());
    //pip.render();
}
//pip.setTextureId(depthFBO.getDepthTexId());
//pip.setTextureId(depthFBO.getRgbaTexId());
//pip.render();

//GLenum renderObj[] = {GL_FRONT_FACE};
//glDrawBuffers(1, renderObj);
//glBindFramebuffer(GL_READ_FRAMEBUFFER, depthFBO._fboID);
//glReadBuffer(GL_DEPTH_ATTACHMENT);
//glBlitFramebuffer(0, 0, mViewWidth,mViewHeight,
//                  mViewWidth/2,0, mViewWidth,mViewHeight/2,
//                    GL_DEPTH_BUFFER_BIT, GL_LINEAR);

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

//__inline CELL::real3 ShadowFBORender::getLightDir() {
//    return normalize(mCamera3D.getTarget() - mLightPosition);
//}

