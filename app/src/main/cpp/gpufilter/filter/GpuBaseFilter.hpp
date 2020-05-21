//
// Created by nicky on 2019/12/5.
//

#ifndef GPU_NORMAL_FILTER_HPP
#define GPU_NORMAL_FILTER_HPP

#include <string>
#include "../../program/ShaderHelper.h"
// 和 java/org.zzrblog.gpufilter.FilterType对应
// 而且对应的衍生类的getTypeId也要返回正确的值
#define FILTER_TYPE_NORMAL          0x1010
#define FILTER_TYPE_COLOR_INVERT    0x1011
#define FILTER_TYPE_PIXELATION      0x1012
#define FILTER_TYPE_CONTRAST        0x1013
#define FILTER_TYPE_BRIGHTNESS      0x1014
#define FILTER_TYPE_HUE             0x1015
#define FILTER_TYPE_EXPOSURE        0x1016
#define FILTER_TYPE_SATURATION      0x1017

/**
 * Filter基础类，支持YUV / RGB渲染模式。
 */
class GpuBaseFilter  {
public:
    // 用于上层获取滤镜列表对应的Filter类型
    virtual int getTypeId() { return FILTER_TYPE_NORMAL; }

    GpuBaseFilter()
    {
        //LOGI("---GpuNormalFilter构造, %p",this);
        NO_FILTER_VERTEX_SHADER   = "attribute vec4 position;\n\
                                     attribute vec4 inputTextureCoordinate;\n\
                                     varying vec2 textureCoordinate;\n\
                                     void main()\n\
                                     {\n\
                                        gl_Position = position;\n\
                                        textureCoordinate = inputTextureCoordinate.xy;\n\
                                     }";

        NO_FILTER_FRAGMENT_SHADER = "precision mediump float;\n\
                                     varying highp vec2 textureCoordinate;\n\
                                     uniform sampler2D SamplerRGB;\n\
                                     uniform sampler2D SamplerY;\n\
                                     uniform sampler2D SamplerU;\n\
                                     uniform sampler2D SamplerV;\n\
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
                                        gl_FragColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                        //gl_FragColor = texture2D(SamplerRGB, textureCoordinate);\n\
                                     }";
    }
    virtual ~GpuBaseFilter()
    {
        //LOGI("---GpuNormalFilter析构, %p",this);
        if(!NO_FILTER_VERTEX_SHADER.empty()) NO_FILTER_VERTEX_SHADER.clear();
        if(!NO_FILTER_FRAGMENT_SHADER.empty()) NO_FILTER_FRAGMENT_SHADER.clear();
        mIsInitialized = false;
    }

    virtual void init() {
        init(NO_FILTER_VERTEX_SHADER.c_str(), NO_FILTER_FRAGMENT_SHADER.c_str());
    }

    void init(const char *vertexShaderSource, const char *fragmentShaderSource) {
        mGLProgId = ShaderHelper::buildProgram(vertexShaderSource, fragmentShaderSource);
        mGLAttribPosition = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "position"));
        mGLAttribTextureCoordinate = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "inputTextureCoordinate"));
        mGLUniformSampleRGB = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "SamplerRGB"));
        mGLUniformSampleY = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "SamplerY"));
        mGLUniformSampleU = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "SamplerU"));
        mGLUniformSampleV = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "SamplerV"));
        mIsInitialized = true;
    }

    virtual void destroy() {
        mIsInitialized = false;
        glDeleteProgram(mGLProgId);
    }

    virtual void onOutputSizeChanged(int width, int height) {
        mOutputWidth = width;
        mOutputHeight = height;
    }

    virtual void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks();
        glVertexAttribPointer(mGLAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, positionCords);
        glEnableVertexAttribArray(mGLAttribPosition);
        glVertexAttribPointer(mGLAttribTextureCoordinate, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(mGLAttribTextureCoordinate);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SamplerY_texId);
        glUniform1i(mGLUniformSampleY, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SamplerU_texId);
        glUniform1i(mGLUniformSampleU, 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, SamplerV_texId);
        glUniform1i(mGLUniformSampleV, 2);
        // onDrawArraysPre();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(mGLAttribPosition);
        glDisableVertexAttribArray(mGLAttribTextureCoordinate);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // GLUniformSampleRGB对应的draw，我这里没用rgb模式.
    //virtual void onDraw2(GLuint textureId, void* positionCords, void* textureCords)
    //{
    //    if (!mIsInitialized)
    //        return;
    //    glUseProgram(mGLProgId);
    //    // runPendingOnDrawTasks();
    //    glVertexAttribPointer(mGLAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, positionCords);
    //    glEnableVertexAttribArray(mGLAttribPosition);
    //    glVertexAttribPointer(mGLAttribTextureCoordinate, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
    //    glEnableVertexAttribArray(mGLAttribTextureCoordinate);
    //    if (textureId != -1) {
    //        glActiveTexture(GL_TEXTURE0);
    //        glBindTexture(GL_TEXTURE_2D, textureId);
    //        glUniform1i(mGLUniformSampleRGB, 0);
    //    }
    //    // onDrawArraysPre();
    //    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    //    glDisableVertexAttribArray(mGLAttribPosition);
    //    glDisableVertexAttribArray(mGLAttribTextureCoordinate);
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //}

    // 相关滤镜对应的可调整参数，通过此借口进行操作
    virtual void setAdjustEffect(float percent) {
        // subclass override
    }

    bool isInitialized(){ return mIsInitialized;}

    GLuint getProgram(){ return mGLProgId;}

    float range(float percentage, float start, float end) {
        return (end - start) * percentage / 100.0f + start;
    }

protected:
    std::string NO_FILTER_VERTEX_SHADER;
    std::string NO_FILTER_FRAGMENT_SHADER;

    GLuint  mGLProgId;
    GLuint  mGLAttribPosition;
    GLuint  mGLUniformSampleRGB;
    GLuint  mGLAttribTextureCoordinate;

    GLuint  mGLUniformSampleY;
    GLuint  mGLUniformSampleU;
    GLuint  mGLUniformSampleV;

    int     mOutputWidth;
    int     mOutputHeight;
    bool    mIsInitialized;
};

#endif // GPU_NORMAL_FILTER_HPP
