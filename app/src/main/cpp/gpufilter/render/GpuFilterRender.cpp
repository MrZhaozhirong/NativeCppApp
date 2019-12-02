//
// Created by nicky on 2019/11/28.
//

#include <assert.h>
#include "GpuFilterRender.h"
#include "../../common/zzr_common.h"
#include "../../common/ByteBuffer.hpp"

GpuFilterRender::GpuFilterRender()
{
    mEglCore = NULL;
    mWindowSurface = NULL;
    positionCords = new float[8]{0};
    textureCords = new float[8]{0};
}

GpuFilterRender::~GpuFilterRender()
{
    delete positionCords;
    positionCords = NULL;
    delete textureCords;
    textureCords = NULL;
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
    adjustFrameScaling();
}
void GpuFilterRender::adjustFrameScaling()
{
    float outputWidth = mViewWidth;
    float outputHeight = mViewHeight;
    if (mRotation == ROTATION_270 || mRotation == ROTATION_90) {
        outputWidth = mViewHeight;
        outputHeight = mViewWidth;
    }

    float ratio1 = outputWidth / mFrameWidth;
    float ratio2 = outputHeight / mFrameHeight;
    float ratioMax = std::max(ratio1, ratio2);

    int imageWidthNew = static_cast<int>(mFrameWidth * ratioMax);
    int imageHeightNew = static_cast<int>(mFrameHeight * ratioMax);

    float ratioWidth = imageWidthNew / outputWidth;
    float ratioHeight = imageHeightNew / outputHeight;

    //if (GPUImage.ScaleType.CENTER_CROP)
    {
        float distHorizontal = (1 - 1 / ratioWidth) / 2;
        float distVertical = (1 - 1 / ratioHeight) / 2;
        generateFrameTextureCords(mRotation, mFlipHorizontal, mFlipVertical);
        textureCords[0] = addDistance(textureCords[0], distHorizontal);
        textureCords[2] = addDistance(textureCords[2], distHorizontal);
        textureCords[4] = addDistance(textureCords[4], distHorizontal);
        textureCords[6] = addDistance(textureCords[6], distHorizontal);

        textureCords[1] = addDistance(textureCords[1], distVertical);
        textureCords[3] = addDistance(textureCords[3], distVertical);
        textureCords[5] = addDistance(textureCords[5], distVertical);
        textureCords[7] = addDistance(textureCords[7], distVertical);
    }
    //if (GPUImage.ScaleType.CENTER_INSIDE)
    {
        float cube[8] = {
                // position   x, y
                -1.0f, -1.0f,   //左下
                1.0f, -1.0f,    //右下
                -1.0f, 1.0f,    //左上
                1.0f, 1.0f,     //右上
        };
        positionCords[0] = cube[0] / ratioHeight;
        positionCords[2] = cube[2] / ratioHeight;
        positionCords[4] = cube[4] / ratioHeight;
        positionCords[6] = cube[6] / ratioHeight;

        positionCords[1] = cube[1] / ratioWidth;
        positionCords[3] = cube[3] / ratioWidth;
        positionCords[5] = cube[5] / ratioWidth;
        positionCords[7] = cube[7] / ratioWidth;
    }
}

void GpuFilterRender::generateFrameTextureCords(int rotation, bool flipHorizontal, bool flipVertical)
{
    float tempTex[8]={0};
    switch (rotation)
    {
        case ROTATION_90:{
            float rotatedTex[8] = {
                    1.0f, 1.0f,
                    1.0f, 0.0f,
                    0.0f, 1.0f,
                    0.0f, 0.0f,
            };
            memcpy(tempTex, rotatedTex, sizeof(rotatedTex));
        }break;
        case ROTATION_180:{
            float rotatedTex[8] = {
                    1.0f, 0.0f,
                    0.0f, 0.0f,
                    1.0f, 1.0f,
                    0.0f, 1.0f,
            };
            memcpy(tempTex, rotatedTex, sizeof(rotatedTex));
        }break;
        case ROTATION_270:{
            float rotatedTex[8] = {
                    0.0f, 0.0f,
                    0.0f, 1.0f,
                    1.0f, 0.0f,
                    1.0f, 1.0f,
            };
            memcpy(tempTex, rotatedTex, sizeof(rotatedTex));
        }break;
        default:
        case ROTATION_0:{
            float rotatedTex[8] = {
                    0.0f, 1.0f,
                    1.0f, 1.0f,
                    0.0f, 0.0f,
                    1.0f, 0.0f,
            };
            memcpy(tempTex, rotatedTex, sizeof(rotatedTex));
        }break;
    }
    if (flipHorizontal) {
        tempTex[0] = flip(tempTex[0]);
        tempTex[2] = flip(tempTex[2]);
        tempTex[4] = flip(tempTex[4]);
        tempTex[6] = flip(tempTex[6]);
    }
    if (flipVertical) {
        tempTex[1] = flip(tempTex[1]);
        tempTex[3] = flip(tempTex[3]);
        tempTex[5] = flip(tempTex[5]);
        tempTex[7] = flip(tempTex[7]);
    }
    memset(textureCords, 0, sizeof(textureCords));
    memcpy(textureCords, tempTex, sizeof(tempTex));
}


void GpuFilterRender::renderOnDraw(double elpasedInMilliSec)
{

}



