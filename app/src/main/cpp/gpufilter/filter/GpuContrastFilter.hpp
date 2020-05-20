//
// Created by nicky on 2019/12/19.
//

#ifndef GPU_CONTRAST_FILTER_HPP
#define GPU_CONTRAST_FILTER_HPP

#include "GpuBaseFilter.hpp"
/**
 * 更改图像的对比度。
 * 对比度值在0.0到4.0之间，正常值为1.0
 */
class GpuContrastFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_CONTRAST; }

    GpuContrastFilter()
    {
        CONTRAST_FRAGMENT_SHADER  ="precision mediump float;\n\
                                    varying highp vec2 textureCoordinate;\n\
                                    uniform sampler2D SamplerRGB;\n\
                                    uniform sampler2D SamplerY;\n\
                                    uniform sampler2D SamplerU;\n\
                                    uniform sampler2D SamplerV;\n\
                                    uniform lowp float contrast;\n\
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
                                       vec4 textureColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                       gl_FragColor = vec4((contrast*(textureColor.rgb - vec3(0.5)) + vec3(0.5)), textureColor.w);\n\
                                    }";
    }
    ~GpuContrastFilter() {
        if(!CONTRAST_FRAGMENT_SHADER.empty()) CONTRAST_FRAGMENT_SHADER.clear();
    }

    void init() {
        GpuBaseFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), CONTRAST_FRAGMENT_SHADER.c_str());
        mContrastLocation = glGetUniformLocation(mGLProgId, "contrast");
        mContrastValue = 1.0f;
    }

    void setAdjustEffect(float percent) {
        mContrastValue = range(percent*100.0f, 0.0f, 4.0f);
        //mContrastValue = percent * 4.0f;
    }

    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                        void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks
        glUniform1f(mContrastLocation, mContrastValue);

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
    std::string CONTRAST_FRAGMENT_SHADER;

    GLint   mContrastLocation;
    float   mContrastValue;
};
#endif //GPU_CONTRAST_FILTER_HPP
