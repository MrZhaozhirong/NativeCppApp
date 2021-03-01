//
// Created by nicky on 2019/8/6.
//
#pragma once
#ifndef FRAME_BUFFER_OBJECT_HPP
#define FRAME_BUFFER_OBJECT_HPP

#define FBO_NONE  0x0000
#define FBO_RGBA  0x0001
#define FBO_DEPTH 0x0002
#define FBO_MSAA  0x0003

#include <GLES3/gl3.h>
#include <GLES3/gl32.h>
#include "zzr_common.h"


class   FrameBufferObject
{

public:
    unsigned int   _width;
    unsigned int   _height;
    unsigned int   _fboID;
private:
    unsigned int   _rgbaTexId;
    unsigned int   _depthTexId;
    unsigned int   _depthRboId;
    unsigned int   _multisampleTexId;
    unsigned int   _multisampleRboId;
    unsigned int   _type;

    void    createRgbaTexture()
    {
        glGenTextures(1, &_rgbaTexId);
        //LOGE("createFBOTexture check err 1 : 0x%08x\n", glGetError());
        glBindTexture(GL_TEXTURE_2D, _rgbaTexId);
        //LOGE("createFBOTexture check err 2 : 0x%08x\n", glGetError());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //LOGE("createFBOTexture check err 3 : 0x%08x\n", glGetError());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        //LOGE("createFBOTexture check err 4 : 0x%08x\n", glGetError());
    }

    void    createDepthTexture()
    {
        glGenTextures(1, &_depthTexId);
        //LOGE("createDepthTexture check err 1 : 0x%08x\n", glGetError());
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        //LOGE("createDepthTexture check err 2 : 0x%08x\n", glGetError());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //LOGE("createDepthTexture check err 3 : 0x%08x\n", glGetError());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
        //LOGE("createDepthTexture check err 4 : 0x%08x\n", glGetError());

        //EGL配置 EGL_DEPTH_SIZE 16
        // GL_BYTE / GL_UNSIGNED_BYTE
        // GL_SHORT / GL_UNSIGNED_SHORT
        // GL_INT / GL_UNSIGNED_INT
        // GL_FLOAT

        //GL_DEPTH_COMPONENT -> GL_UNSIGNED_SHORT/GL_UNSIGNED_INT 才能生成depth深度位图
    }

    void    createDepthRenderBuffer()
    {
        glGenRenderbuffers( 1, &_depthRboId );
        glBindRenderbuffer( GL_RENDERBUFFER, _depthRboId );
        glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _width, _height );
        LOGE("glRenderbufferStorage check err : 0x%08x\n", glGetError());
    }

    void createMultisampleTexture()
    {
        glGenTextures(1, &_multisampleTexId);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _multisampleTexId);
        // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, _width, _height, GL_TRUE);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, _width, _height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    }

    void createMultisampleRenderBuffer()
    {
        glGenRenderbuffers(1, &_multisampleRboId);
        glBindRenderbuffer(GL_RENDERBUFFER, _multisampleRboId);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, _width, _height);
    }
public:
    FrameBufferObject()
    {
        _width  = 0;
        _height = 0;
        _fboID  = 0;
        _rgbaTexId = 0;
        _depthTexId = 0;
        _depthRboId = 0;
        _type   = FBO_NONE;
    }

    int getDepthTexId() {
        return _depthTexId;
    }
    int getRgbaTexId() {
        return _rgbaTexId;
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

        if(_type == FBO_DEPTH) {
            //createDepthRenderBuffer();
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRboId);
            createDepthTexture();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexId, 0);
            //createRgbaTexture();
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rgbaTexId, 0);
        }
        if(_type == FBO_RGBA) {
            createRgbaTexture();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rgbaTexId, 0);
            //LOGE("after glFramebufferTexture2D : 0x%08x\n", glGetError());
            createDepthRenderBuffer();
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRboId);
            //LOGE("after glFramebufferRenderbuffer : 0x%08x\n", glGetError());
        }

        // https://blog.csdn.net/cui6864520fei000/article/details/89949920
        // https://www.khronos.org/opengl/wiki/GL_EXT_framebuffer_multisample
        // https://www.xuebuyuan.com/845887.html
        if(_type == FBO_MSAA) {
            createMultisampleTexture();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _multisampleTexId, 0);
            createMultisampleRenderBuffer();
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, _multisampleRboId);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLenum err = glGetError();
        while(err!=GL_NO_ERROR) {
            LOGE("fbo.setup : 0x%08x\n", err);
            err = glGetError();
        }
        return  true;
    }

    void    begin()
    {
        GLenum err = glGetError();
        while(err!=GL_NO_ERROR) {
            //LOGE("fbo.begin : 0x%08x\n", err);
            err = glGetError();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, _fboID);
        //LOGE("after glBindFramebuffer: 0x%08x\n", glGetError());

        if(_type == FBO_DEPTH) {
            glDrawBuffers(0, GL_NONE);
            //LOGE("after glDrawBuffers: 0x%08x\n", glGetError());
            glReadBuffer(GL_NONE);
            //LOGE("after glReadBuffer: 0x%08x\n", glGetError());
            /**
             * 我们需要的只是在从光的透视图下渲染场景的时候深度信息，所以颜色缓冲没有用。
             * 然而帧缓冲对象不是完全不包含颜色缓冲的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。
             * 我们通过将调用glDrawBuffer，glReadBuffer把读和绘制缓冲设置为GL_NONE来做这件事。
             */
            //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRboId);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTexId, 0);
            //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rgbaTexId, 0);
        }
        if(_type == FBO_RGBA) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _rgbaTexId, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRboId);
        }

        GLenum fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(fbo_status!=GL_FRAMEBUFFER_COMPLETE) {
            LOGE("glCheckFramebufferStatus check err : 0x%08x\n", fbo_status);
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
        glDeleteRenderbuffers(1, &_depthRboId);
        glDeleteTextures(1,&_depthTexId);
        glDeleteTextures(1,&_rgbaTexId);
        glDeleteFramebuffers(1, &_fboID);
        _fboID  = 0;
        _depthRboId = 0;
        _rgbaTexId = 0;
        _depthTexId = 0;
        _type   = FBO_NONE;
        _width  = 0;
        _height = 0;
    }
};
#endif // FRAME_BUFFER_OBJECT_HPP
