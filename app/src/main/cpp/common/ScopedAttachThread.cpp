#include <assert.h>
#include "ScopedAttachThread.h"
#include "zzr_common.h"


ScopedAttachThread::ScopedAttachThread(JavaVM* jvm)
        : attached_(false), jvm_(jvm), env_(NULL) {
    jint ret_val = jvm->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_6);
    if (ret_val == JNI_EDETACHED) {
        JavaVMAttachArgs args = {JNI_VERSION_1_6, NULL, NULL};
        // Attach the thread to the Java VM.
        ret_val = jvm_->AttachCurrentThread(&env_, (void*)&args);
        attached_ = (ret_val == JNI_OK);
        assert(attached_);
    }
}

ScopedAttachThread::~ScopedAttachThread() {
    int dresult = 0;
    if (attached_ && ((dresult = jvm_->DetachCurrentThread()) != JNI_OK)) {
        //    assert(false);
        LOGE("thread detach failed: %d", dresult);
    }
}

JNIEnv* ScopedAttachThread::env() { return env_; }
