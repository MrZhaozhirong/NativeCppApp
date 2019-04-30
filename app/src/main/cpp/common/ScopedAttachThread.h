//
// Created by nicky on 2019/4/22.
//

#ifndef SCOPEDATTACHTHREAD_H
#define SCOPEDATTACHTHREAD_H


#include <jni.h>

class ScopedAttachThread {
public:
    explicit ScopedAttachThread(JavaVM* jvm);
    ~ScopedAttachThread();
    JNIEnv* env();

private:
    bool attached_;
    JavaVM* jvm_;
    JNIEnv* env_;
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

#endif //CODECTEST2_SCOPEDATTACHTHREAD_H
