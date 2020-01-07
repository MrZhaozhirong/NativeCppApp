//
// Created by nicky on 2020/1/6.
//

#include <pthread.h>
#include <string>
#include "CodecEncoder.h"
#include "../../common/zzr_common.h"
#include <media/NdkMediaCodec.h>

CodecEncoder::CodecEncoder() {
    encoder_thread_t = 0;
    mWindowRef = NULL;
    mCodecRef = NULL;
    mWindowSurface = NULL;
    mEglCore = NULL;
}

CodecEncoder::~CodecEncoder() {
    releaseEglWindow();
    releaseMediaCodec();
}

void CodecEncoder::setMetaData(int width, int height, int mimeType) {
    this->mWidth = width;
    this->mHeight = height;
    this->mimeType = mimeType;
}

bool CodecEncoder::initMediaCodec() {
    if( isPrepareCodec ){
        return isPrepareCodec;
    }
    // Java.MediaFormat.MIMETYPE_VIDEO_AVC = "video/avc";
    // Java.MediaFormat.MIMETYPE_VIDEO_HEVC = "video/hevc";
    std::string mime;
    if(mimeType == MIME_TYPE_H264) {
        mime = "video/avc";
    }
    if(mimeType == MIME_TYPE_H265) {
        mime = "video/hevc";
    }
    AMediaFormat* format = AMediaFormat_new();
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME,   mime.c_str());
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_WIDTH,  mWidth);
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_HEIGHT, mHeight);
    // 这些参数以后可以扩展到上层设置
    int32_t iFrameInterval = 5; // I帧间隔(单位秒)
    int32_t frameRate = 15; // 帧率
    int32_t bitRate = 8*1*1024*25; //码率bit per second(25Kb)
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_BIT_RATE, bitRate);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_FRAME_RATE, frameRate);
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, iFrameInterval);
    // COLOR_FormatSurface indicates that the data will be a GraphicBuffer metadata reference.
    //public static final int COLOR_FormatSurface = 0x7F000789;
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_COLOR_FORMAT, 0x7F000789);
    // 解码需要
    //AMediaFormat_setBuffer(videoFormat, "csd-0", sps, spsSize); // h264填入sps
    //AMediaFormat_setBuffer(videoFormat, "csd-1", pps, ppsSize); // h264填入pps
    //AMediaFormat_setBuffer(videoFormat, "csd-0", vps+sps+pps, total_size); // h265填入vps+sps+pps
    mCodecRef = AMediaCodec_createEncoderByType(mime.c_str());
    media_status_t rc = AMediaCodec_configure(mCodecRef, format, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    AMediaFormat_delete(format);
    LOGW("CodecEncoder AMediaCodec_configure %d ", rc);

#if __ANDROID_API__ >= 26
    // after {@link #configure} and before {@link #start}.
    media_status_t ret = AMediaCodec_createInputSurface(mCodecRef, &mWindowRef);
    if(AMEDIA_OK == ret) {
        ret = AMediaCodec_start(mCodecRef);
        LOGI("CodecEncoder AMediaCodec_start %d ",ret);
        isPrepareCodec = true;
        return (ret==AMEDIA_OK);
    } else {
        LOGW("CodecEncoder AMediaCodec_createInputSurface != OK. ");
        AMediaCodec_delete(mCodecRef);
        isPrepareCodec = false;
        return false;
    }
#else
    //TUDO native source create input surface
    AMediaCodec_start(mCodecRef);
    isPrepareCodec = true;
    return true;
#endif
}

void CodecEncoder::releaseMediaCodec() {
    stopEncode();
    try{
        if( mCodecRef ) {
            AMediaCodec_flush(mCodecRef);
            AMediaCodec_stop(mCodecRef);
            AMediaCodec_delete(mCodecRef);
            mCodecRef = NULL;
        }
    }catch (char *str) {
        LOGE("releaseMediaCodec_err : %s", str);
    }
    isPrepareCodec = false;
    LOGI("CodecEncoder release.");
}

