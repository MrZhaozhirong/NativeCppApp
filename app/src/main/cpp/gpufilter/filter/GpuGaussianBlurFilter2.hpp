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
                                        vec2 singleStepOffset = vec2(texelHeightOffset, texelWidthOffset);\n\
                                        int multiplier = 0;\n\
                                        vec2 blurStep;\n\
                                        for (int i = 0; i < GAUSSIAN_SAMPLES; i++)\n\
                                        {\n\
                                            multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));\n\
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
                            varying vec2 textureCoordinate[GAUSSIAN_SAMPLES];\n\
                            void main()\n\
                            {\n\
                                vec3 fragmentColor = vec(0,0); \n\
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
                                }\n\
                                else \n\
                                { \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[0]).rgb *0.05; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[1]).rgb *0.09; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[2]).rgb *0.12; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[3]).rgb *0.15; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[4]).rgb *0.18; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[5]).rgb *0.15; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[6]).rgb *0.12; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[7]).rgb *0.09; \n\
                                    fragmentColor += texture2D(inputImageTexture, blurCoordinates[8]).rgb *0.05; \n\
                                } \n\
                                gl_FragColor = vec4(fragmentColor, 1.0);\n\
                            }";
    }
    ~GpuGaussianBlurFilter2()
    {
        if(!GAUSSIAN_BLUR_VERTEX_SHADER.empty()) GAUSSIAN_BLUR_VERTEX_SHADER.clear();
        if(!GAUSSIAN_BLUR_FRAGMENT_SHADER.empty()) GAUSSIAN_BLUR_FRAGMENT_SHADER.clear();
    }

    void init() {
        GpuBaseFilter filter1;
        filter1.init(GAUSSIAN_BLUR_VERTEX_SHADER.c_str(), GAUSSIAN_BLUR_VERTEX_SHADER.c_str());
        mWidthFactorLocation1  = glGetUniformLocation(filter1.getProgram(), "widthFactor");
        mHeightFactorLocation1 = glGetUniformLocation(filter1.getProgram(), "heightFactor");
        mSampleOffsetLocation1 = glGetUniformLocation(filter1.getProgram(), "offset");
        addFilter(filter1);

        GpuBaseFilter filter2;
        filter2.init(GAUSSIAN_BLUR_VERTEX_SHADER.c_str(), GAUSSIAN_BLUR_VERTEX_SHADER.c_str());
        mWidthFactorLocation2  = glGetUniformLocation(filter2.getProgram(), "widthFactor");
        mHeightFactorLocation2 = glGetUniformLocation(filter2.getProgram(), "heightFactor");
        mSampleOffsetLocation2 = glGetUniformLocation(filter2.getProgram(), "offset");
        addFilter(filter2);
    }

    void onOutputSizeChanged(int width, int height) {
        GpuBaseFilterGroup::onOutputSizeChanged(width, height);
        // glUniform1f(mWidthFactorLocation, 1.0f / width);
        // glUniform1f(mHeightFactorLocation, 1.0f / height);
    }


    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (mFilterList.size()==0) return;
        int previousTexture = -100;
        int size = mFilterList.size();
        for (int i = 0; i < size; i++) {
            GpuBaseFilter filter = mFilterList[i];
            bool isNotLast = i < size - 1;
            if (isNotLast) {
                glBindFramebuffer(GL_FRAMEBUFFER, mFBO_IDs[i]);
                glClearColor(0, 0, 0, 0);
            }

            if (i == 0) {
                filter.onDraw(SamplerY_texId, SamplerU_texId, SamplerV_texId, positionCords, textureCords);
            }else {
                drawFBO(filter, previousTexture, mGLCubeBuffer, mGLTextureBuffer);
            }

            if (isNotLast) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                previousTexture = mFBO_TextureIDs[i];
            }
        }
    }
private:
    std::string GAUSSIAN_BLUR_VERTEX_SHADER;
    std::string GAUSSIAN_BLUR_FRAGMENT_SHADER;

    GLint mWidthFactorLocation1;
    GLint mHeightFactorLocation1;
    GLint mSampleOffsetLocation1;

    GLint mWidthFactorLocation2;
    GLint mHeightFactorLocation2;
    GLint mSampleOffsetLocation2;
};
#endif // GPU_GAUSSIANBLUR_2_FILTER_HPP
