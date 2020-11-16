//
// Created by nicky on 2020/11/16.
//

#ifndef DOUYIN_ELECTRIC_SHOCK_FILTER_HPP
#define DOUYIN_ELECTRIC_SHOCK_FILTER_HPP

#include "GpuBaseFilter.hpp"

class DouYinElectricShockFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_DOUYIN_SHOCK; }

    DouYinElectricShockFilter()
    {
        SHOCK_FRAGMENT_SHADER     ="precision mediump float;\n\
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
                                    uniform int is_shock;\n\
                                    void main()\n\
                                    {\n\
                                        vec4 textureColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                        if (is_shock==0) \n\
                                        {\n\
                                            gl_FragColor = textureColor;\n\
                                        }\n\
                                        else\n\
                                        {\n\
                                            gl_FragColor = vec4( (1.0-textureColor.rgb), textureColor.w);\n\
                                        }\n\
                                    }";
        mInputFps = 0;
        mCurrentFps = 0;
    }

    ~DouYinElectricShockFilter() {
        if(!SHOCK_FRAGMENT_SHADER.empty()) SHOCK_FRAGMENT_SHADER.clear();
    }

    void init() {
        GpuBaseFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), SHOCK_FRAGMENT_SHADER.c_str());
        mShockLocation = glGetUniformLocation(mGLProgId, "is_shock");
    }


    void setShockFps(int inputFps, int currentFps) {
        mInputFps = inputFps;
        mCurrentFps = currentFps;
    }

    void onDraw(GLuint SamplerY_texId, GLuint SamplerU_texId, GLuint SamplerV_texId,
                void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;
        glUseProgram(mGLProgId);
        glUniform1i(mDrawModeLocation, 0);

        glVertexAttribPointer(mGLAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, positionCords);
        glEnableVertexAttribArray(mGLAttribPosition);
        glVertexAttribPointer(mGLAttribTextureCoordinate, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(mGLAttribTextureCoordinate);
        // runPendingOnDrawTasks();
        if (mInputFps==0&&mCurrentFps==0) {
            mShock = 0;
        } else {
            int halfInputFps = mInputFps/2;
            if (mCurrentFps==halfInputFps
                ||mCurrentFps==halfInputFps+1
                ||mCurrentFps==halfInputFps+2
                ||mCurrentFps==halfInputFps+3
                ||mCurrentFps==halfInputFps+4)
            {
                mShock = 1;
            }
            else
            {
                mShock = 0;
            }
        }

        glUniform1i(mShockLocation, mShock);
        if (mShock==0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, SamplerY_texId);
            glUniform1i(mGLUniformSampleY, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, SamplerU_texId);
            glUniform1i(mGLUniformSampleU, 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, SamplerV_texId);
            glUniform1i(mGLUniformSampleV, 2);
        } else {
            glActiveTexture(GL_TEXTURE0);
            glActiveTexture(GL_TEXTURE1);
            glActiveTexture(GL_TEXTURE2);
        }
        // onDrawArraysPre();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(mGLAttribPosition);
        glDisableVertexAttribArray(mGLAttribTextureCoordinate);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


private:
    std::string SHOCK_FRAGMENT_SHADER;
    GLint mShockLocation;
    int mShock;

    int mInputFps;
    int mCurrentFps;
};
#endif // DOUYIN_ELECTRIC_SHOCK_FILTER_HPP
