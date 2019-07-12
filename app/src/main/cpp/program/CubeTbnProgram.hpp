//
// Created by nicky on 2019/7/11.
//

#ifndef NATIVECPPAPP_CUBETBNPROGRAM_HPP
#define NATIVECPPAPP_CUBETBNPROGRAM_HPP


#include "ShaderProgram.h"
#include "ShaderHelper.h"

class CubeTbnProgram : public ShaderProgram {
public:
    GLint           _position;
    GLint           _normal;
    GLint           _uv;
    GLint           _tagent;
    GLint           _biTagent;

    GLint           _mvp;
    GLint           _normalMatrix;
    GLint           _matModel;

    GLint           _lightColor;
    GLint           _lightDiffuse;
    GLint           _texture;
    GLint           _texNormal;

    GLint           _lightPos;
    GLint           _cameraPos;



public:
    virtual void    initialize()
    {
        const char* vs  =  "#version 320 es\n\
                            in      vec3    _position;\n\
                            in      vec3    _normal;\n\
                            in      vec2    _uv;\n\
                            in      vec3    _tagent;\n\
                            in      vec3    _biTagent;\n\
                            uniform mat4    _mvp;\n\
                            uniform mat3    _normalMatrix;\n\
                            uniform mat4    _matModel;\n\
                            out     vec2    _outUV;\n\
                            out     vec3    _outPos;\n\
                            out     mat3    _TBN;\n\
                            void main() \n\
                            {\n\
                               _outUV          =   _uv;\n\
                               vec4    pos     =   _matModel*vec4(_position,1);\n\
                               _outPos         =   pos.xyz;\n\
                               vec3    normal  =   normalize(_normalMatrix * _normal);\n\
                               vec3    tagent  =   normalize(_normalMatrix * _tagent);\n\
                               vec3    biTagent=   normalize(_normalMatrix * _biTagent);\n\
                               _TBN            =   mat3x3(tagent,biTagent,normal);\n\
                               gl_Position     =   _mvp * vec4(_position,1.0);\n\
                            }";

        const char* fs  =  "#version 320 es\n\
                            precision highp float;\n\
                            in      vec2      _outUV;\n\
                            in      vec3      _outPos;\n\
                            in      mat3      _TBN;\n\
                            uniform vec3      _lightColor;\n\
                            uniform vec3      _lightDiffuse;\n\
                            uniform sampler2D _texture;\n\
                            uniform sampler2D _texNormal;\n\
                            uniform vec3      _lightPos;\n\
                            uniform vec3      _cameraPos;\n\
                            out     vec3      _fragColor;\n\
                            void main()\n\
                            {\n\
                               vec3   cameraDir  =  normalize(_cameraPos - _outPos);\n\
                               vec3   lightDir   =  normalize(_lightPos  - _outPos);\n\
                               vec3   normal     =  normalize(_TBN * (texture(_texNormal,_outUV).xyz*2.0 - 1.0));\n\
                               vec4   materialColor =  texture(_texture,_outUV);\n\
                               float  lightStrength =  max(dot(normal, lightDir), 0.0);\n\
                               vec4   lightColor    =  vec4(_lightColor * lightStrength + _lightDiffuse, 1);\n\
                               _fragColor.rgb = materialColor.rgb * 0.2 + 0.8 * lightColor.rgb;\n\
                            }";

        programId   =   ShaderHelper::buildProgram(vs, fs);
        _position   =   glGetAttribLocation(programId,  "_position");
        _normal     =   glGetAttribLocation(programId,  "_normal");
        _uv         =   glGetAttribLocation(programId,  "_uv");
        _tagent     =   glGetAttribLocation(programId,  "_tagent");
        _biTagent   =   glGetAttribLocation(programId,  "_biTagent");

        _mvp          = glGetUniformLocation(programId, "_mvp");
        _normalMatrix = glGetUniformLocation(programId, "_normalMatrix");
        _matModel     = glGetUniformLocation(programId, "_matModel");

        _lightColor   = glGetUniformLocation(programId, "_lightColor");
        _lightDiffuse = glGetUniformLocation(programId, "_lightDiffuse");

        _texture      = glGetUniformLocation(programId, "_texture");
        _texNormal    = glGetUniformLocation(programId, "_texNormal");

        _lightPos     = glGetUniformLocation(programId, "_lightPos");
        _cameraPos    = glGetUniformLocation(programId, "_cameraPos");
    }



    /**
    *   使用shader
    */
    virtual void    begin()
    {
        glUseProgram(programId);
        glEnableVertexAttribArray(_position);
        glEnableVertexAttribArray(_normal);
        glEnableVertexAttribArray(_uv);
        glEnableVertexAttribArray(_tagent);
        glEnableVertexAttribArray(_biTagent);
    }
    /**
    *   结束
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
        glDisableVertexAttribArray(_normal);
        glDisableVertexAttribArray(_uv);
        glDisableVertexAttribArray(_tagent);
        glDisableVertexAttribArray(_biTagent);
        glUseProgram(0);
    }
};


#endif //NATIVECPPAPP_CUBETBNPROGRAM_HPP
