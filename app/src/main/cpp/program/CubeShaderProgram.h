//
// Created by nicky on 2019/3/18.
//

#ifndef NATIVECPPAPP_CUBESHADERPROGRAM_H
#define NATIVECPPAPP_CUBESHADERPROGRAM_H


#include "ShaderProgram.h"
#include "../common/constructormagic.h"

class CubeShaderProgram : public ShaderProgram{
public:
    //static char * vertexShaderResourceStr ;
    //static char * fragmentShaderResourceStr ;

    CubeShaderProgram();
    ~CubeShaderProgram();

    void setUniforms(float matrix[]);

public:
    GLint uMatrixLocation;
    GLint aPositionLocation;
    GLint aColorLocation;

private:
    DISALLOW_EVIL_CONSTRUCTORS(CubeShaderProgram);
};


#endif //NATIVECPPAPP_CUBESHADERPROGRAM_H
