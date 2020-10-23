//
// Created by nicky on 2020/1/6.
//
#include <pthread.h>
#include <string>
#include "CodecEncoder.h"
#include "../../common/zzr_common.h"
#include "../filter/GpuContrastFilter.hpp"
#include "../filter/GpuColorInvertFilter.hpp"
#include "../filter/GpuPixelationFilter.hpp"
#include "../filter/GpuBrightnessFilter.hpp"
#include "../filter/GpuHueFilter.hpp"
#include "../filter/GpuExposureFilter.hpp"
#include "../filter/GpuSaturationFilter.hpp"
#include "../filter/GpuSharpenFilter.hpp"
#include "../filter/GpuBoxBlurFilter.hpp"
#include "../filter/GpuGaussianBlurFilter.hpp"
#include "../filter/GpuGaussianBlurFilter2.hpp"
#include "../filter/GpuBilateralBlurFilter.hpp"
#include <media/NdkMediaCodec.h>
#include <sys/stat.h>

//#include "media/stagefright/MediaCodec.h"
//#include "gui/Surface.h"

CodecEncoder::CodecEncoder() {
    encoder_thread_t = 0;
    mWindowRef = NULL;
    mCodecRef = NULL;
    mWindowSurface = NULL;
    mEglCore = NULL;
    mFilter = NULL;
    mimeType = MIME_TYPE_NONE;
    mWidth = 0;
    mHeight = 0;
    mCurrentTypeId = 0;
    mRequestTypeId = 0;
    mFilterEffectPercent = 0.0f;
    //初始化互斥锁与条件变量
    pthread_mutex_init(&mutex, NULL);
    pps_sps_header = NULL;
}

CodecEncoder::~CodecEncoder() {
    releaseEglWindow();
    releaseMediaCodec();
    delete mFilter; mFilter = NULL;
    pthread_mutex_destroy(&mutex);
    if(pps_sps_header!=NULL)
        free(pps_sps_header);
}

void CodecEncoder::setMetaConfig(int mimeType, int width, int height,
                                 int iFrameInterval, int frameRate, int bitRate) {
    this->mimeType = mimeType;
    this->mWidth = width;
    this->mHeight = height;
    this->iFrameInterval = iFrameInterval;
    this->frameRate = frameRate;
    this->bitRate = bitRate;
}
//media_status_t CodecEncoder::MY_AMediaCodec_createInputSurface(AMediaCodec *mData, ANativeWindow **surface) {
//    if (surface == NULL || mData == NULL) {
//        return AMEDIA_ERROR_INVALID_PARAMETER;
//    }
//    *surface = NULL;
//    sp<IGraphicBufferProducer> igbp = NULL;
//    status_t err = mData->mCodec->createInputSurface(&igbp);
//    *surface = new Surface(igbp);
//    ANativeWindow_acquire(*surface);
//    return AMEDIA_OK;
//}

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
    mCodecRef = AMediaCodec_createEncoderByType(mime.c_str());

    AMediaFormat* format = AMediaFormat_new();
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME,   mime.c_str());
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_WIDTH,  mWidth);
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_HEIGHT, mHeight);
    // 这些参数可以扩展到上层MetaData设置
    //int32_t iFrameInterval = 3; // I帧间隔(单位秒)
    //int32_t frameRate = 15; // 帧率
    //int32_t bitRate = 8*1*1024*100; //码率bit per second(100Kb)
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_BIT_RATE, bitRate);
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_FRAME_RATE, frameRate);
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_I_FRAME_INTERVAL, iFrameInterval);
    // public static final int COLOR_FormatSurface = 0x7F000789;
    AMediaFormat_setInt32( format, AMEDIAFORMAT_KEY_COLOR_FORMAT, 0x7F000789);
#if __ANDROID_API__ >= 28
    // //Constant quality mode 忽略用户设置的码率，由编码器自己控制码率，并尽可能保证画面清晰度和码率的均衡
    // public static final int BITRATE_MODE_CQ = 0;
    // //Variable bitrate mode
    // //尽可能遵守用户设置的码率，但是会根据帧画面之间运动矢量（通俗理解就是帧与帧之间的画面变化程度）
    // //来动态调整码率，如果运动矢量较大，则在该时间段将码率调高，如果画面变换很小，则码率降低
    // public static final int BITRATE_MODE_VBR = 1;
    // //Constant bitrate mode 无论视频的画面内容如果，尽可能遵守用户设置的码率
    // public static final int BITRATE_MODE_CBR = 2;
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_BITRATE_MODE, 0); //默认1-VBR
    //以上参考链接 https://segmentfault.com/a/1190000021223837?utm_source=tag-newest
