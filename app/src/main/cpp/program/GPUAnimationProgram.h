//
// Created by nicky on 2019/4/2.
//

#ifndef NATIVECPPAPP_GPUANIMATIONPROGRAM_H
#define NATIVECPPAPP_GPUANIMATIONPROGRAM_H

#include "../common/constructormagic.h"
#include "ShaderProgram.h"

class GPUAnimationProgram : public ShaderProgram{

public:
    GPUAnimationProgram();
    ~GPUAnimationProgram();

    void setMVPUniforms(float matrix[]);
    void setAnimUniforms(int row,int col,int frame);

public:
    GLint uMatrixLocation;
    GLint uAnimInforLocation;

    GLint aPositionLocation;
    GLint aTexUvLocation;
    GLint uTextureUnit;

private:
    DISALLOW_EVIL_CONSTRUCTORS(GPUAnimationProgram);
};


#endif //NATIVECPPAPP_GPUANIMATIONPROGRAM_H
