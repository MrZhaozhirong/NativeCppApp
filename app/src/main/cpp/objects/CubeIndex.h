//
// Created by nicky on 2019/3/18.
//

#ifndef NATIVECPPAPP_CUBEINDEX_H
#define NATIVECPPAPP_CUBEINDEX_H


#include "../program/CubeShaderProgram.h"

class CubeIndex {
public:
    static const int POSITION_COMPONENT_COUNT ;
    static const int COLOR_COMPONENT_COUNT ;
    static const int STRIDE ;
public:
    CubeIndex();
    ~CubeIndex();

    void draw();
    void bindData(CubeShaderProgram shaderProgram);
private:
    char CUBE_VERTEX_DATA[];
    char CUBE_INDEX[];
};


#endif //NATIVECPPAPP_CUBEINDEX_H
