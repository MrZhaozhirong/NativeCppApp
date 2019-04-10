//
// Created by nicky on 2019/4/10.
//
#pragma once
#ifndef NATIVECPPAPP_GRASSLAND_PROGRAM_HPP
#define NATIVECPPAPP_GRASSLAND_PROGRAM_HPP

#include "ShaderProgram.h"
#include "ShaderHelper.h"
#include "../common/constructormagic.h"

class GrasslandProgram : public ShaderProgram{
public:
    GrasslandProgram()
    {
        const char* vs  ="\n\
                        uniform     mat4   _mvp;\n\
                        attribute   vec3   _position;\n\
                        attribute   vec3   _normal;\n\
                        attribute   vec2   _uv;\n\
                        varying     vec2   out_uv;\n\
                        void main()\n\
                        {\n\
                            out_uv      =   _uv;\n\
                            gl_Position =   _mvp * vec4(_position,1.0);\n\
                        }";

        const char* ps ="precision mediump float;\n\
                        uniform sampler2D  _texture;\n\
                        varying    vec2    out_uv;\n\
                        void main()\n\
                        {\n\
                            gl_FragColor = texture2D(_texture, out_uv);\n\
                        }";

        programId = ShaderHelper::buildProgram(vs, ps);

        uMVPMatrixLocation  = glGetUniformLocation(programId, "_mvp");
        aPositionLocation   = glGetAttribLocation(programId,  "_position");
        aTexUvLocation      = glGetAttribLocation(programId,  "_uv");
        aNormalLocation     = glGetAttribLocation(programId,  "_normal");
        uTextureUnit        = glGetUniformLocation(programId, "_texture");
    }

    void setMVPUniforms(float* matrix){
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, matrix);
    }

public:
    GLint uMVPMatrixLocation;
    GLint aPositionLocation;
    GLint aTexUvLocation;
    GLint aNormalLocation;
    GLint uTextureUnit;

private:
    DISALLOW_EVIL_CONSTRUCTORS(GrasslandProgram);
};


#endif
