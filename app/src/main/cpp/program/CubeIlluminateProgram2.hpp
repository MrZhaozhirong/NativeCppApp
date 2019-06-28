//
// Created by zzr on 2019/6/5.
//
#pragma once
#ifndef NATIVECPPAPP_CUBEILLUMINATEPROGRAM2_HPP
#define NATIVECPPAPP_CUBEILLUMINATEPROGRAM2_HPP

#include "ShaderProgram.h"
#include "ShaderHelper.h"

class CubeIlluminateProgram2 : public ShaderProgram
{
public:
    GLint       _mvp;
    GLint       _lightDir;
    GLint       _lightColor;
    GLint       _lightDiffuse;
    GLint       _texture;
    GLint       _position;
    GLint       _normal;
    GLint       _normalMatrix;
    GLint       _uv;
public:
    virtual void    initialize()
    {
        const char* vs  =  "#version 320 es\n\
                            uniform mat4   _mvp;\n\
                            in      vec3   _position;\n\
                            in      vec3   _normal;\n\
                            in      vec2   _uv;\n\
                            out     vec2   _fragTexUV;\n\
                            out     vec3   _fragNormal;\n\
                            void main()\n\
                            {\n\
                                _fragTexUV  = _uv;\n\
                                _fragNormal = _normal;\n\
                                gl_Position = _mvp * vec4(_position,1.0);\n\
                            }";

        const char* fs =   "#version 320 es\n\
                            precision mediump float;\n\
                            in        vec3        _fragNormal;\n\
                            in        vec2        _fragTexUV;\n\
                            uniform   vec3        _lightColor;\n\
                            uniform   vec3        _lightDiffuse;\n\
                            uniform   vec3        _lightDir;\n\
                            uniform   mat3        _normalMatrix;\n\
                            uniform   sampler2D   _texture;\n\
                            out       vec4        _fragColor;\n\
                            void main()\n\
                            {\n\
                                vec3  normalizedLightDir = normalize(-_lightDir);\n\
                                vec3  transformedNormal  = normalize(_normalMatrix * _fragNormal);\n\
                                float lightStrength      = max( dot(normalizedLightDir, transformedNormal), 0.0);\n\
                                vec4  composeColor       = vec4(_lightColor * lightStrength + _lightDiffuse, 1.0);\n\
                                vec4  color              = texture(_texture, _fragTexUV);\n\
                                _fragColor               = color * composeColor;\n\
                            }";

        programId = ShaderHelper::buildProgram(vs, fs);
        _mvp        =   glGetUniformLocation(programId,  "_mvp");
        _position   =   glGetAttribLocation(programId,   "_position");
        _normal     =   glGetAttribLocation(programId,   "_normal");
        _uv         =   glGetAttribLocation(programId,   "_uv");

        _lightColor =   glGetUniformLocation(programId,  "_lightColor");
        _lightDiffuse = glGetUniformLocation(programId,  "_lightDiffuse");
        _lightDir   =   glGetUniformLocation(programId,  "_lightDir");
        _normalMatrix = glGetUniformLocation(programId,  "_normalMatrix");
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
#endif //NATIVECPPAPP_CUBEILLUMINATEPROGRAM2_HPP
