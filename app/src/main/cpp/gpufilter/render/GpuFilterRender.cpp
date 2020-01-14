//
// Created by nicky on 2019/11/28.
//

#include <assert.h>
#include <pthread.h>
#include "GpuFilterRender.h"
#include "../../common/zzr_common.h"
#include "../../common/ByteBuffer.hpp"
#include "../filter/GpuColorInvertFilter.hpp"
#include "../filter/GpuPixelationFilter.hpp"

GpuFilterRender::GpuFilterRender() {
    pthread_mutex_init(&mutex, NULL);
    mEglCore = NULL;
    mWindowSurface = NULL;
    positionCords = new float[8]{0};
    textureCords = new float[8]{0};
    i420BufferY = NULL;
    i420BufferU = NULL;
    i420BufferV = NULL;
    yTextureId = -1;
    uTextureId = -1;
    vTextureId = -1;
    mFilter = NULL;
    mCurrentTypeId = 0;
    mRequestTypeId = 0;
    mFilterEffectPercent = 0.0f;
}
GpuFilterRender::~GpuFilterRender() {
    pthread_mutex_destroy(&mutex);
    delete mEglCore;
    mEglCore = NULL;
    delete mWindowSurface;
    mWindowSurface = NULL;
    delete positionCords;
    positionCords = NULL;
    delete textureCords;
    textureCords = NULL;
    delete i420BufferY; i420BufferY=NULL;
    delete i420BufferU; i420BufferU=NULL;
    delete i420BufferV; i420BufferV=NULL;
    delete mFilter; mFilter = NULL;
}

void GpuFilterRender::surfaceCreated(ANativeWindow *window)
{
    if (mEglCore == NULL) {
        mEglCore = new EglCore(NULL, FLAG_TRY_GLES2);
    }
    mWindowSurface = new WindowSurface(mEglCore, window, true);
    assert(mWindowSurface != NULL && mEglCore != NULL);
    LOGD("render surface create ... ");
    mWindowSurface->makeCurrent();
    if( mFilter==NULL) {
        mFilter = new GpuBaseFilter();
    } else {
        mFilter->destroy();
    }
    mFilter->init();
    mRequestTypeId = mCurrentTypeId = mFilter->getTypeId();
    mWindowSurface->swapBuffers();

    int32_t windowWidth = ANativeWindow_getWidth(window);
    int32_t windowHeight = ANativeWindow_getHeight(window);
    // 和surfaceChanged传入的width和height是一样的，但是这里就可以提前激活AMediaCodec
    mEncoder.renderCreated(windowWidth, windowHeight);
}

void GpuFilterRender::surfaceChanged(int width, int height)
{
    this->mViewWidth = width;
    this->mViewHeight = height;
    mWindowSurface->makeCurrent();
    mFilter->onOutputSizeChanged(width, height);
    mWindowSurface->swapBuffers();
    mEncoder.renderChanged(width, height);
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
    mEncoder.renderDestroyed();
}

