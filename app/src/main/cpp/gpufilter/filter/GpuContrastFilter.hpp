//
// Created by nicky on 2019/12/19.
//

#ifndef GPU_CONTRAST_FILTER_HPP
#define GPU_CONTRAST_FILTER_HPP

#include "GpuNormalFilter.hpp"

class GpuContrastFilter : public GpuNormalFilter {
public:
    int getTypeId() { return 1; }

    GpuContrastFilter()
    {
        //LOGI("---GpuContrastFilter构造, %p",this);
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
        //LOGI("---GpuContrastFilter析构, %p",this);
    }

    void init() {
        GpuNormalFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), CONTRAST_FRAGMENT_SHADER.c_str());
        mContrastLocation = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "contrast"));
        mContrastValue = 1.0f;
    }

    void setContrast(float contrast) {
        mContrastValue = contrast;
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

    int     mContrastLocation;
    float   mContrastValue;
};
#endif //GPU_CONTRAST_FILTER_HPP