#endif
    // 解码才需要
    //AMediaFormat_setBuffer(videoFormat, "csd-0", sps, spsSize); // h264填入sps
    //AMediaFormat_setBuffer(videoFormat, "csd-1", pps, ppsSize); // h264填入pps
    //AMediaFormat_setBuffer(videoFormat, "csd-0", vps+sps+pps, total_size); // h265填入vps+sps+pps
    LOGI("AMediaCodec_configure format : %s", AMediaFormat_toString(format));
    media_status_t rc = AMediaCodec_configure(mCodecRef, format, NULL, NULL, AMEDIACODEC_CONFIGURE_FLAG_ENCODE);
    AMediaFormat_delete(format);
    LOGI("CodecEncoder AMediaCodec_configure %d ", rc);
#if __ANDROID_API__ >= 26
    // after {@link #configure} and before {@link #start}.
    media_status_t ret = AMediaCodec_createInputSurface(mCodecRef, &mWindowRef);
#else
    // 参照源码，自编写AMediaCodec_createInputSurface
    media_status_t ret = MY_AMediaCodec_createInputSurface(mCodecRef, &mWindowRef);
#endif
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
}
void CodecEncoder::releaseMediaCodec() {
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
    if( isPrepareWindow ){
        return isPrepareWindow;
    }
    if (mWindowRef == NULL) {
        LOGW("SurfaceWindow is null, Call initMediaCodec before initEglWindow. ");
        return false;
    }
    if (mEglCore == NULL)
        mEglCore = new EglCore(eglGetCurrentContext(), FLAG_TRY_GLES2|FLAG_RECORDABLE);
    if (mWindowSurface == NULL)
        mWindowSurface = new WindowSurface(mEglCore, mWindowRef, true);

    assert(mWindowSurface != NULL && mEglCore != NULL);
    isPrepareWindow = true;
    return true;
}
void CodecEncoder::releaseEglWindow() {
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


void CodecEncoder::encoderCreated() {
    if(mWidth==0||mHeight==0){
        LOGW("MetaData wrong! width*height %dx%d",mWidth,mHeight);
        return;
    }
    initMediaCodec();
    initEglWindow();
    mWindowSurface->makeCurrent();
    if( mFilter==NULL) {
        mFilter = new GpuBaseFilter();
    } else {
        mFilter->destroy();
    }
    mFilter->init();
    mRequestTypeId = mCurrentTypeId = mFilter->getTypeId();
    mWindowSurface->swapBuffers();
    //startEncode();
}
void CodecEncoder::encoderChanged(int width, int height) {
    this->mWidth = width;
    this->mHeight = height;
}
void CodecEncoder::encoderDestroyed() {
    // releaseMediaCodec();
    // releaseEglWindow();
    //在编码线程退出后调用。
    stopEncode();
}
void CodecEncoder::encoderOnDraw(GLuint mYSamplerId, GLuint mUSamplerId, GLuint mVSamplerId,
                                float* positionCords, float* textureCords) {
    mWindowSurface->makeCurrent();
    if(mCurrentTypeId!=mRequestTypeId) {
        // 更新filter
        if( mFilter!=NULL) {
            mFilter->destroy();
            delete mFilter;
            mFilter = NULL;
        }
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
            case FILTER_TYPE_BRIGHTNESS:{
                mFilter = new GpuBrightnessFilter();
            }break;
            case FILTER_TYPE_HUE:{
                mFilter = new GpuHueFilter();
            }break;
            case FILTER_TYPE_EXPOSURE:{
                mFilter = new GpuExposureFilter();
            }break;
            case FILTER_TYPE_SATURATION:{
                mFilter = new GpuSaturationFilter();
            }break;
            case FILTER_TYPE_SHARPEN:{
                mFilter = new GpuSharpenFilter();
            }break;
            case FILTER_TYPE_BOXBLUR:{
                mFilter = new GpuBoxBlurFilter();
            }break;
            case FILTER_TYPE_GAUSSIANBLUR:{
                mFilter = new GpuGaussianBlurFilter2();
            }break;
            case FILTER_TYPE_BilateralBLUR:{
                mFilter = new GpuBilateralBlurFilter();
            }break;
            default:
                mFilter = new GpuBaseFilter();
                break;
        }
        mFilter->init();
        mFilter->onOutputSizeChanged(mWidth, mHeight);
        mCurrentTypeId = mRequestTypeId;
    }
    if( mFilter!=NULL) {
        mFilter->setAdjustEffect(mFilterEffectPercent);
        mFilter->onDraw(mYSamplerId, mUSamplerId, mVSamplerId, positionCords, textureCords);
    }
    // 1s=1,000ms=1,000,000us=1,000,000,000ns
    //static int64_t count = 0;
    //count++;
    //long frame_interval = 1000000000L / frameRate;
    //long long pts = system_time_base_ns + frame_interval * count;
    //LOGI("setPresentationTime %lld", pts);
    //mWindowSurface->setPresentationTime(pts);
    mWindowSurface->swapBuffers();
}



void CodecEncoder::adjustFilterValue(int value, int max) {
    mFilterEffectPercent = (float)value / (float)max;
}
void CodecEncoder::setFilter(int filter_type_id) {
    mRequestTypeId = filter_type_id;
}


