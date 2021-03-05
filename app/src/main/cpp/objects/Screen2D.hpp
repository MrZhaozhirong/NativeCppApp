//
// Created by nicky on 2021/3/3.
//

#ifndef NATIVECPPAPP_SCREEN2D_HPP
#define NATIVECPPAPP_SCREEN2D_HPP

#include <string>
#include <GLES3/gl3.h>
#include "../program/ShaderHelper.h"
#include "../common/VertexBufferObject.hpp"

class Screen2D {
public:
    Screen2D()
    {
        VERTEX_SHADER    = "#version 300 es\n\
                            layout(location = 0) in vec3 position;\n\
                            layout(location = 1) in vec2 inputTextureCoordinate;\n\
                            out vec2 textureCoordinate;\n\
                            void main()\n\
                            {\n\
                               gl_Position = vec4(position, 1.0);\n\
                               textureCoordinate = inputTextureCoordinate.xy;\n\
                            }";

        FRAGMENT_SHADER  = "#version 300 es\n\
                            precision mediump float;\n\
                            in vec2 textureCoordinate;\n\
                            uniform sampler2D s_texture;\n\
                            out vec4 fragColor;\n\
                            void main()\n\
                            {\n\
                                fragColor = texture(s_texture, textureCoordinate);\n\
                            }";

        V3T2 verts[] =  {
                            {-1.0f, 1.0f,  0.0f,  0.0f, 1.0f},
                            { 1.0f, 1.0f,  0.0f,  1.0f, 1.0f},
                            {-1.0f, -1.0f, 0.0f,  0.0f, 0.0f},
                            { 1.0f, -1.0f, 0.0f,  1.0f, 0.0f},
                            {-1.0f, -1.0f, 0.0f,  0.0f, 0.0f},
                            { 1.0f, 1.0f,  0.0f,  1.0f, 1.0f},
                        };
        memcpy(_data, verts, sizeof(verts));
    }
    ~Screen2D()
    {
        if(!VERTEX_SHADER.empty()) VERTEX_SHADER.clear();
        if(!FRAGMENT_SHADER.empty()) FRAGMENT_SHADER.clear();
        glDeleteProgram(mGLProgId);
        mIsInitialized = false;
    }

    void init()
    {
        mGLProgId = ShaderHelper::buildProgram(VERTEX_SHADER.c_str(), FRAGMENT_SHADER.c_str());
        mTextureLoc = glGetUniformLocation(mGLProgId, "s_texture");
        _vbo.bindData(_data, sizeof(V3T2) * 6);
        mIsInitialized = true;
    }


    void draw()
    {
        glUseProgram (mGLProgId);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glUniform1i(mTextureLoc, 0);

        glBindBuffer(GL_ARRAY_BUFFER, _vbo.getVertexBufferID());
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3T2), (const void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(V3T2), (const void *)(3*sizeof(float)));

        // glEnableVertexAttribArray(0);
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3T2), &_data[0].x);
        // glEnableVertexAttribArray(1);
        // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(V3T2), &_data[0].u);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        // glBindTexture(GL_TEXTURE_2D, 0);
        // glDisableVertexAttribArray(0);
        // glDisableVertexAttribArray(1);
    }


    void setTextureId(GLuint texId) {
        mTextureId = texId;
    }

private:
    struct V3T2 {
        float x, y, z; //位置坐标
        float u, v; //纹理坐标
    };

    V3T2                  _data[6];
    VertexBufferObject    _vbo;

    std::string VERTEX_SHADER;
    std::string FRAGMENT_SHADER;

    int     mTextureId;

    GLuint  mGLProgId;
    GLint   mTextureLoc;
    bool    mIsInitialized;
};

#endif //NATIVECPPAPP_SCREEN2D_HPP
