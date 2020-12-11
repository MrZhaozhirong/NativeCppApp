//
// Created by nicky on 2020/11/19.
//

#ifndef DOUYIN_SOULOUT_FILTER_HPP
#define DOUYIN_SOULOUT_FILTER_HPP

#include "GpuBaseFilter.hpp"

class DouYinSouloutFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_DOUYIN_SOULOUT; }

    DouYinSouloutFilter()
    {
        SOULOUT_VERTEX_SHADER   = "attribute vec4 position;\n\
                                   attribute vec4 inputTextureCoordinate;\n\
                                   attribute vec4 soulTextureCoordinate;\n\
                                   varying vec2 textureCoordinate;\n\
                                   varying vec2 soulCoordinate;\n\
                                   void main()\n\
                                   {\n\
                                      textureCoordinate = inputTextureCoordinate.xy;\n\
                                      soulCoordinate = soulTextureCoordinate.xy;\n\
                                      gl_Position = position;\n\
                                   }";

        SOULOUT_FRAGMENT_SHADER = "precision mediump float;\n\
                                   varying vec2 textureCoordinate;\n\
                                   varying vec2 soulCoordinate;\n\
                                   uniform sampler2D SamplerY;\n\
                                   uniform sampler2D SamplerU;\n\
                                   uniform sampler2D SamplerV;\n\
                                   uniform int mixFlag;\n\
                                   uniform int drawMode;\n\
                                   uniform sampler2D textureSoul;\n\
                                   mat3 colorConversionMatrix = mat3(\n\
                                                      1.0, 1.0, 1.0,\n\
                                                      0.0, -0.39465, 2.03211,\n\
                                                      1.13983, -0.58060, 0.0);\n\
                                   vec3 yuv2rgb(vec2 pos)\n\
                                   {\n\
                                      vec3 yuv;\n\
                                      yuv.x = texture2D(SamplerY, pos).r;\n\
                                      yuv.y = texture2D(SamplerU, pos).r - 0.5;\n\
                                      yuv.z = texture2D(SamplerV, pos).r - 0.5;\n\
                                      return colorConversionMatrix * yuv;\n\
                                   }\n\
                                   void main()\n\
                                   {\n\
                                      if (mixFlag==1) \n\
                                      {\n\
                                          gl_FragColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                      }\n\
                                      else\n\
                                      {\n\
                                          vec4 normalColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                          vec4 soulColor = texture2D(textureSoul, soulCoordinate);\n\
                                          gl_FragColor = mix(normalColor, soulColor, 0.35);\n\
                                      }\n\
                                   }";
    }
    ~DouYinSouloutFilter()
    {
        if(!SOULOUT_VERTEX_SHADER.empty()) SOULOUT_VERTEX_SHADER.clear();
        if(!SOULOUT_FRAGMENT_SHADER.empty()) SOULOUT_FRAGMENT_SHADER.clear();
    }

    void onOutputSizeChanged(int width, int height) {
        GpuBaseFilter::onOutputSizeChanged(width, height);
        if (mSoulTextureId != 9999) {
            glDeleteTextures(1, &mSoulTextureId);
            mSoulTextureId = 9999;
        }
        if (mFboId != 9999) {
            glDeleteFramebuffers(1, &mFboId);
            mFboId = 9999;
        }
        glGenFramebuffers(1, &mFboId);
        glGenTextures(1, &mSoulTextureId);
        glBindTexture(GL_TEXTURE_2D, mSoulTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_REPEAT
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mSoulTextureId, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void init() {
        GpuBaseFilter::init(SOULOUT_VERTEX_SHADER.c_str(), SOULOUT_FRAGMENT_SHADER.c_str());
        mGLAttribSoulCoordinate = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "soulTextureCoordinate"));
        mMixFlagLocation = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "mixFlag"));
        mGLUniformSampleSoul = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "textureSoul"));
    }


    void setInputFps(int fps) {
        mInputFps = fps;
    }
    void setCurrentFps(int fps) {
        mCurrentFps = fps;
    }


    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (mInputFps==0&&mCurrentFps==0) {
            mMixFlag = 0;
        } else {
            int halfInputFps = mInputFps/2;
            if (mCurrentFps>halfInputFps+0)
            //    || mCurrentFps==halfInputFps+1
            //    || mCurrentFps==halfInputFps+2
            //    || mCurrentFps==halfInputFps+3
            //    || mCurrentFps==halfInputFps+4
            //    || mCurrentFps==halfInputFps+5)
            {
                mMixFlag = 1;
            }
            else
            {
                mMixFlag = 0;
            }
        }

        if (mMixFlag == 1) //mMixFlag == 1
        { // 准备好灵魂的纹理
            glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
            GpuBaseFilter::onDraw(SamplerY_texId, SamplerU_texId, SamplerV_texId, positionCords, textureCords );
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks();
        glVertexAttribPointer(mGLAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, positionCords); // positionCords
        glEnableVertexAttribArray(mGLAttribPosition);
        glVertexAttribPointer(mGLAttribTextureCoordinate, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(mGLAttribTextureCoordinate);

        glVertexAttribPointer(mGLAttribSoulCoordinate, 2, GL_FLOAT, GL_FALSE, 0, soul_texCoordinates);
        glEnableVertexAttribArray(mGLAttribSoulCoordinate);

        glUniform1i(mMixFlagLocation, mMixFlag);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SamplerY_texId);
        glUniform1i(mGLUniformSampleY, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SamplerU_texId);
        glUniform1i(mGLUniformSampleU, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, SamplerV_texId);
        glUniform1i(mGLUniformSampleV, 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, mSoulTextureId);
        glUniform1i(mGLUniformSampleSoul, 3);
        // onDrawArraysPre();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(mGLAttribPosition);
        glDisableVertexAttribArray(mGLAttribTextureCoordinate);
        glDisableVertexAttribArray(mGLAttribSoulCoordinate);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
private:
    std::string SOULOUT_VERTEX_SHADER;
    std::string SOULOUT_FRAGMENT_SHADER;

    GLuint mFboId = 9999;
    GLuint mSoulTextureId = 9999;

    GLuint mMixFlagLocation;
    GLuint mGLAttribSoulCoordinate;
    GLuint mGLUniformSampleSoul;

    int mMixFlag;
    int mInputFps;
    int mCurrentFps;

    float soul_positionCords[8] = {
            //x, y          //position
            -1.0f, -1.0f,   //左下
            1.0f, -1.0f,    //右下
            -1.0f, 1.0f,    //左上
            1.0f, 1.0f,     //右上
    };

    float soul_texCoordinates[8] = {
            0.1f, 0.1f,     //左下
            0.9f, 0.1f,     //右下
            0.1f, 0.9f,     //左上
            0.9f, 0.9f,     //右上
    };
};
#endif // DOUYIN_SOULOUT_FILTER_HPP
