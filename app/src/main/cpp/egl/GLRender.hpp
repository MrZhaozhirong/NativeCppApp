//
// Created by nicky on 2019/3/15.
//

#ifndef NATIVECPPAPP_GLRENDER_H
#define NATIVECPPAPP_GLRENDER_H


#include <android/native_window.h>
// C++接口类的注意事项
// https://blog.csdn.net/netyeaxi/article/details/80724557
class GLRender {

public:
    virtual ~GLRender() {};

    virtual void surfaceCreated(ANativeWindow *window)=0;
    virtual void surfaceChanged(int width, int height)=0;
    virtual void renderOnDraw()=0;
    virtual void surfaceDestroyed(void)=0;

};


#endif //NATIVECPPAPP_GLRENDER_H
