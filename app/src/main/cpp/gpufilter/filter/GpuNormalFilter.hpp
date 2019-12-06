//
// Created by nicky on 2019/12/5.
//

#ifndef GPU_NORMAL_FILTER_HPP
#define GPU_NORMAL_FILTER_HPP

#include <string>
#include "../../program/ShaderHelper.h"

class GpuNormalFilter  {
public:
    GpuNormalFilter()
    {
        NO_FILTER_FRAGMENT_SHADER = "attribute vec4 position;\n\
                                     attribute vec4 inputTextureCoordinate;\n\
                                     varying vec2 textureCoordinate;\n\
                                     void main()\n\
                                     {\n\
                                         gl_Position = position;\n\
                                         textureCoordinate = inputTextureCoordinate.xy;\n\
                                     }";

        NO_FILTER_VERTEX_SHADER   = "varying highp vec2 textureCoordinate;\n\
                                     uniform sampler2D inputImageTexture;\n\
                                     void main()\n\
                                     {\n\
                                          gl_FragColor = texture2D(inputImageTexture, textureCoordinate);\n\
                                     }";
    }
    virtual ~GpuNormalFilter()
    {
        NO_FILTER_VERTEX_SHADER = NULL;
        NO_FILTER_FRAGMENT_SHADER = NULL;
        mIsInitialized = false;
    }

    void init() {
        mGLProgId = ShaderHelper::buildProgram(NO_FILTER_VERTEX_SHADER.c_str(), NO_FILTER_FRAGMENT_SHADER.c_str());
        mGLAttribPosition = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "position"));
        mGLUniformTexture = static_cast<GLuint>(glGetUniformLocation(mGLProgId, "inputImageTexture"));
        mGLAttribTextureCoordinate = static_cast<GLuint>(glGetAttribLocation(mGLProgId, "inputTextureCoordinate"));
        mIsInitialized = true;
    }

    void destroy() {
        mIsInitialized = false;
        glDeleteProgram(mGLProgId);
    }

    void onOutputSizeChanged(int width, int height) {
        mOutputWidth = width;
        mOutputHeight = height;
    }

    void onDraw(GLuint textureId, void* positionCords, void* textureCords)
    {
        if (!mIsInitialized)
            return;

        glUseProgram(mGLProgId);
        // runPendingOnDrawTasks();
        glVertexAttribPointer(mGLAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, positionCords);
        glEnableVertexAttribArray(mGLAttribPosition);
        glVertexAttribPointer(mGLAttribTextureCoordinate, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
        glEnableVertexAttribArray(mGLAttribTextureCoordinate);
        if (textureId != -1) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
            glUniform1i(mGLUniformTexture, 0);
        }
        // onDrawArraysPre();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisableVertexAttribArray(mGLAttribPosition);
        glDisableVertexAttribArray(mGLAttribTextureCoordinate);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

public:
    std::string NO_FILTER_VERTEX_SHADER;
    std::string NO_FILTER_FRAGMENT_SHADER;
protected:
    GLuint  mGLProgId;
    GLuint  mGLAttribPosition;
    GLuint  mGLUniformTexture;
    GLuint  mGLAttribTextureCoordinate;
private:
    int     mOutputWidth;
    int     mOutputHeight;
    bool    mIsInitialized;
};

#endif // GPU_NORMAL_FILTER_HPP
