//
// Created by nicky on 2021/3/3.
//

#include <assert.h>
#include <cstdio>
#include <GLES3/gl3.h>
#include "OptimizationRender.h"
#include "../common/zzr_common.h"
#include "../utils/TextureHelper.h"

OptimizationRender::OptimizationRender() {
    mEglCore = NULL;
    mWindowSurface = NULL;
    res_path = NULL;
}

OptimizationRender::~OptimizationRender() {
    if( res_path!= NULL) {
        delete res_path;
        res_path = NULL;
    }
}

void OptimizationRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_TRY_GLES3);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    mWindowSurface->makeCurrent();






    const GLubyte * glslVersionStr = glGetString(GL_SHADING_LANGUAGE_VERSION);
    LOGW("GL_SHADING_LANGUAGE_VERSION : %s", glslVersionStr);
    const GLubyte * renderer = glGetString(GL_RENDERER);
    LOGI("GL_RENDERER : %s\n",renderer);
    const GLubyte * vendor = glGetString(GL_VENDOR);
    LOGI("GL_VENDOR : %s\n",vendor);
    const GLubyte *  extensions = glGetString(GL_EXTENSIONS);
    LOGI("GL_EXTENSIONS : %s\n",extensions);

    GLint num_compressed_format;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &num_compressed_format);
    GLint * compressed_format = (GLint*)malloc(num_compressed_format * 50 * sizeof(char));
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, compressed_format);
    for (int i = 0; i < num_compressed_format ; ++i) {
        // LOGI("GL_COMPRESSED_TEXTURE_FORMATS : %#x\n", compressed_format[i]);
    }
    delete compressed_format;
    compressed_format = NULL;

    char res_name[250]={0};
    sprintf(res_name, "%s%s", res_path, "test_normal.jpg");


    //GLuint s_texture_id = TextureHelper::createTextureFromImage(res_name);
    GLuint s_texture_id = TextureHelper::createTextureFromPBO(res_name);

    screen.init();
    screen.setTextureId(s_texture_id);
}

void OptimizationRender::surfaceChanged(int width, int height)
{
    mWindowSurface->makeCurrent();
    this->mViewHeight = height;
    this->mViewWidth = width;



    mWindowSurface->swapBuffers();
}


void OptimizationRender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore==NULL || mWindowSurface==NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    mWindowSurface->makeCurrent();
    glViewport(0,0, mViewWidth, mViewHeight);

    screen.draw();

    mWindowSurface->swapBuffers();
}

void OptimizationRender::surfaceDestroyed(void)
{
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






void OptimizationRender::handleMultiTouch(float distance) {
    //real    present =   distance > 0 ? 0.9f : 1.1f;
    //real3 target = mCamera3D.getTarget();
    //mCamera3D.scaleCameraByPos(target, present);
}

void OptimizationRender::handleTouchDown(float x, float y) {
    this->mLastX = x;
    this->mLastY = y;
}

void OptimizationRender::handleTouchDrag(float x, float y) {
    float offsetX = this->mLastX - x;
    offsetX /= 10;

    float offsetY = this->mLastY - y;
    offsetY /= 50;

    this->mLastX = x;
    this->mLastY = y;
}

void OptimizationRender::handleTouchUp(float x, float y) {
    this->mLastX = 0;
    this->mLastY = 0;
}