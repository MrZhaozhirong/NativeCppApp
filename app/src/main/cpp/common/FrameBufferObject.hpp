//
// Created by nicky on 2019/8/6.
//
#pragma once
#ifndef FRAME_BUFFER_OBJECT_HPP
#define FRAME_BUFFER_OBJECT_HPP

#define FBO_NONE  0x1010
#define FBO_RGBA  0x1011
#define FBO_DEPTH 0x1012

#include <GLES3/gl3.h>
#include "zzr_common.h"


class   FrameBufferObject
{

public:
    unsigned int   _width;
    unsigned int   _height;
    unsigned int   _fboID;
private:
    unsigned int   _texObj;
    unsigned int   _type;
public:
    FrameBufferObject()
    {
        _width  = 0;
        _height = 0;
        _fboID  = 0;
        _texObj = 0;
        _type   = FBO_NONE;
    }

    int getTextureId() {
        return _texObj;
    }
    bool isInstantiation() {
        return _width!=0||_height!=0;
    }

    bool    setup(int w, int h, int type)
    {
        _type = static_cast<unsigned int>(type);
        _width = static_cast<unsigned int>(w);
        _height = static_cast<unsigned int>(h);
        glGenFramebuffers(1, &_fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, _fboID);

        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &_texObj);
        glBindTexture(GL_TEXTURE_2D, _texObj);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if(_type == FBO_DEPTH) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }else if(_type == FBO_RGBA) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return  true;
    }

    void    begin()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fboID);

        if(_type == FBO_DEPTH) {
            glDrawBuffers(0, NULL); // 不再绘制颜色缓冲区
            glReadBuffer(GL_NONE);  // 只用来计算深度
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _texObj, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
        }else if(_type == FBO_RGBA) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texObj, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        GLint err = glGetError();
        if (err != GL_NO_ERROR) {
            LOGE("GL error after fbo.begin(): 0x%08x\n", err);
        }
    }

    void    end()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    /**
    *   销毁
    */
    void    destroy()
    {
        glDeleteTextures(1,&_texObj);
        glDeleteFramebuffers(1, &_fboID);
        _texObj =   0;
        _fboID  =   0;
        _type   =   FBO_NONE;
    }
};
#endif // FRAME_BUFFER_OBJECT_HPP
