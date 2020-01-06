//
// Created by nicky on 2020/1/6.
//

#ifndef CODEC_ENCODER_H
#define CODEC_ENCODER_H


#include <cstdlib>
#include <android/native_window.h>
#include "media/NdkMediaCodec.h"
#include "../../common/constructormagic.h"

#define MIME_TYPE_H264 0x1201
#define MIME_TYPE_H265 0x1202

class CodecEncoder {
public:
    CodecEncoder();
    ~CodecEncoder();

    int             mimeType;
    bool            initMediaCodecr();
    void            releaseMediaCodecr();
    //encoder thread
    static void*    onEncoderThreadStub(void *p)
    {
        (static_cast<CodecEncoder*>(p))->onEncoderThreadProc();
        return NULL;
    }
    void            onEncoderThreadProc();
    void            startEncode();
    void            stopEncode();
public:
    pthread_t       encoder_thread_t;
    bool            requestStopEncoder;
    bool            isPrepare;
    //BufferPool      pool;
    pthread_mutex_t mutex;

    int             mWidth;
    int             mHeight;
    AMediaCodec*    mCodecRef;
    ANativeWindow*  mWindowRef;

private:
    DISALLOW_EVIL_CONSTRUCTORS(CodecEncoder);
    bool            isDebug = true;
};


#endif // CODEC_ENCODER_H
