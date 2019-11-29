//
// Created by nicky on 2019/11/28.
//

#include <assert.h>
#include "GpuFilterRender.h"
#include "../../common/zzr_common.h"
#include "../components/ByteBuffer.hpp"
#include "../../common/ByteBuffer.hpp"

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

void GpuFilterRender::feedVideoData(int8_t *data, int data_len, int previewWidth, int previewHeight)
{
    mFrameWidth = previewWidth;
    mFrameHeight = previewHeight;

    int size = previewWidth * previewHeight;
    int y_len = size;   // mWidth*mHeight
    int u_len = size / 4;   // mWidth*mHeight / 4
    int v_len = size / 4;   // mWidth*mHeight / 4
    // nv21数据中 y占1个width*mHeight，uv各占1/4个width*mHeight 共 3/2个width*mHeight
    if(data_len < y_len+u_len+v_len)
        return;
    ByteBuffer* p = new ByteBuffer(static_cast<size_t>(data_len));
    p->param1 = y_len;
    p->param2 = u_len;
    p->param3 = v_len;
    p->wrap(data, static_cast<size_t>(data_len));
    mNV21Pool.put(p);
}


void GpuFilterRender::setRotationCamera(int rotation, bool flipHorizontal, bool flipVertical)
{
    this->mRotation = rotation;
    this->mFlipHorizontal = flipHorizontal;
    this->mFlipVertical = flipVertical;
    adjustImageScaling();
}
void GpuFilterRender::adjustImageScaling()
{
    float outputWidth = mViewWidth;
    float outputHeight = mViewHeight;
    if (mRotation == ROTATION_270 || mRotation == ROTATION_90) {
        outputWidth = mViewHeight;
        outputHeight = mViewWidth;
    }

    float ratio1 = outputWidth / mFrameWidth;
    float ratio2 = outputHeight / mFrameHeight;
    // float ratioMax = Math.max(ratio1, ratio2);
}

void GpuFilterRender::renderOnDraw(double elpasedInMilliSec)
{

}




