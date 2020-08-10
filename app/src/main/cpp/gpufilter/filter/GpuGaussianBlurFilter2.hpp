//
// Created by zzr on 2020/7/25.
//

#ifndef GPU_GAUSSIANBLUR_2_FILTER_HPP
#define GPU_GAUSSIANBLUR_2_FILTER_HPP

#include "GpuBaseFilterGroup.hpp"

class GpuGaussianBlurFilter2 : public GpuBaseFilterGroup {
public:
    int getTypeId() { return FILTER_TYPE_GAUSSIANBLUR; }

    GpuGaussianBlurFilter2()
    {
        GAUSSIAN_BLUR_VERTEX_SHADER ="\n\
                                    attribute vec4 position;\n\
                                    attribute vec4 inputTextureCoordinate;\n\
                                    const int GAUSSIAN_SAMPLES = 9;\n\
                                    uniform float widthFactor;\n\
                                    uniform float heightFactor;\n\
                                    uniform float offset;\n\
                                    \n\
                                    varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n\
                                    void main()\n\
                                    {\n\
                                        gl_Position = position;\n\
                                        \n\
                                        vec2 singleStepOffset = vec2(widthFactor, heightFactor);\n\
                                        int multiplier = 0;\n\
                                        vec2 blurStep;\n\
                                        for (int i = 0; i < GAUSSIAN_SAMPLES; i++)\n\
                                        {\n\
                                            multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));\n\
                                            //-4，-3，-2，-1，0，1，2，3，4\n\
                                            blurStep = float(multiplier) * singleStepOffset;\n\
                                            blurCoordinates[i] = inputTextureCoordinate.xy + blurStep;\n\
                                        }\n\
                                    }";

        GAUSSIAN_BLUR_FRAGMENT_SHADER ="\n\
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
                            uniform int drawMode; //0为YUV,1为RGB \n\
                            const int GAUSSIAN_SAMPLES = 9;\n\
                            varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n\
                            void main()\n\
                            {\n\
                                vec3 fragmentColor = vec3(0.0); \n\
                                if (drawMode==0) \n\
                                {\n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[0]) *0.05); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[1]) *0.09); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[2]) *0.12); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[3]) *0.15); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[4]) *0.18); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[5]) *0.15); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[6]) *0.12); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[7]) *0.09); \n\
                                    fragmentColor += (yuv2rgb(blurCoordinates[8]) *0.05); \n\
                                    gl_FragColor = vec4(fragmentColor, 1.0);\n\
                                }\n\
                                else \n\
                                { \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[0]).rgb *0.05); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[1]).rgb *0.09); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[2]).rgb *0.12); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[3]).rgb *0.15); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[4]).rgb *0.18); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[5]).rgb *0.15); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[6]).rgb *0.12); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[7]).rgb *0.09); \n\
                                    fragmentColor += (texture2D(SamplerRGB, blurCoordinates[8]).rgb *0.05); \n\
                                    gl_FragColor = vec4(fragmentColor, 1.0);\n\
                                } \n\
                            }";
    }
    ~GpuGaussianBlurFilter2()
    {
        if(!GAUSSIAN_BLUR_VERTEX_SHADER.empty()) GAUSSIAN_BLUR_VERTEX_SHADER.clear();
        if(!GAUSSIAN_BLUR_FRAGMENT_SHADER.empty()) GAUSSIAN_BLUR_FRAGMENT_SHADER.clear();
    }

    void init() {
        GpuBaseFilter filter1;
        filter1.init(GAUSSIAN_BLUR_VERTEX_SHADER.c_str(), GAUSSIAN_BLUR_FRAGMENT_SHADER.c_str());
        mWidthFactorLocation1  = glGetUniformLocation(filter1.getProgram(), "widthFactor");
        mHeightFactorLocation1 = glGetUniformLocation(filter1.getProgram(), "heightFactor");
        mSampleOffsetLocation1 = glGetUniformLocation(filter1.getProgram(), "offset");
        mDrawModeLocation1     = glGetUniformLocation(filter1.getProgram(), "drawMode");
        addFilter(filter1);

        GpuBaseFilter filter2;
        filter2.init(GAUSSIAN_BLUR_VERTEX_SHADER.c_str(), GAUSSIAN_BLUR_FRAGMENT_SHADER.c_str());
        mWidthFactorLocation2  = glGetUniformLocation(filter2.getProgram(), "widthFactor");
        mHeightFactorLocation2 = glGetUniformLocation(filter2.getProgram(), "heightFactor");
        mSampleOffsetLocation2 = glGetUniformLocation(filter2.getProgram(), "offset");
        mDrawModeLocation2     = glGetUniformLocation(filter2.getProgram(), "drawMode");
        addFilter(filter2);
    }

    void onOutputSizeChanged(int width, int height) {
        GpuBaseFilterGroup::onOutputSizeChanged(width, height);
        // glUniform1f(mWidthFactorLocation, 1.0f / width);
        // glUniform1f(mHeightFactorLocation, 1.0f / height);
    }
    void setAdjustEffect(float percent) {
        mSampleOffset = range(percent * 100.0f, 0.0f, 2.0f);
    }

    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (mFilterList.size()==0) return;
        GLuint previousTexture = 0;
        int size = mFilterList.size();
        for (int i = 0; i < size; i++) {
            GpuBaseFilter filter = mFilterList[i];
            bool isNotLast = i < size - 1;
            if (isNotLast) {
                glBindFramebuffer(GL_FRAMEBUFFER, mFBO_IDs[i]);
            }
            glClearColor(0, 0, 0, 0);
            if (i == 0) {
                drawFilter1YUV(filter, SamplerY_texId, SamplerU_texId, SamplerV_texId, positionCords, textureCords);
            }
            if (i == 1) { //isNotLast=false, not bind FBO, draw on screen.
                drawFilter2RGB(filter, previousTexture, positionCords, mNormalTextureCords);
            }
            if (isNotLast) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                previousTexture = mFBO_TextureIDs[i];
            }
        }
    }
