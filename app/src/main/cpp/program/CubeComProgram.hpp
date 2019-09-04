//
// Created by nicky on 2019/9/4.
//

#ifndef NATIVECPPAPP_CUBECOMPROGRAM_HPP
#define NATIVECPPAPP_CUBECOMPROGRAM_HPP

#include "ShaderProgram.h"
#include "ShaderHelper.h"

class CubeComProgram : public ShaderProgram
{
public:
    GLint       _mvp;
    GLint       _lightColor;
    GLint       _texture;
    GLint       _position;
    GLint       _normal;
    GLint       _uv;
public:
    virtual void    initialize()
    {
        const char* vs  =   "uniform mat4    _mvp;\n\
                             attribute vec4  _position;\n\
                             attribute vec2  _uv;\n\
                             varying vec2    out_uv;\n\
                             void main()\n\
                             {\n\
                                   out_uv = _uv;\n\
                                   gl_Position = _mvp * _position;\n\
                             }";

        const char* fs =   "precision mediump float;\n\
                            uniform sampler2D _texture;\n\
                            uniform vec3      _color;\n\
                            varying vec2      out_uv;\n\
                            void main()\n\
                            {\n\
                                //gl_FragColor = texture2D(_texture, out_uv);\n\
                                gl_FragColor = _color;\n\
                            }";

        programId   =   ShaderHelper::buildProgram(vs, fs);
        _mvp        =   glGetUniformLocation(programId, "_mvp");
        _position   =   glGetAttribLocation(programId,  "_position");
        _uv         =   glGetAttribLocation(programId,  "_uv");
        _texture    =   glGetUniformLocation(programId, "_texture");
        _lightColor =   glGetUniformLocation(programId, "_lightColor");
    }



    /**
    *   使用shader
    */
    virtual void    begin()
    {
        glEnableVertexAttribArray(_position);
        glEnableVertexAttribArray(_normal);
        glEnableVertexAttribArray(_uv);
        glUseProgram(programId);
    }
    /**
    *   结束
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
        glDisableVertexAttribArray(_normal);
        glDisableVertexAttribArray(_uv);
        glUseProgram(0);
    }
};
#endif //NATIVECPPAPP_CUBECOMPROGRAM_HPP
