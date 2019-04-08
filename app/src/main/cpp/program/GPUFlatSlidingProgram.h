//
// Created by nicky on 2019/4/8.
//

#ifndef NATIVECPPAPP_GPUFLATSLIDINGPROGRAM_H
#define NATIVECPPAPP_GPUFLATSLIDINGPROGRAM_H


#include "ShaderProgram.h"
#include "../common/constructormagic.h"

class GPUFlatSlidingProgram : public ShaderProgram{
public:
    //static char * vertexShaderResourceStr ;
    //static char * fragmentShaderResourceStr ;

    GPUFlatSlidingProgram();
    ~GPUFlatSlidingProgram();

    void setMVPUniforms(float matrix[]);
    void setOffsetUniform(double mix);
public:
    GLint uMatrixLocation;
    GLint aPositionLocation;
    GLint aTexUvLocation;
    GLint uTextureUnit;

    GLint uOffset;
    GLint uTextureUnit0;
    GLint uTextureUnit1;

private:
    DISALLOW_EVIL_CONSTRUCTORS(GPUFlatSlidingProgram);
};


#endif //NATIVECPPAPP_GPUFLATSLIDINGPROGRAM_H
