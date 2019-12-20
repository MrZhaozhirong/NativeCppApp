//
// Created by nicky on 2019/11/28.
//

#ifndef GPU_FILTER_RENDER_H
#define GPU_FILTER_RENDER_H


#include <GLES3/gl3.h>
#include "../../egl/GLRender.hpp"
#include "../../egl/EglCore.h"
#include "../../egl/WindowSurface.h"
#include "../components/NV21BufferPool.hpp"
#include "../filter/GpuBaseFilter.hpp"
#include "../filter/GpuContrastFilter.hpp"

#define ROTATION_0      0
#define ROTATION_90     90
#define ROTATION_180    180
#define ROTATION_270    270

class GpuFilterRender : public GLRender{
public:
    GpuFilterRender();
    ~GpuFilterRender();

    void surfaceCreated(ANativeWindow *window) ;
    void surfaceChanged(int width, int height) ;
    void renderOnDraw(double elpasedInMilliSec) ;
    void surfaceDestroyed(void) ;

    void setRotationCamera(int rotation, bool flipHorizontal, bool flipVertical) ;
    void feedVideoData(int8_t* data, int data_len, int previewWidth, int previewHeight);
    void setFilter(int filter_type_id);
    void adjustFilterValue(int value, int max);
private:
    EglCore*        mEglCore;
    WindowSurface*  mWindowSurface;
    // Filter滤镜
    GpuBaseFilter*  mFilter;
    int             mRequestTypeId;
    int             mFilterTypeId;
    float           mFilterEffectPercent;
    // 帧图缓存
    NV21BufferPool  mNV21Pool;
    pthread_mutex_t mutex;
    ByteBuffer*     i420BufferY;
    ByteBuffer*     i420BufferU;
    ByteBuffer*     i420BufferV;
    int             yTextureId;
    int             uTextureId;
    int             vTextureId;
    // surface宽高
    int             mViewWidth;
    int             mViewHeight;
    // 预览帧图宽高
    int             mFrameWidth;
    int             mFrameHeight;
    // 相机：是否水平垂直翻转，
    bool            mFlipHorizontal;
    bool            mFlipVertical;
    // 相机：初始旋转角
    int             mRotation; // 0,90,180,270

    float*          positionCords;
    float*          textureCords;
private:
    DISALLOW_EVIL_CONSTRUCTORS(GpuFilterRender);

    void    adjustFrameScaling();
    void    generateFrameTextureCords(int rotation, bool flipHorizontal, bool flipVertical);
    void    generateFramePositionCords();

    GLuint  updateTexture(int8_t* src, GLuint texId, int width, int height);
    void    checkFilterChange();

    __inline float flip(float value)
    {
        if(value ==0.0f)
            return 1.0f;
        else // == 1.0f
            return 0.0f;
    };
    __inline float addDistance(float coordinate, float distance)
    {
        return coordinate == 0.0f ? distance : 1 - distance;
    };
};


#endif // GPU_FILTER_RENDER_H
