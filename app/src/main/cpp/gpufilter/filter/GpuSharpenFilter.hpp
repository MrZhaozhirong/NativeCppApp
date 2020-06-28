//
// Created by nicky on 2020/6/9.
//

#include "GpuBaseFilter.hpp"

#ifndef GPU_SHARPEN_FILTER_HPP
#define GPU_SHARPEN_FILTER_HPP


class GpuSharpenFilter : public GpuBaseFilter {
public:
    virtual int getTypeId() { return FILTER_TYPE_SHARPEN; }

    GpuSharpenFilter()
    {
        SHARPEN_VERTEX_SHADER     ="\n\
                                    attribute vec4 position;\n\
                                    attribute vec4 inputTextureCoordinate;\n\
                                    \n\
                                    uniform float imageWidthFactor;\n\
                                    uniform float imageHeightFactor;\n\
                                    uniform float sharpness;\n\
                                    \n\
                                    varying vec2 textureCoordinate;\n\
                                    varying vec2 leftTextureCoordinate;\n\
                                    varying vec2 rightTextureCoordinate;\n\
                                    varying vec2 topTextureCoordinate;\n\
                                    varying vec2 bottomTextureCoordinate;\n\
                                    varying float centerMultiplier;\n\
                                    varying float edgeMultiplier;\n\
                                    void main()\n\
                                    {\n\
                                        gl_Position = position;\n\
                                        vec2 widthStep = vec2(imageWidthFactor, 0.0);\n\
                                        vec2 heightStep = vec2(0.0, imageHeightFactor);\n\
                                        textureCoordinate = inputTextureCoordinate.xy;\n\
                                        leftTextureCoordinate = inputTextureCoordinate.xy - widthStep;\n\
                                        rightTextureCoordinate = inputTextureCoordinate.xy + widthStep;\n\
                                        topTextureCoordinate = inputTextureCoordinate.xy + heightStep;\n\
                                        bottomTextureCoordinate = inputTextureCoordinate.xy - heightStep;\n\
                                        centerMultiplier = 1.0 + 4.0 * sharpness;\n\
                                        edgeMultiplier = sharpness;\n\
                                    }";

        SHARPEN_FRAGMENT_SHADER   ="\n\
                                    varying highp vec2 textureCoordinate;\n\
                                    varying highp vec2 leftTextureCoordinate;\n\
                                    varying highp vec2 rightTextureCoordinate;\n\
                                    varying highp vec2 topTextureCoordinate;\n\
                                    varying highp vec2 bottomTextureCoordinate;\n\
                                    varying float centerMultiplier;\n\
                                    varying float edgeMultiplier;\n\
                                    \n\
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
                                        mediump vec3 textureColor = yuv2rgb(textureCoordinate);\n\
                                        mediump vec3 leftTextureColor = yuv2rgb(leftTextureCoordinate);\n\
                                        mediump vec3 rightTextureColor = yuv2rgb(rightTextureCoordinate);\n\
                                        mediump vec3 topTextureColor = yuv2rgb(topTextureCoordinate);\n\
                                        mediump vec3 bottomTextureColor = yuv2rgb(bottomTextureCoordinate);\n\
                                        gl_FragColor = vec4((textureColor*centerMultiplier - (leftTextureColor*edgeMultiplier + rightTextureColor*edgeMultiplier + topTextureColor*edgeMultiplier + bottomTextureColor*edgeMultiplier)), 1.0);\n\
                                    }";
    }

    ~GpuSharpenFilter() {
        if(!SHARPEN_VERTEX_SHADER.empty()) SHARPEN_VERTEX_SHADER.clear();
        if(!SHARPEN_FRAGMENT_SHADER.empty()) SHARPEN_FRAGMENT_SHADER.clear();
    }

    void setAdjustEffect(float percent) {
        mSharpness = range(percent * 100.0f, -2.0f, 5.0f);
        // from -4.0 to 4.0, with 0.0 as the normal level
    }

    void init() {
        GpuBaseFilter::init(SHARPEN_VERTEX_SHADER.c_str(), SHARPEN_FRAGMENT_SHADER.c_str());
        mSharpnessLocation = glGetUniformLocation(mGLProgId, "sharpness");
        mImageWidthFactorLocation = glGetUniformLocation(mGLProgId, "imageWidthFactor");
        mImageHeightFactorLocation = glGetUniformLocation(mGLProgId, "imageHeightFactor");
        mSharpness = 0.0f;
    }

    void onOutputSizeChanged(int width, int height) {
        GpuBaseFilter::onOutputSizeChanged(width, height);
        glUniform1f(mImageWidthFactorLocation, 1.0f / width);
        glUniform1f(mImageHeightFactorLocation, 1.0f / height);
    }


    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks
        glUniform1f(mSharpnessLocation, mSharpness);
        glUniform1f(mImageWidthFactorLocation, 1.0f / mOutputWidth);
        glUniform1f(mImageHeightFactorLocation, 1.0f / mOutputHeight);

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
        // onDrawArraysPre
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(mGLAttribPosition);
        glDisableVertexAttribArray(mGLAttribTextureCoordinate);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
private:
    std::string SHARPEN_VERTEX_SHADER;
    std::string SHARPEN_FRAGMENT_SHADER;

    GLint mImageWidthFactorLocation;
    GLint mImageHeightFactorLocation;
    GLint mSharpnessLocation;
    float mSharpness;
};

#endif