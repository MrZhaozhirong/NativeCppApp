//
// Created by nicky on 2019/11/28.
//

#include <assert.h>
#include "GpuFilterRender.h"
#include "../../common/zzr_common.h"

GpuFilterRender::GpuFilterRender()
{
    mEglCore = NULL;
    mWindowSurface = NULL;
}

GpuFilterRender::~GpuFilterRender()
{

}

void GpuFilterRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_TRY_GLES3);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");
    mWindowSurface->makeCurrent();
}

void GpuFilterRender::surfaceChanged(int width, int height)
{
    this->mViewHeight = height;
    this->mViewWidth = width;
    mWindowSurface->makeCurrent();
}

void GpuFilterRender::surfaceDestroyed()
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

void GpuFilterRender::setRotationCamera(int rotation, bool flipHorizontal, bool flipVertical)
{

}

void GpuFilterRender::renderOnDraw(double elpasedInMilliSec)
{

}