private:
    void drawFilter1YUV(GpuBaseFilter filter,
                 GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                 void* positionCords, void* textureCords)
    {
        if (!filter.isInitialized())
            return;
        glUseProgram(filter.getProgram());
        glUniform1i(mDrawModeLocation1, 0);
        //glUniform1f(mSampleOffsetLocation1, mSampleOffset);
        glUniform1f(mWidthFactorLocation1, mSampleOffset / filter.mOutputWidth);
        glUniform1f(mHeightFactorLocation1, 0);

        glVertexAttribPointer(filter.mGLAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, positionCords);
        glEnableVertexAttribArray(filter.mGLAttribPosition);
        glVertexAttribPointer(filter.mGLAttribTextureCoordinate, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(filter.mGLAttribTextureCoordinate);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SamplerY_texId);
        glUniform1i(filter.mGLUniformSampleY, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SamplerU_texId);
        glUniform1i(filter.mGLUniformSampleU, 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, SamplerV_texId);
        glUniform1i(filter.mGLUniformSampleV, 2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(filter.mGLAttribPosition);
        glDisableVertexAttribArray(filter.mGLAttribTextureCoordinate);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void drawFilter2RGB(GpuBaseFilter filter, GLuint _texId, void* positionCords, void* textureCords)
    {
        if (!filter.isInitialized())
            return;
        glUseProgram(filter.getProgram());
        glUniform1i(mDrawModeLocation2, 1);
        //glUniform1f(mSampleOffsetLocation2, mSampleOffset);
        glUniform1f(mWidthFactorLocation2, 0);
        glUniform1f(mHeightFactorLocation2, mSampleOffset / filter.mOutputHeight);

        glVertexAttribPointer(filter.mGLAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, positionCords);
        glEnableVertexAttribArray(filter.mGLAttribPosition);

        glVertexAttribPointer(filter.mGLAttribTextureCoordinate, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(filter.mGLAttribTextureCoordinate);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, _texId);
        glUniform1i(filter.mGLUniformSampleRGB, 3);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(filter.mGLAttribPosition);
        glDisableVertexAttribArray(filter.mGLAttribTextureCoordinate);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    std::string GAUSSIAN_BLUR_VERTEX_SHADER;
    std::string GAUSSIAN_BLUR_FRAGMENT_SHADER;

    GLint mWidthFactorLocation1;
    GLint mHeightFactorLocation1;
    GLint mSampleOffsetLocation1;
    GLint mDrawModeLocation1;

    GLint mWidthFactorLocation2;
    GLint mHeightFactorLocation2;
    GLint mSampleOffsetLocation2;
    GLint mDrawModeLocation2;

    float mSampleOffset;
};
#endif // GPU_GAUSSIANBLUR_2_FILTER_HPP
