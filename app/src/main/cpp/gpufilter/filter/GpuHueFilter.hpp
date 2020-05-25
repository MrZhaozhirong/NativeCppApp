//
// Created by nicky on 2020/5/20.
//

#ifndef GPU_HUE_FILTER_HPP
#define GPU_HUE_FILTER_HPP


#include "GpuBaseFilter.hpp"
#include "../../common/CELLMath.hpp"
using namespace CELL;
class GpuHueFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_HUE; }

    GpuHueFilter()
    {
        HUE_FRAGMENT_SHADER =      "precision mediump float;\n\
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
                                    uniform mediump float hueAdjust;\n\
                                    const highp vec4 kRGBToYPrime = vec4 (0.299, 0.587, 0.114, 0.0);\n\
                                    const highp vec4 kRGBToI = vec4 (0.595716, -0.274453, -0.321263, 0.0);\n\
                                    const highp vec4 kRGBToQ = vec4 (0.211456, -0.522591, 0.31135, 0.0);\n\
                                    const highp vec4 kYIQToR = vec4 (1.0, 0.9563, 0.6210, 0.0);\n\
                                    const highp vec4 kYIQToG = vec4 (1.0, -0.2721, -0.6474, 0.0);\n\
                                    const highp vec4 kYIQToB = vec4 (1.0, -1.1070, 1.7046, 0.0);\n\
                                    void main()\n\
                                    {\n\
                                        // Sample the input pixel\n\
                                        vec4 textureColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                        // Convert to YIQ\n\
                                        highp float YPrime = dot(textureColor, kRGBToYPrime);\n\
                                        highp float I = dot(textureColor, kRGBToI);\n\
                                        highp float Q = dot(textureColor, kRGBToQ);\n\
                                        // Calculate the hue and chroma\n\
                                        highp float hue = atan(Q, I);\n\
                                        highp float chroma = sqrt(I * I + Q * Q);\n\
                                        // Make the user's adjustments & 为啥取反?\n\
                                        hue += (-hueAdjust);\n\
                                        // Convert back to YIQ\n\
                                        Q = chroma * sin (hue);\n\
                                        I = chroma * cos (hue);\n\
                                        // Convert back to RGB\n\
                                        highp vec4 yIQ = vec4 (YPrime, I, Q, 0.0);\n\
                                        textureColor.r = dot(yIQ, kYIQToR);\n\
                                        textureColor.g = dot(yIQ, kYIQToG);\n\
                                        textureColor.b = dot(yIQ, kYIQToB);\n\
                                        gl_FragColor = textureColor;\n\
                                    }";
    }
    ~GpuHueFilter() {
        if(!HUE_FRAGMENT_SHADER.empty()) HUE_FRAGMENT_SHADER.clear();
    }

    void setAdjustEffect(float percent) {
        //mHue = (percent % 360.0f) * (float) PI / 180.0f;
        mHue = range(percent * 100.0f, -1.5f, 1.5f);
    }

    void init() {
        GpuBaseFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), HUE_FRAGMENT_SHADER.c_str());
        mHueLocation = glGetUniformLocation(mGLProgId, "hueAdjust");
        mHue = 0.0f;
    }

    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks
        glUniform1f(mHueLocation, mHue);

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
    std::string HUE_FRAGMENT_SHADER;
    GLint mHueLocation;
    float mHue;
};


#endif //GPU_HUE_FILTER_HPP
