//
// Created by nicky on 2020/5/20.
//

#ifndef GPU_BRIGHTNESS_FILTER_HPP
#define GPU_BRIGHTNESS_FILTER_HPP


#include "GpuBaseFilter.hpp"

class GpuBrightnessFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_BRIGHTNESS; }

    GpuBrightnessFilter()
    {
        BRIGHTNESS_FRAGMENT_SHADER="precision mediump float;\n\
                                    varying highp vec2 textureCoordinate;\n\
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
                                    uniform lowp float brightness;\n\
                                    void main()\n\
                                    {\n\
                                        vec4 vec4 textureColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                        gl_FragColor = vec4((textureColor.rgb + vec3(brightness)), textureColor.w);\n\
                                    }";

    }
    ~GpuBrightnessFilter() {
        if(!BRIGHTNESS_FRAGMENT_SHADER.empty()) BRIGHTNESS_FRAGMENT_SHADER.clear();
    }

    void setAdjustEffect(float percent) {
        if(percent==0.0f) percent=0.01f;
        mBrightness = range(percent * 100.0f, -1.0f, 1.0f);
    }

    void init() {
        GpuBaseFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), BRIGHTNESS_FRAGMENT_SHADER.c_str());
        mBrightnessLocation = glGetUniformLocation(mGLProgId, "brightness");
        mBrightness = 0.0f;
    }

    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks
        glUniform1f(mBrightnessLocation, mBrightness);

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
    std::string BRIGHTNESS_FRAGMENT_SHADER;
    GLint mBrightnessLocation;
    float mBrightness;

    float range(float percentage, float start, float end) {
        return (end - start) * percentage / 100.0f + start;
    }
};


#endif //GPU_COLOR_INVERT_FILTER_HPP
