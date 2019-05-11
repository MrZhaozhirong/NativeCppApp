
#pragma once
#ifndef GREENERY_PROGRAM_HPP
#define GREENERY_PROGRAM_HPP

#include "ShaderProgram.h"
#include "ShaderHelper.h"

class GreeneryShaderProgram : public ShaderProgram
{
public:
    GLint _position;
    GLint _mvp     ;
    GLint _rightDir;
    GLint _upDir   ;
    GLint _texture ;

public:
    GreeneryShaderProgram() {}
    ~GreeneryShaderProgram() {}

    /// 初始化函数
    virtual void    initialize()
    {
        const char * vertexShaderResourceStr =
        {

                "uniform     vec3     _rightDir; \n\
                uniform     vec3     _upDir; \n\
                uniform     mat4     _mvp; \n\
                attribute   vec3     _pos; \n\
                varying     vec2     _texcoord;\n\
                void main(void ) \n\
                {\n\
                    const vec2 uvData[6] = vec2[6]( \n\
                                            vec2(0.0, 0.0),\n\
                                            vec2(0.0, 1.0),\n\
                                            vec2(1.0, 1.0),\n\
                                            vec2(0.0, 0.0),\n\
                                            vec2(1.0, 1.0),\n\
                                            vec2(1.0, 0.0) );\n\
                    _texcoord       =   uvData[gl_VertexID];\n\
                    float _texcoord_x       =   _texcoord.x;\n\
                    float _texcoord_y       =   _texcoord.y;\n\
                    vec3 newPs      =   _pos + ((_texcoord_x - 0.5)* 4.0)* _rightDir + (_texcoord_y * 4.0) * _upDir;\n\
                    gl_Position     =   _mvp * vec4(newPs.x,newPs.y,newPs.z,1);\n\
                }\n"
        };

        const char * fragmentShaderResourceStr =
        {
                "uniform sampler2D  _texture;\n\
                 varying vec2       _texcoord;\n\
                 void main(void)\n\
                 {\n\
                    vec4   color   =   texture2D(_texture,vec2(_texcoord.x, 1.0-_texcoord.y)); \n\
                    if(color.a < 0.2) discard;\n\
                    gl_FragColor   =   color;\n\
                 }\n"
        };

        programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);

        _position   =   glGetAttribLocation (programId, "_pos");
        _mvp        =   glGetUniformLocation(programId, "_mvp");
        _rightDir   =   glGetUniformLocation(programId, "_rightDir");
        _upDir      =   glGetUniformLocation(programId, "_upDir");
        _texture    =   glGetUniformLocation(programId, "_texture");

    }


    /**
    *   使用程序
    */
    virtual void    begin()
    {
        glUseProgram(programId);
        glEnableVertexAttribArray(static_cast<GLuint>(_position));
    }
    /**
    *   使用完成
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(static_cast<GLuint>(_position));
        glUseProgram(0);
    }
};


#endif //GREENERY_PROGRAM_HPP