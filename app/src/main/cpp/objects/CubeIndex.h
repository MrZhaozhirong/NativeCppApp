//
// Created by nicky on 2019/3/18.
//

#ifndef NATIVECPPAPP_CUBEINDEX_H
#define NATIVECPPAPP_CUBEINDEX_H


#include "../program/CubeShaderProgram.h"
#include "../program/GPUAnimationProgram.h"
#include "../program/GPUMixShaderProgram.h"
#include "../program/GPUFlatSlidingProgram.h"

class CubeIndex {
public:
    static const int POSITION_COMPONENT_COUNT = 3;
    static const int COLOR_COMPONENT_COUNT = 3;
    static const int TEXTURE_COORDINATE_COMPONENT_COUNT = 2;
    static const int STRIDE = POSITION_COMPONENT_COUNT + TEXTURE_COORDINATE_COMPONENT_COUNT;
public:
    CubeIndex();
    ~CubeIndex();

    void draw();
    void bindData(CubeShaderProgram * shaderProgram);
    void bindData(GPUAnimationProgram * shaderProgram);
    void bindData(GPUMixShaderProgram * shaderProgram);
    void bindData(GPUFlatSlidingProgram * shaderProgram);
public:
    float * modelMatrix;

private:
    int8_t * CUBE_VERTEX_DATA;
    int8_t * CUBE_INDEX;
};


#endif //NATIVECPPAPP_CUBEINDEX_H
