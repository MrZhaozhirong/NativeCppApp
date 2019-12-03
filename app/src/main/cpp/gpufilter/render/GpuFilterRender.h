//
// Created by nicky on 2019/11/28.
//

#ifndef GPU_FILTER_RENDER_H
#define GPU_FILTER_RENDER_H


#include "../../egl/GLRender.hpp"
#include "../../egl/EglCore.h"
#include "../../egl/WindowSurface.h"
#include "../components/NV21BufferPool.hpp"

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
private:
    EglCore*        mEglCore;
    WindowSurface*  mWindowSurface;
    // 帧图缓存池
    NV21BufferPool  mNV21Pool;
    // surface宽高
    int             mViewWidth;
    int             mViewHeight;
    // 预览帧宽高
    int             mFrameWidth;
    int             mFrameHeight;
    // 是否水平垂直翻转
    bool            mFlipHorizontal;
    bool            mFlipVertical;
    int             mRotation; // 0,90,180,270

    float*          positionCords;
    float*          textureCords;
private:
    DISALLOW_EVIL_CONSTRUCTORS(GpuFilterRender);

    void    adjustFrameScaling();
    void    generateFrameTextureCords(int rotation, bool flipHorizontal, bool flipVertical);
    void    generateFramePositionCords();

    float   flip(float value)
    {
        if(value ==0.0f)
            return 1.0f;
        else // == 1.0f
            return 0.0f;
    };
    float   addDistance(float coordinate, float distance)
    {
        return coordinate == 0.0f ? distance : 1 - distance;
    }
};


#endif // GPU_FILTER_RENDER_H
