//
// Created by nicky on 2019/3/14.
//

#include "EglCore.h"
#include "../common/zzr_common.h"
// static method
void EglCore::logCurrentEglState()
{
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;

    display = eglGetCurrentDisplay();
    context = eglGetCurrentContext();
    surface = eglGetCurrentSurface(EGL_DRAW);

    LOGI("Current EGL state : display=%p, context=%p, draw_surface=%p",display,context,surface);
}



EglCore::EglCore() {
    mEglDisplay = EGL_NO_DISPLAY;
    mEglContext = EGL_NO_CONTEXT;
    mEglConfig = NULL;
    eglPresentationTimeANDROID = NULL;
    mEglVersion = -1;
    initEgl(NULL, 0);
}

EglCore::~EglCore() {
    if (mEglDisplay != EGL_NO_DISPLAY) {
        // 类析构函数 检测回收资源
        LOGW("WARNING: EglCore was not explicitly released -- state may be leaked");
        release();
    }
}

EglCore::EglCore(EGLContext sharedContext, int flags) {
    mEglDisplay = EGL_NO_DISPLAY;
    mEglContext = EGL_NO_CONTEXT;
    mEglConfig = NULL;
    initEgl(sharedContext, flags);
}




int EglCore::initEgl(EGLContext sharedContext, int flags)
{
    if (mEglDisplay != EGL_NO_DISPLAY) {
        LOGE("EGL already set up");
        return -1;
    }
    if (sharedContext == NULL) {
        sharedContext = EGL_NO_CONTEXT;
    }

    mEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(mEglDisplay == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay wrong");
        return -1;
    }

    EGLint *version = new EGLint[2];
    if (!eglInitialize(mEglDisplay, &version[0], &version[1]) ) {
        mEglDisplay = NULL;
        LOGE("unable to initialize");
        return -1;
    }

    if ((flags & FLAG_TRY_GLES3) != 0) {
        EGLConfig config = getConfig(flags, 3);
        if (config != NULL) {
            const EGLint attrib3_list[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 3,
                    EGL_NONE
            };
            EGLContext context = eglCreateContext(mEglDisplay, config, EGL_NO_CONTEXT, attrib3_list);
            if (eglGetError() == EGL_SUCCESS) {
                LOGD("Got GLES 3 config");
                mEglConfig = config;
                mEglContext = context;
                mEglVersion = 3;
            }
        }
    }
    //如果只要求GLES版本2  又或者GLES3失败了。
    if (mEglContext == EGL_NO_CONTEXT) {
        EGLConfig config = getConfig(flags, 2);
        if (config != NULL) {
            int attrib2_list[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
            };
            EGLContext context = eglCreateContext(mEglDisplay, config, sharedContext, attrib2_list);
            if (eglGetError() == EGL_SUCCESS) {
                LOGD("Got GLES 2 config");
                mEglConfig = config;
                mEglContext = context;
                mEglVersion = 2;
            }
        }
    }

    // 动态获取eglPresentationTimeANDROID方法的地址
    eglPresentationTimeANDROID = (EGL_PRESENTATION_TIME_ANDROID_PROC)
            eglGetProcAddress("eglPresentationTimeANDROID");
    if (!eglPresentationTimeANDROID) {
        LOGE("eglPresentationTimeANDROID is not available!");
    }
    return 0;
}

//释放EGL资源
void EglCore::release() {
    if (mEglDisplay != EGL_NO_DISPLAY) {
        // Android 使用一个引用计数EGLDisplay。
        // 因此，对于每个eglInitialize，我们需要一个eglTerminate。
        eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(mEglDisplay, mEglContext);
        eglReleaseThread();
        eglTerminate(mEglDisplay);
    }
    mEglDisplay = EGL_NO_DISPLAY;
    mEglContext = EGL_NO_CONTEXT;
    mEglConfig = NULL;
}

