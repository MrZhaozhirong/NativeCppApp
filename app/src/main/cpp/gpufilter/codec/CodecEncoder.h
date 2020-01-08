//
// Created by nicky on 2020/1/6.
//

#ifndef CODEC_ENCODER_H
#define CODEC_ENCODER_H


#include <cstdlib>
#include <android/native_window.h>
#include "media/NdkMediaCodec.h"
#include "../../common/constructormagic.h"
#include "../../egl/EglCore.h"
#include "../../egl/WindowSurface.h"

#define MIME_TYPE_H264 0x1201
#define MIME_TYPE_H265 0x1202

class CodecEncoder {
public:
    CodecEncoder();
    ~CodecEncoder();
    // Encoder core
    void            setMetaConfig(int mimeType);
    bool            initEglWindow();
    void            releaseEglWindow();
    bool            initMediaCodec();
    void            releaseMediaCodec();
    void            startEncode();
    void            stopEncode();
    // Encoder logic
    void            renderCreated(int width, int height);
    void            renderChanged(int width, int height);
    void            renderOnDraw();
    void            renderDestroyed() ;
    void            setFilter(int filter_type_id);
    void            adjustFilterValue(int value, int max);
public:
    EglCore*        mEglCore;
    WindowSurface*  mWindowSurface;

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
    AMediaCodec*    mCodecRef;
    ANativeWindow*  mWindowRef;
    bool            isPrepareWindow = false;
    bool            isPrepareCodec = false;
private:
    DISALLOW_EVIL_CONSTRUCTORS(CodecEncoder);
    bool            isDebug = true;
};


#endif // CODEC_ENCODER_H
