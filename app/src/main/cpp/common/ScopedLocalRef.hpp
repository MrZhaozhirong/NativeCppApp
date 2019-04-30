//
// Created by nicky on 2019/4/22.
//
#pragma once
#ifndef SCOPEDLOCALREF_HPP
#define SCOPEDLOCALREF_HPP

#include <jni.h>
#include "constructormagic.h"

template<typename T>
class ScopedLocalRef {
public:
    ScopedLocalRef(JNIEnv* env, T localRef)
            :mEnv(env),
             mLocalRef(localRef) {
    }

    ~ScopedLocalRef() {
        reset();
    }

    void reset(T ptr = NULL) {
        if (ptr != mLocalRef) {
            if (mLocalRef != NULL) {
                mEnv->DeleteLocalRef(mLocalRef);
            }
            mLocalRef = ptr;
        }
    }

    T release() __attribute__((warn_unused_result)) {
        T localRef = mLocalRef;
        mLocalRef = NULL;
        return localRef;
    }

    T get() const {
        return mLocalRef;
    }

private:
    JNIEnv* const mEnv;
    T mLocalRef;

    DISALLOW_EVIL_CONSTRUCTORS(ScopedLocalRef);
};

//使用方法
//void X264Encoder::onH264CallBackToJava(const unsigned char* data,
//                                       const int len, bool isIFrame)
//{
//    //onNativeCallback
//    ScopedAttachThread ats(gJavaVM);
//
//    ScopedLocalRef<jbyteArray> jbuf(ats.env(), ats.env()->NewByteArray(len));
//    ats.env()->SetByteArrayRegion(jbuf.get(), 0, len, (const jbyte*)data);
//
//    ats.env()->CallVoidMethod(mObject, mNativeCallback_mid, jbuf.get(), isIFrame );
//}

#endif // SCOPEDLOCALREF_HPP
