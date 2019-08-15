//
// Created by nicky on 2019/8/14.
//

#ifndef DEPTH_ORTHO_SHADER_HPP
#define DEPTH_ORTHO_SHADER_HPP

#include "ShaderProgram.h"
#include "ShaderHelper.h"

class DepthOrthoShader : public ShaderProgram
{
public:
    GLint       _position;
    GLint       _uv;
    GLint       _mvp;
    GLint       _tex;

public:
    virtual void    initialize()
    {
        const char* vs  =   "#version 320 es\n\
                            in      vec3    _position;\n\
                            in      vec2    _uv;\n\
                            uniform mat4    _mvp;\n\
                            out     vec2    _outUV;\n\
                            void main()\n\
                            {\n\
                                _outUV      =   _uv;\n\
                                gl_Position =   _mvp * vec4(_position,1.0);\n\
                            }";

        const char* fs =    "#version 320 es\n\
                            precision mediump float;\n\
                            in      vec2        _outUV;\n\
                            uniform sampler2D   _tex;\n\
                            out     vec4        _fragColor;\n\
                            float restDepth(vec4 RGBA){\n\
                                const float rMask = 1.0;\n\
                                const float gMask = 1.0 / 255.0;\n\
                                const float bMask = 1.0 / (255.0 * 255.0);\n\
                                const float aMask = 1.0 / (255.0 * 255.0 * 255.0);\n\
                                float depth = dot(RGBA, vec4(rMask, gMask, bMask, aMask));\n\
                                return depth;\n\
                            }\n\
                            void main()\n\
                            {\n\
                                vec4    color = texture(_tex, _outUV);\n\
                                float   dd = restDepth(color);\n\
                                _fragColor = vec4(dd,dd,dd,1);\n\
                            }";

        programId   =   ShaderHelper::buildProgram(vs, fs);

        _mvp        =   glGetUniformLocation(programId, "_mvp");
        _tex        =   glGetUniformLocation(programId, "_tex");
        _position   =   glGetAttribLocation(programId,  "_position");
        _uv         =   glGetAttribLocation(programId,  "_uv");
    }


    virtual void    begin()
    {
        glUseProgram(programId);
        glEnableVertexAttribArray(_position);
        glEnableVertexAttribArray(_uv);
    }

    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
        glDisableVertexAttribArray(_uv);
        glUseProgram(0);
    }
};
#endif // DEPTH_ORTHO_SHADER_HPP