bool CodecEncoder::initEglWindow() {
    if (mWindowRef == NULL) {
        LOGW("SurfaceWindow is null, Call initMediaCodec before initEglWindow. ");
        return false;
    }
    if (mEglCore == NULL)
        mEglCore = new EglCore(NULL, FLAG_TRY_GLES2);
    if (mWindowSurface == NULL)
        mWindowSurface = new WindowSurface(mEglCore, mWindowRef, true);

    assert(mWindowSurface != NULL && mEglCore != NULL);
    isPrepareWindow = true;
    return true;
}
void CodecEncoder::releaseEglWindow() {
    if( mWindowRef) {
        ANativeWindow_release(mWindowRef);
        mWindowRef = NULL;
    }
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
    isPrepareWindow = false;
}

void CodecEncoder::startEncode() {
    if( encoder_thread_t == 0) {
        //初始化互斥锁与条件变量
        pthread_mutex_init(&mutex, NULL);
        requestStopEncoder = false;
        pthread_create(&encoder_thread_t, NULL, onEncoderThreadStub, this);
    }
}
void CodecEncoder::stopEncode() {
    if( encoder_thread_t!= 0) {
        requestStopEncoder = true; // 请求退出解码线程
        //pthread_join(decoder_thread_t, NULL);
        pthread_mutex_destroy(&mutex);
        encoder_thread_t = 0;
    }
}

void CodecEncoder::onEncoderThreadProc() {
    int TIMEOUT_USEC = 2000;
    bool finish = false;
    bool doRender = false;

    long buff_idx;
    while ( !finish ){
        if( mCodecRef==NULL) {
            finish = true;
            continue;
        }
        try {
            if( requestStopEncoder ){
                if(isDebug) LOGI("Stop requested, send BUFFER_FLAG_END_OF_STREAM to AMediaCodec Encoder");
                buff_idx = AMediaCodec_dequeueInputBuffer(mCodecRef, TIMEOUT_USEC);
                if (buff_idx >= 0) {
                    AMediaCodec_queueInputBuffer(mCodecRef, buff_idx, 0, 0, 0L,
                                                 AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                }
            } else {
                // 等待输入 filter.draw到AMediaCodec->createInputSurface的mWindowRef
            }

            // 从编码output队列 获取解码结果 清空解码状态
            AMediaCodecBufferInfo info;
            auto status = AMediaCodec_dequeueOutputBuffer(mCodecRef, &info, TIMEOUT_USEC);
            if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
                //if (isDebug) LOGI("no output available yet");
            } else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
                // not important for us, since we're using Surface
                if (isDebug) LOGI("encoder output buffers changed");
            } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
                auto format = AMediaCodec_getOutputFormat(mCodecRef);
                if (isDebug) LOGI("format changed to: %s", AMediaFormat_toString(format));
                AMediaFormat_delete(format);
            } else if (status < 0) {
                if (isDebug) LOGE("unexpected result from encoder.dequeueOutputBuffer: " + status);
            } else {
                if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
                    if (isDebug) LOGI("output EOS");
                    finish = true;
                }
                size_t BufferSize;
                uint8_t* outputBuf = AMediaCodec_getOutputBuffer(mCodecRef, status, &BufferSize);
                if (outputBuf != nullptr) {
                    //int64_t pts32_us = info.presentationTimeUs;
                    LOGD("AMediaCodec_getOutputBuffer size : %d", info.size);
                    //## memcpy(dst, outputBuf, outsize);
                    //AMediaCodec_releaseOutputBuffer(mCodecRef, status, info.size != 0);
                    AMediaCodec_releaseOutputBuffer(mCodecRef, status, doRender);
                }
            }
        }catch(char *str) {
            LOGE("AMediaCodec_encode_thread_err : %s", str);
            finish = true;
        }
    }

    if(isDebug) LOGW("Encoder thread exiting");
}