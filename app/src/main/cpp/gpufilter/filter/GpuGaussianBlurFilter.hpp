//
// Created by zzr on 2020/7/21.
//

#ifndef GPU_GAUSSIANBLUR_FILTER_HPP
#define GPU_GAUSSIANBLUR_FILTER_HPP

#include "GpuBaseFilter.hpp"

class GpuGaussianBlurFilter : public GpuBaseFilter {
public:
    virtual int getTypeId() { return FILTER_TYPE_GAUSSIANBLUR; }

    GpuGaussianBlurFilter()
    {
        GAUSSIAN_BLUR_VERTEX_SHADER ="\n\
                            attribute vec4 position;\n\
                            attribute vec4 inputTextureCoordinate;\n\
                            uniform float widthFactor;\n\
                            uniform float heightFactor;\n\
                            uniform float offset;\n\
                            const int GAUSSIAN_SAMPLES = 9;\n\
                            varying vec2 textureCoordinate[GAUSSIAN_SAMPLES];\n\
                            void main()\n\
                            {\n\
                                gl_Position = position;\n\
                                vec2 widthStep = vec2(offset*widthFactor, 0.0);\n\
                                vec2 heightStep = vec2(0.0, offset*heightFactor);\n\
                                textureCoordinate[0] = inputTextureCoordinate.xy - heightStep - widthStep;\n\
                                textureCoordinate[1] = inputTextureCoordinate.xy - heightStep;\n\
                                textureCoordinate[2] = inputTextureCoordinate.xy - heightStep + widthStep;\n\
                                \n\
                                textureCoordinate[3] = inputTextureCoordinate.xy - widthStep;\n\
                                textureCoordinate[4] = inputTextureCoordinate.xy;\n\
                                textureCoordinate[5] = inputTextureCoordinate.xy + widthStep;\n\
                                \n\
                                textureCoordinate[6] = inputTextureCoordinate.xy + heightStep - widthStep;\n\
                                textureCoordinate[7] = inputTextureCoordinate.xy + heightStep;\n\
                                textureCoordinate[8] = inputTextureCoordinate.xy + heightStep + widthStep;\n\
                            }";

        GAUSSIAN_BLUR_FRAGMENT_SHADER ="\n\
                            precision highp float;\n\
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
                            uniform mediump mat3 convolutionMatrix;\n\
                            const int GAUSSIAN_SAMPLES = 9;\n\
                            varying vec2 textureCoordinate[GAUSSIAN_SAMPLES];\n\
                            void main()\n\
                            {\n\
                                //mediump vec3 topLeftColor     = yuv2rgb(textureCoordinate[0]);\n\
                                //mediump vec3 topColor         = yuv2rgb(textureCoordinate[1]);\n\
                                //mediump vec3 topRightColor    = yuv2rgb(textureCoordinate[2]);\n\
                                //mediump vec3 leftColor        = yuv2rgb(textureCoordinate[3]);\n\
                                //mediump vec3 centerColor      = yuv2rgb(textureCoordinate[4]);\n\
                                //mediump vec3 rightColor       = yuv2rgb(textureCoordinate[5]);\n\
                                //mediump vec3 bottomLeftColor  = yuv2rgb(textureCoordinate[6]);\n\
                                //mediump vec3 bottomColor      = yuv2rgb(textureCoordinate[7]);\n\
                                //mediump vec3 bottomRightColor = yuv2rgb(textureCoordinate[8]);\n\
                                vec3 fragmentColor = (yuv2rgb(textureCoordinate[0]) * convolutionMatrix[0][0]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[1]) * convolutionMatrix[0][1]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[2]) * convolutionMatrix[0][2]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[3]) * convolutionMatrix[1][0]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[4]) * convolutionMatrix[1][1]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[5]) * convolutionMatrix[1][2]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[6]) * convolutionMatrix[2][0]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[7]) * convolutionMatrix[2][1]);\n\
                                fragmentColor += (yuv2rgb(textureCoordinate[8]) * convolutionMatrix[2][2]);\n\
                                gl_FragColor = vec4(fragmentColor, 1.0);\n\
                            }";
    }

    ~GpuGaussianBlurFilter()
    {
        if(!GAUSSIAN_BLUR_VERTEX_SHADER.empty()) GAUSSIAN_BLUR_VERTEX_SHADER.clear();
        if(!GAUSSIAN_BLUR_FRAGMENT_SHADER.empty()) GAUSSIAN_BLUR_FRAGMENT_SHADER.clear();

        delete[] convolutionKernel;
    }

    void init() {
        GpuBaseFilter::init(GAUSSIAN_BLUR_VERTEX_SHADER.c_str(), GAUSSIAN_BLUR_FRAGMENT_SHADER.c_str());
        mWidthFactorLocation = glGetUniformLocation(getProgram(), "widthFactor");
        mHeightFactorLocation = glGetUniformLocation(getProgram(), "heightFactor");
        mSampleOffsetLocation = glGetUniformLocation(getProgram(), "offset");
        mUniformConvolutionMatrix = glGetUniformLocation(getProgram(), "convolutionMatrix");
        mSampleOffset = 0.0f;
        // https://blog.csdn.net/Vici__/article/details/102476784
        convolutionKernel = new GLfloat[9]{
                0.0947416f, 0.118318f, 0.0947416f,
                0.118318f,  0.147761f, 0.118318f,
                0.0947416f, 0.118318f, 0.0947416f,
        };
    }

    void onOutputSizeChanged(int width, int height) {
        GpuBaseFilter::onOutputSizeChanged(width, height);
        glUniform1f(mWidthFactorLocation, 1.0f / width);
        glUniform1f(mHeightFactorLocation, 1.0f / height);
    }

    void setAdjustEffect(float percent) {
        mSampleOffset = range(percent * 100.0f, 0.0f, 3.0f);
    }

    // glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks
        glUniformMatrix3fv(mUniformConvolutionMatrix, 1, GL_FALSE, convolutionKernel);
        glUniform1f(mSampleOffsetLocation, mSampleOffset);
        glUniform1f(mWidthFactorLocation, 1.0f / mOutputWidth);
        glUniform1f(mHeightFactorLocation, 1.0f / mOutputHeight);

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
    std::string GAUSSIAN_BLUR_VERTEX_SHADER;
    std::string GAUSSIAN_BLUR_FRAGMENT_SHADER;

    GLint mWidthFactorLocation;
    GLint mHeightFactorLocation;

    GLint mUniformConvolutionMatrix;
    GLfloat* convolutionKernel;

    GLint mSampleOffsetLocation;
    float mSampleOffset;
};
#endif // GPU_GAUSSIANBLUR_FILTER_HPP
