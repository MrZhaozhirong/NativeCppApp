//
// Created by nicky on 2019/12/23.
//

#ifndef GPU_PIXELATION_FILTER_HPP
#define GPU_PIXELATION_FILTER_HPP

#include "GpuBaseFilter.hpp"
/**
 * 对图像应用格仔化效果。
 */
class GpuPixelationFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_PIXELATION; }

    GpuPixelationFilter()
    {
        PIXELATION_FRAGMENT_SHADER="precision highp float;\n\
                                    varying highp vec2 textureCoordinate;\n\
                                    uniform sampler2D SamplerRGB;\n\
                                    uniform sampler2D SamplerY;\n\
                                    uniform sampler2D SamplerU;\n\
                                    uniform sampler2D SamplerV;\n\
                                    mat3 colorConversionMatrix = mat3(\n\
                                                       1.0, 1.0, 1.0,\n\
                                                       0.0, -0.39465, 2.03211,\n\
                                                       1.13983, -0.58060, 0.0);\n\
                                    uniform float imageWidthFactor;\n\
                                    uniform float imageHeightFactor;\n\
                                    uniform float pixel;\n\
                                    vec3 yuv2rgb(vec2 pos)\n\
                                    {\n\
                                        vec3 yuv;\n\
                                        yuv.x = texture2D(SamplerY, pos).r;\n\
                                        yuv.y = texture2D(SamplerU, pos).r-0.5;\n\
                                        yuv.z = texture2D(SamplerV, pos).r-0.5;\n\
                                        return colorConversionMatrix * yuv;\n\
                                    }\n\
                                    void main()\n\
                                    {\n\
                                        vec2 uv  = textureCoordinate.xy;\n\
                                        float dx = pixel * imageWidthFactor;\n\
                                        float dy = pixel * imageHeightFactor;\n\
                                        vec2 coord = vec2(dx*floor(uv.x / dx), dy*floor(uv.y / dy));\n\
                                        gl_FragColor = vec4(yuv2rgb(coord), 1.0);\n\
                                        //gl_FragColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                    }";
    }
    ~GpuPixelationFilter() {
        if(!PIXELATION_FRAGMENT_SHADER.empty()) PIXELATION_FRAGMENT_SHADER.clear();
    }

    void init() {
        GpuBaseFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), PIXELATION_FRAGMENT_SHADER.c_str());
        mPixelLocation = glGetUniformLocation(mGLProgId, "pixel");
        mImageWidthFactorLocation = glGetUniformLocation(mGLProgId, "imageWidthFactor");
        mImageHeightFactorLocation = glGetUniformLocation(mGLProgId, "imageHeightFactor");
        mPixelValue = 1.0f;
    }

    void setAdjustEffect(float percent) {
        if(percent==0.0f) percent=0.01f;
        mPixelValue = percent * 100.0f * 1.0f;
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
        glUniform1f(mPixelLocation, mPixelValue);
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
    std::string PIXELATION_FRAGMENT_SHADER;
    GLint  mImageWidthFactorLocation;
    GLint  mImageHeightFactorLocation;
    GLint  mPixelLocation;
    float   mPixelValue;
};
#endif // GPU_PIXELATION_FILTER_HPP