void GpuFilterRender::feedVideoData(int8_t *data, int data_len, int previewWidth, int previewHeight)
{
    if( mFrameWidth != previewWidth){
        mFrameWidth  = previewWidth;
        mFrameHeight = previewHeight;
        adjustFrameScaling();
    }
    int size = previewWidth * previewHeight;
    int y_len = size;   // mWidth*mHeight
    int u_len = size / 4;   // mWidth*mHeight / 4
    int v_len = size / 4;   // mWidth*mHeight / 4
    // nv21数据中 y占1个width*mHeight，uv各占1/4个width*mHeight 共 3/2个width*mHeight
    if(data_len < y_len+u_len+v_len)
        return;

    pthread_mutex_lock(&mutex);
    ByteBuffer* p = new ByteBuffer(data_len);
    p->param1 = y_len;
    p->param2 = u_len;
    p->param3 = v_len;
    p->wrap(data, data_len);
    mNV21Pool.put(p);

    // nv21->i420的数据容器，用于renderOnDraw的渲染
    if( i420BufferY== NULL)
    {
        i420BufferY = new ByteBuffer(y_len);
        i420BufferU = new ByteBuffer(u_len);
        i420BufferV = new ByteBuffer(v_len);
    }
    pthread_mutex_unlock(&mutex);
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

    generateFramePositionCords();
    generateFrameTextureCords(mRotation, mFlipHorizontal, mFlipVertical);
    // 根据效果调整位置坐标or纹理坐标
    //if (GPUImage.ScaleType.CENTER_CROP)
    {
        float distHorizontal = (1 - 1 / ratioWidth) / 2;
        float distVertical = (1 - 1 / ratioHeight) / 2;
        textureCords[0] = addDistance(textureCords[0], distHorizontal); // x
        textureCords[1] = addDistance(textureCords[1], distVertical);   // y
        textureCords[2] = addDistance(textureCords[2], distHorizontal);
        textureCords[3] = addDistance(textureCords[3], distVertical);
        textureCords[4] = addDistance(textureCords[4], distHorizontal);
        textureCords[5] = addDistance(textureCords[5], distVertical);
        textureCords[6] = addDistance(textureCords[6], distHorizontal);
        textureCords[7] = addDistance(textureCords[7], distVertical);
    }
    //if (GPUImage.ScaleType.CENTER_INSIDE)
    //{
    //    positionCords[0] = positionCords[0] / ratioHeight;
    //    positionCords[1] = positionCords[1] / ratioWidth;
    //    positionCords[2] = positionCords[2] / ratioHeight;
    //    positionCords[3] = positionCords[3] / ratioWidth;
    //    positionCords[4] = positionCords[4] / ratioHeight;
    //    positionCords[5] = positionCords[5] / ratioWidth;
    //    positionCords[6] = positionCords[6] / ratioHeight;
    //    positionCords[7] = positionCords[7] / ratioWidth;
    //}

}
void GpuFilterRender::generateFramePositionCords()
{
    float cube[8] = {
            // position   x, y
            -1.0f, -1.0f,   //左下
            1.0f, -1.0f,    //右下
            -1.0f, 1.0f,    //左上
            1.0f, 1.0f,     //右上
    };
    memset(positionCords, 0, sizeof(positionCords));
    memcpy(positionCords, cube, sizeof(cube));
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


void GpuFilterRender::setFilter(int filter_type_id) {
    mRequestTypeId = filter_type_id;
    // 在 checkFilterChange 方法更新mFilterTypeId
    if(mCurrentTypeId!=mRequestTypeId) {
        // mFilter->destroy(); 非GL线程，不执行GL语句
        delete mFilter;
        mFilter = NULL;
        switch (mRequestTypeId)
        {
            case FILTER_TYPE_NORMAL: {
                mFilter = new GpuBaseFilter();
            }break;
            case FILTER_TYPE_CONTRAST:{
                mFilter = new GpuContrastFilter();
            }break;
            case FILTER_TYPE_COLOR_INVERT:{
                mFilter = new GpuColorInvertFilter();
            }break;
            case FILTER_TYPE_PIXELATION:{
                mFilter = new GpuPixelationFilter();
            }break;
            default:
                mFilter = new GpuBaseFilter();
                break;
        }
    }
    mEncoder.setFilter(filter_type_id);
}
void GpuFilterRender::adjustFilterValue(int value, int max) {
    mFilterEffectPercent = (float)value / (float)max;
    //LOGD("GpuFilterRender adjust %f", mFilterEffectPercent);
    mEncoder.adjustFilterValue(value, max);
}
void GpuFilterRender::checkFilterChange() {
    if(mCurrentTypeId!=mRequestTypeId) {
        if( mFilter!=NULL) {
            mFilter->init();
            mFilter->onOutputSizeChanged(mViewWidth, mViewHeight);
            mCurrentTypeId = mRequestTypeId;
        }
    }
}
void GpuFilterRender::renderOnDraw(double elpasedInMilliSec)
{
    if (mEglCore == NULL || mWindowSurface == NULL) {
        LOGW("Skipping drawFrame after shutdown");
        return;
    }
    pthread_mutex_lock(&mutex);
    ByteBuffer* item = mNV21Pool.get();
    if(item == NULL) {
        pthread_mutex_unlock(&mutex);
        return;
    } else { // item!=NULL，i420BufferY/U/V也!=NULL
        int8_t * nv21_buffer = item->data();
        int            y_len = item->param1;
        int            u_len = item->param2;
        int            v_len = item->param3;
        // 装填y u v数据。
        int8_t * dst_y = i420BufferY->data();
        int8_t * dst_u = i420BufferU->data();
        int8_t * dst_v = i420BufferV->data();
        memcpy(dst_y, nv21_buffer, (size_t) y_len);
        for (int i = 0; i < u_len; i++) {
            //NV21 先v后u
            *(dst_v + i) = (uint8_t) *(nv21_buffer + y_len + i * 2);
            *(dst_u + i) = (uint8_t) *(nv21_buffer + y_len + i * 2 + 1);
        }
        // 删除BufferPool当中的引用。
        delete item;
        pthread_mutex_unlock(&mutex);//#赶紧解锁
        // 画面渲染
        mWindowSurface->makeCurrent();
        yTextureId = updateTexture(dst_y, yTextureId, mFrameWidth, mFrameHeight);
        uTextureId = updateTexture(dst_u, uTextureId, mFrameWidth/2, mFrameHeight/2);
        vTextureId = updateTexture(dst_v, vTextureId, mFrameWidth/2, mFrameHeight/2);
        checkFilterChange();
        if( mFilter!=NULL) {
            mFilter->setAdjustEffect(mFilterEffectPercent);
            mFilter->onDraw(yTextureId, uTextureId, vTextureId, positionCords, textureCords);
        }
        mWindowSurface->swapBuffers();
        // 视频录制
        mEncoder.renderOnDraw(yTextureId, uTextureId, vTextureId, positionCords, textureCords);
    }
}
GLuint GpuFilterRender::updateTexture(int8_t *src, int texId, int width, int height)
{
    GLuint mTextureID;
    if( texId == -1) {
        glGenTextures(1, &mTextureID);
        glBindTexture(GL_TEXTURE_2D, mTextureID);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height,
                     0, GL_LUMINANCE, GL_UNSIGNED_BYTE, src);
    } else {
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        GL_LUMINANCE, GL_UNSIGNED_BYTE, src);
        mTextureID = texId;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return mTextureID;
}




