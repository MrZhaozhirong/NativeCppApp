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

    void    createFBOTexture()
    {
        glGenTextures(1, &_texObj);
        //GLenum err = glGetError();
        //LOGE("createFBOTexture check err 1 : 0x%08x\n", err);
        glBindTexture(GL_TEXTURE_2D, _texObj);
        //err = glGetError();
        //LOGE("createFBOTexture check err 2 : 0x%08x\n", err);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //err = glGetError();
        //LOGE("createFBOTexture check err 3 : 0x%08x\n", err);
        if(_type == FBO_RGBA) {
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        }else if(_type == FBO_DEPTH) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            //glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, _width, _height);
        }
        //err = glGetError();
        //LOGE("createFBOTexture check err 4 : 0x%08x\n", err);
    }
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
        if( isInstantiation()){
            destroy();
        }
        _type = static_cast<unsigned int>(type);
        _width = static_cast<unsigned int>(w);
        _height = static_cast<unsigned int>(h);

        glGenFramebuffers(1, &_fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, _fboID);

        if( _texObj==0 ) {
            createFBOTexture();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLenum err = glGetError();
        while(err!=GL_NO_ERROR) {
            LOGE("fbo.setup.glTexImage2D : 0x%08x\n", err);
            err = glGetError();
        }
        return  true;
    }

    void    begin()
    {
        LOGE("before glBindFramebuffer: 0x%08x\n", glGetError());
        glBindFramebuffer(GL_FRAMEBUFFER, _fboID);
        LOGE("after glBindFramebuffer: 0x%08x\n", glGetError());

        if(_type == FBO_DEPTH) {
            glDrawBuffers(0, GL_NONE); // 不再绘制颜色缓冲区
            LOGE("after glDrawBuffers: 0x%08x\n", glGetError());
            glReadBuffer(GL_NONE);  // 只用来计算深度
            LOGE("after glReadBuffer: 0x%08x\n", glGetError());
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, _texObj, 0);
            LOGE("after glFramebufferTexture2D: 0x%08x\n", glGetError());
        }else if(_type == FBO_RGBA) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texObj, 0);
            LOGE("after glFramebufferTexture2D: 0x%08x\n", glGetError());
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
