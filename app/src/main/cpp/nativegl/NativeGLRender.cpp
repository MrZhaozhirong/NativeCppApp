//
// Created by nicky on 2019/3/15.
//

#include <assert.h>
#include <GLES2/gl2.h>
#include "NativeGLRender.h"
#include "../common/zzr_common.h"


NativeGLRender::NativeGLRender() {
    mEglCore = NULL;
    mWindowSurface = NULL;
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
}

void NativeGLRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_RECORDABLE);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");
}
void NativeGLRender::surfaceChanged(int width, int height)
{
    mWindowSurface->makeCurrent();
    LOGD("render surface change ... update MVP!");
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
    r_count++;
    if(r_count > 255) {
        r_count = 0;
    }
    glClearColor(static_cast<GLfloat>(r_count / 100.0),
                 0.6,
                 static_cast<GLfloat>(1.0 - r_count / 100.0),
                 1.0);
    mWindowSurface->swapBuffers();
}
void NativeGLRender::surfaceDestroyed(void)
{
    // 清空自定义模型，纹理，各种BufferObject
}


