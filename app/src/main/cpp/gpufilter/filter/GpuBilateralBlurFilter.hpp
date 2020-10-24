

#ifndef GPU_BilateralBLUR_FILTER_HPP
#define GPU_BilateralBLUR_FILTER_HPP

#include "GpuBaseFilter.hpp"

class GpuBilateralBlurFilter : public GpuBaseFilter {
public:
    virtual int getTypeId() { return FILTER_TYPE_BilateralBLUR; }

    GpuBilateralBlurFilter()
    {
        Bilateral_BLUR_VERTEX_SHADER ="\n\
                                       attribute vec4 position;\n\
                                       attribute vec4 inputTextureCoordinate;\n\
                                       const int GAUSSIAN_SAMPLES = 9;\n\
                                       uniform vec2 singleStepOffset;\n\
                                       varying vec2 textureCoordinate;\n\
                                       varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n\
                                       \n\
                                       void main()\n\
                                       {\n\
                                           gl_Position = position;\n\
                                           textureCoordinate = inputTextureCoordinate.xy;\n\
                                           \n\
                                           int multiplier = 0;\n\
                                           vec2 blurStep;\n\
                                           for (int i = 0; i < GAUSSIAN_SAMPLES; i++)\n\
                                           {\n\
                                               multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));\n\
                                               blurStep = float(multiplier) * singleStepOffset;\n\
                                               blurCoordinates[i] = inputTextureCoordinate.xy + blurStep;\n\
                                           }\n\
                                       }";


        Bilateral_BLUR_FRAGMENT_SHADER = "\n\
                                  uniform sampler2D SamplerY;\n\
                                  uniform sampler2D SamplerU;\n\
                                  uniform sampler2D SamplerV;\n\
                                  uniform sampler2D SamplerRGB;\n\
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
                                  const lowp int GAUSSIAN_SAMPLES = 9;\n\
                                  varying highp vec2 textureCoordinate;\n\
                                  varying highp vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n\
                                  uniform mediump float distanceNormalizationFactor;\n\
                                  \n\
                                  void main()\n\
                                  {\n\
                                      lowp vec4 centralColor;\n\
                                      lowp float gaussianWeightTotal;\n\
                                      lowp vec4 sampleSum;\n\
                                      lowp vec4 sampleColor;\n\
                                      lowp float distanceFromCentralColor;\n\
                                      lowp float gaussianWeight;\n\
                                      \n\
                                      centralColor = vec4(yuv2rgb(blurCoordinates[4]), 1.0);\n\
                                      gaussianWeightTotal = 0.22;\n\
                                      sampleSum = centralColor * 0.22;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[0]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.03 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[1]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.07 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[2]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[3]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.17 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[5]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.17 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[6]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[7]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.07 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      sampleColor = vec4(yuv2rgb(blurCoordinates[8]), 1.0);\n\
                                      distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);\n\
                                      gaussianWeight = 0.03 * (1.0 - distanceFromCentralColor);\n\
                                      gaussianWeightTotal += gaussianWeight;\n\
                                      sampleSum += sampleColor * gaussianWeight;\n\
                                      \n\
                                      //gl_FragColor = vec4(   min(distance(centralColor, sampleColor)*distanceNormalizationFactor, 1.0)   , 0.0, 0.0, 1.0);\n\
                                      gl_FragColor = sampleSum / gaussianWeightTotal;\n\
                                  }";

    }

    ~GpuBilateralBlurFilter()
    {
        if(!Bilateral_BLUR_VERTEX_SHADER.empty()) Bilateral_BLUR_VERTEX_SHADER.clear();
        if(!Bilateral_BLUR_FRAGMENT_SHADER.empty()) Bilateral_BLUR_FRAGMENT_SHADER.clear();
    }

    void setAdjustEffect(float percent) {
        mThreshold_ColorDistanceNormalization = range(percent*100.0f, 10.0f, 1.0f);
    }

    void init() {
        GpuBaseFilter::init(Bilateral_BLUR_VERTEX_SHADER.c_str(), Bilateral_BLUR_FRAGMENT_SHADER.c_str());
        mThreshold_ColorDistanceNormalization = 10.0f;
        mSingleStepOffsetLocation = glGetUniformLocation(getProgram(), "singleStepOffset");
        mColorDisNormalFactorLocation = glGetUniformLocation(getProgram(), "distanceNormalizationFactor");
    }

    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(getProgram());
        glUniform2f(mSingleStepOffsetLocation, 1.0f/mOutputWidth, 1.0f/mOutputHeight);
        glUniform1f(mColorDisNormalFactorLocation, mThreshold_ColorDistanceNormalization);


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
    std::string Bilateral_BLUR_VERTEX_SHADER;
    std::string Bilateral_BLUR_FRAGMENT_SHADER;


    GLint mSingleStepOffsetLocation;
    GLint mColorDisNormalFactorLocation;
    float mThreshold_ColorDistanceNormalization;
};

#endif