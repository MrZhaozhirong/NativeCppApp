//
// Created by nicky on 2020/1/6.
//

#ifndef CODEC_ENCODER_H
#define CODEC_ENCODER_H


#include <cstdlib>
#include <android/native_window.h>
#include <GLES3/gl3.h>
#include "media/NdkMediaCodec.h"
#include "../../common/constructormagic.h"
#include "../../egl/EglCore.h"
#include "../../egl/WindowSurface.h"
#include "../filter/GpuBaseFilter.hpp"

#define MIME_TYPE_NONE 0x1000
#define MIME_TYPE_H264 0x1201
#define MIME_TYPE_H265 0x1202
#define AMEDIACODEC_BUFFER_FLAG_KEY_FRAME 1

class CodecEncoder {
public:
    CodecEncoder();
    ~CodecEncoder();
    // Encoder core
    void            setMetaConfig(int mimeType, int width, int height,
                                  int iFrameInterval, int frameRate, int bitRate);
    bool            initEglWindow();
    void            releaseEglWindow();
    bool            initMediaCodec();
    void            releaseMediaCodec();
    void            startEncode();
    void            stopEncode();
    // Encoder logic
    void            encoderCreated();
    void            encoderChanged(int width, int height);
    void            encoderOnDraw(GLuint mYSamplerId, GLuint mUSamplerId, GLuint mVSamplerId,
                                 float* positionCords, float* textureCords);
    void            encoderDestroyed() ;
    void            setFilter(int filter_type_id);
    void            adjustFilterValue(int value, int max);
private:
    EglCore*        mEglCore;
    WindowSurface*  mWindowSurface;
    GpuBaseFilter*  mFilter;
    int             mRequestTypeId;
    int             mCurrentTypeId;
    float           mFilterEffectPercent;

private:
    pthread_t       encoder_thread_t;
    bool            requestStopEncoder;
    pthread_mutex_t mutex;
    //encoder thread
    static void*    onEncoderThreadStub(void *p)
    {
        (static_cast<CodecEncoder*>(p))->onEncoderThreadProc();
        return NULL;
    }
    void            onEncoderThreadProc();


    int             mWidth;
    int             mHeight;
    int             mimeType; // 默认H264
    int             iFrameInterval; // I帧间隔(单位秒)
    int             frameRate; // 帧率
    int             bitRate; //码率bit per second(100Kb)
    AMediaCodec*    mCodecRef;
    ANativeWindow*  mWindowRef;
    bool            isPrepareWindow = false;
    bool            isPrepareCodec = false;

    uint8_t*        pps_sps_header;
    int             header_size;
    void            saveConfigPPSandSPS(uint8_t* data, int32_t data_size);
private:
    DISALLOW_EVIL_CONSTRUCTORS(CodecEncoder);
    bool            isDebug = false;
    void            debugWriteOutputFile(uint8_t* data, int32_t data_size, bool bNeedPack_PPS_SPS);
    media_status_t  MY_AMediaCodec_createInputSurface(AMediaCodec *mData, ANativeWindow **surface);
};


#endif // CODEC_ENCODER_H