//从本地设备中寻找合适的 EGLConfig.
EGLConfig EglCore::getConfig(int flags, int version)
{
    int renderableType = EGL_OPENGL_ES2_BIT;
    if (version >= 3) {
        renderableType |= EGL_OPENGL_ES3_BIT_KHR;
    }

    int attribute_list[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_RENDERABLE_TYPE, renderableType,
            EGL_NONE, 0,      // placeholder for recordable [@-3]
            EGL_NONE
    };
    int attribute_list_length = sizeof(attribute_list) / sizeof(attribute_list[0]);
    if ((flags & FLAG_RECORDABLE) != 0) {
        attribute_list[attribute_list_length - 3] = EGL_RECORDABLE_ANDROID;
        attribute_list[attribute_list_length - 2] = 1;
    }
    EGLConfig configs = NULL;
    int numConfigs;
    if (!eglChooseConfig(mEglDisplay, attribute_list, &configs, 1, &numConfigs)) {
        LOGW("unable to find RGB8888 / %d EGLConfig", version );
        return NULL;
    }
    return configs;
}




// 创建EGLSurface
EGLSurface EglCore::createWindowSurface(ANativeWindow *surface)
{
    if (surface == NULL) {
        LOGE("ANativeWindow is NULL!");
        return NULL;
    }
    int surfaceAttribs[] = {
            EGL_NONE
    };
    EGLSurface eglSurface = eglCreateWindowSurface(mEglDisplay, mEglConfig, surface, surfaceAttribs);
    checkEglError("eglCreateWindowSurface");
    if (eglSurface == NULL) {
        LOGE("EGLSurface is NULL!");
        return NULL;
    }
    return eglSurface;
}

// 用旧版的Pbuffer，创建离屏的EGLSurface
EGLSurface EglCore::createOffscreenSurface(int width, int height) {
    int surfaceAttribs[] = {
            EGL_WIDTH, width,
            EGL_HEIGHT, height,
            EGL_NONE
    };
    EGLSurface eglSurface = eglCreatePbufferSurface(mEglDisplay, mEglConfig, surfaceAttribs);
    checkEglError("createOffscreenSurface");
    if (eglSurface == NULL) {
        LOGE("EGLSurface was null");
    }
    return eglSurface;
}

// 查询当前surface的状态值。
int EglCore::querySurface(EGLSurface eglSurface, int what)
{
    int value;
    eglQuerySurface(mEglDisplay, eglSurface, what, &value);
    return value;
}

//Makes our EGL context current, using the supplied surface for both "draw" and "read".
void EglCore::makeCurrent(EGLSurface eglSurface) {
    if (mEglDisplay == EGL_NO_DISPLAY) {
        LOGD("NOTE: makeCurrent w/o display");
    }
    if (!eglMakeCurrent(mEglDisplay, eglSurface, eglSurface, mEglContext)) {
        //TODO throw new RuntimeException("eglMakeCurrent failed");
    }
}
//Makes our EGL context current, using the supplied "draw" and "read" surfaces.
void EglCore::makeCurrent(EGLSurface drawSurface, EGLSurface readSurface) {
    if (mEglDisplay == EGL_NO_DISPLAY) {
        LOGD("NOTE: makeCurrent w/o display");
    }
    if (!eglMakeCurrent(mEglDisplay, drawSurface, readSurface, mEglContext)) {
        //TODO throw new RuntimeException("eglMakeCurrent(draw,read) failed");
    }
}

void EglCore::makeNothingCurrent() {
    if (!eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        // TODO 抛出异常
    }
}

// 交换显示
bool EglCore::swapBuffers(EGLSurface eglSurface)
{
    return static_cast<bool>(eglSwapBuffers(mEglDisplay, eglSurface));
}

//是否处于当前EGL环境
bool EglCore::isCurrent(EGLSurface eglSurface) {
    return mEglContext == eglGetCurrentContext() &&
           eglSurface == eglGetCurrentSurface(EGL_DRAW);
}

// 销毁EGLSurface
void EglCore::releaseSurface(EGLSurface eglSurface) {
    eglDestroySurface(mEglDisplay, eglSurface);
}

// 设置显示时间戳 pts
void EglCore::setPresentationTime(EGLSurface eglSurface, long nsecs) {
    if(eglPresentationTimeANDROID)
        eglPresentationTimeANDROID(mEglDisplay, eglSurface, nsecs);
    else
        LOGE("eglPresentationTimeANDROID is not available!");
}





//检查是否出错
void EglCore::checkEglError(const char *msg) {
    int error;
    if ((error = eglGetError()) != EGL_SUCCESS) {
        // TODO 抛出异常
        LOGE("%s: EGL error: %x", msg, error);
    }
}