void CodecEncoder::startEncode() {
    if( encoder_thread_t == 0) {
        requestStopEncoder = false;
        pthread_create(&encoder_thread_t, NULL, onEncoderThreadStub, this);
    }
}
void CodecEncoder::stopEncode() {
    if( encoder_thread_t!= 0) {
        requestStopEncoder = true; // 请求退出解码线程
        //pthread_join(decoder_thread_t, NULL);
        encoder_thread_t = 0;
    }
}

void CodecEncoder::onEncoderThreadProc() {
    int TIMEOUT_USEC = 2000;
    bool finish = false;
    bool doRender = false;

    while ( !finish ){
        if( mCodecRef==NULL) {
            finish = true;
            continue;
        }
        try {
            if( requestStopEncoder ){
                media_status_t rc = AMediaCodec_signalEndOfInputStream(mCodecRef);
                if(isDebug) LOGI("Stop requested, send BUFFER_FLAG_END_OF_STREAM to AMediaCodecEncoder %d.", rc);
                //int buff_idx = AMediaCodec_dequeueInputBuffer(mCodecRef, TIMEOUT_USEC);
                //### Error:dequeueInputBuffer can't be used with input surface
                //if (buff_idx >= 0) {
                //    AMediaCodec_queueInputBuffer(mCodecRef,buff_idx, 0, 0, 0L, AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM);
                //}
            }
            // 从编码output队列 获取解码结果 清空解码状态
            AMediaCodecBufferInfo info;
            auto status = AMediaCodec_dequeueOutputBuffer(mCodecRef, &info, TIMEOUT_USEC);

            if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER) {
                if (isDebug) LOGV("no output available yet");
            } else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
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
                if (outputBuf == nullptr) {
                    if (isDebug) LOGW("FBI WARMING: outputBuf nullptr!");
                    continue;
                }

                if (info.flags & AMEDIACODEC_BUFFER_FLAG_CODEC_CONFIG) {
                    // 标记为BUFFER_FLAG_CODEC_CONFIG的缓冲区包含编码数据(PPS SPS)
                    if (isDebug) LOGI("capture Video BUFFER_FLAG_CODEC_CONFIG.");
                    saveConfigPPSandSPS(outputBuf, info.size);
                    if (isDebug) debugWriteOutputFile(outputBuf, info.size, false);
                } else if (info.flags & AMEDIACODEC_BUFFER_FLAG_KEY_FRAME) {
                    if (isDebug) LOGI("capture Video BUFFER_FLAG_KEY_FRAME.");
                    if (isDebug) debugWriteOutputFile(outputBuf, info.size, false);
                } else {
                    //if (isDebug) LOGI("capture Frame AMediaCodecBufferInfo.flags %d.", info.flags);
                    if (isDebug) debugWriteOutputFile(outputBuf, info.size, false);
                }
                int64_t pts = info.presentationTimeUs;
                //if (isDebug) LOGD("AMediaCodec_getOutputBuffer 容器空间BufferSize : %d", BufferSize);
                if (isDebug) LOGD("AMediaCodec_getOutputBuffer 有效数据InfoSize : %d, PTS : %lld", info.size, pts);
                AMediaCodec_releaseOutputBuffer(mCodecRef, status, doRender);
            }
        }catch(char *str) {
            LOGE("AMediaCodec_encode_thread_err : %s", str);
            finish = true;
        }
    }
    releaseMediaCodec();
    releaseEglWindow();
    if(isDebug) LOGW("Encoder thread exiting");
}
void CodecEncoder::saveConfigPPSandSPS(uint8_t* data, int32_t data_size){
    if(pps_sps_header==NULL) {
        pps_sps_header = (uint8_t *) malloc(sizeof(uint8_t)*data_size);
    } else {
        int pps_sps_header_length = sizeof(pps_sps_header)/sizeof(pps_sps_header[0]);
        if(pps_sps_header_length < data_size)
            pps_sps_header = (uint8_t *) realloc(pps_sps_header, sizeof(int8_t)*data_size);
    }
    header_size = data_size;
    memset(pps_sps_header, 0, sizeof(uint8_t)*data_size);
    memcpy(pps_sps_header, data, sizeof(uint8_t)*data_size);
}
void CodecEncoder::debugWriteOutputFile(uint8_t* data, int32_t data_size, bool bNeedPack_PPS_SPS){
    FILE *fp=fopen("/storage/emulated/0/Android/data/org.zzrblog.nativecpp/files/debug.h264","a+");
    /* 自己创建目录 确保目录有效性*/
    if(fp){
        if(bNeedPack_PPS_SPS && pps_sps_header!=NULL) {
            size_t re = fwrite(pps_sps_header, sizeof(uint8_t), header_size, fp);
        }
        fwrite(data, sizeof(uint8_t), data_size, fp);
        fflush(fp);
        fclose(fp);
    }
}
