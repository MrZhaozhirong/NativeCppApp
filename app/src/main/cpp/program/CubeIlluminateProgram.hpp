//
// Created by zzr on 2019/6/5.
//
#pragma once
#ifndef NATIVECPPAPP_CUBEILLUMINATEPROGRAM_HPP
#define NATIVECPPAPP_CUBEILLUMINATEPROGRAM_HPP

#include "ShaderProgram.h"
#include "ShaderHelper.h"

class CubeIlluminateProgram : public ShaderProgram
{
public:
    GLint       _mvp;
    GLint       _lightDir;
    GLint       _lightColor;
    GLint       _lightDiffuse;
    GLint       _texture;
    GLint       _position;
    GLint       _normal;
    GLint       _uv;
public:
    virtual void    initialize()
    {
        const char* vs  =  "#version 320 es\n\
                            uniform mat4   _mvp;\n\
                            uniform vec3   _lightDir;\n\
                            uniform vec3   _lightColor;\n\
                            uniform vec3   _lightDiffuse;\n\
                            in      vec3   _position;\n\
                            in      vec3   _normal;\n\
                            in      vec2   _uv;\n\
                            out     vec2   _outUV;\n\
                            out     vec4   _outComposeColor;\n\
                            void main()\n\
                            {\n\
                                _outUV                =   _uv; \n\
                                float lightStrength   =   max(dot(_normal, -_lightDir), 0.0); \n\
                                _outComposeColor =   vec4(_lightColor * lightStrength + _lightDiffuse, 1);\n\
                                gl_Position      =   _mvp * vec4(_position,1.0);\n\
                            }";

        const char* fs =   "#version 320 es\n\
                            precision mediump float;\n\
                            in      vec4        _outComposeColor;\n\
                            in      vec2        _outUV;\n\
                            uniform sampler2D   _texture;\n\
                            out     vec4        _fragColor;\n\
                            void main()\n\
                            {\n\
                                vec4    color   =   texture(_texture,_outUV);\n\
                                _fragColor      =   color * _outComposeColor;\n\
                            }";

        programId = ShaderHelper::buildProgram(vs, fs);
        _mvp        =   glGetUniformLocation(programId,  "_mvp");
        _lightDir   =   glGetUniformLocation(programId,  "_lightDir");
        _lightColor =   glGetUniformLocation(programId,  "_lightColor");
        _lightDiffuse = glGetUniformLocation(programId,  "_lightDiffuse");

        _position   =   glGetAttribLocation(programId,   "_position");
        _normal     =   glGetAttribLocation(programId,   "_normal");
        _uv         =   glGetAttribLocation(programId,   "_uv");

        _texture    =   glGetUniformLocation(programId,  "_texture");
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
#endif //NATIVECPPAPP_CUBEILLUMINATEPROGRAM_H
