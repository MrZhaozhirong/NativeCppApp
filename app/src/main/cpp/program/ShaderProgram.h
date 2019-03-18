//
// Created by nicky on 2019/3/18.
//

#ifndef NATIVECPPAPP_SHADERPROGRAM_H
#define NATIVECPPAPP_SHADERPROGRAM_H


#include <GLES2/gl2.h>

class ShaderProgram {
protected:
    GLuint programId;

public:
    ShaderProgram();
    ShaderProgram( const char * vertexShaderResourceStr,
                   const char * fragmentShaderResourceStr );
    virtual ~ShaderProgram();

    int getShaderProgramId();

    void userProgram();

    void deleteProgram();
};


#endif //NATIVECPPAPP_SHADERPROGRAM_H
