//
// Created by nicky on 2019/4/8.
//

#ifndef NATIVECPPAPP_GPUMIXSHADERPROGRAM_H
#define NATIVECPPAPP_GPUMIXSHADERPROGRAM_H


#include "ShaderProgram.h"
#include "../common/constructormagic.h"

class GPUMixShaderProgram  : public ShaderProgram{
public:
    //static char * vertexShaderResourceStr ;
    //static char * fragmentShaderResourceStr ;

    GPUMixShaderProgram();
    ~GPUMixShaderProgram();

    void setMVPUniforms(float matrix[]);
    void setMixUniform(double mix);

public:
    GLint uMatrixLocation;
    GLint aPositionLocation;
    GLint uMixLocation;
    GLint aTexUvLocation;
    GLint uTextureUnit0;
    GLint uTextureUnit1;

private:
    DISALLOW_EVIL_CONSTRUCTORS(GPUMixShaderProgram);
};


#endif //NATIVECPPAPP_GPUMIXSHADERPROGRAM_H
