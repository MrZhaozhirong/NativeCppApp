//
// Created by nicky on 2020/11/12.
//

#ifndef DOUYIN_4IMAGE_FILTER_HPP
#define DOUYIN_4IMAGE_FILTER_HPP

#include "GpuBaseFilter.hpp"

class DouYin4ImageFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_NORMAL; }

    DouYin4ImageFilter()
    {
        LOGI("---DouYin4ImageFilter构造, %p", this);
        GpuBaseFilter::GpuBaseFilter();
    }

    void init() {
        GpuBaseFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), NO_FILTER_FRAGMENT_SHADER.c_str());
    }

    void onOutputSizeChanged(int width, int height) {
        GpuBaseFilter::onOutputSizeChanged(width, height);
        if (mFboTextureId != 9999) {
            glDeleteTextures(1, &mFboTextureId);
            mFboTextureId = 9999;
        }
        if (mFboId != 9999) {
            glDeleteFramebuffers(1, &mFboId);
            mFboId = 9999;
        }
        glGenFramebuffers(1, &mFboId);
        glGenTextures(1, &mFboTextureId);
        glBindTexture(GL_TEXTURE_2D, mFboTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_REPEAT
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFboTextureId, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
        GpuBaseFilter::onDraw(SamplerY_texId, SamplerU_texId, SamplerV_texId, positionCords, textureCords );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


    }

private:
    GLuint mFboId = 9999;
    GLuint mFboTextureId = 9999;

};


#endif // DOUYIN_4IMAGE_FILTER_HPP
