//
// Created by nicky on 2019/4/10.
//

#ifndef NATIVECPPAPP_GRASSLAND_H
#define NATIVECPPAPP_GRASSLAND_H


#include "../program/GrasslandProgram.hpp"
#include "../common/CELLMath.hpp"

class Grassland {
public:
    struct  GrasslandVertex
    {
        float   x,y,z;
        float   nx,ny,nz;
        float   u,v;
    };
public:
    Grassland();
    ~Grassland();

    void draw();
    void bindData(GrasslandProgram * shaderProgram);
    void unbind(GrasslandProgram * shaderProgram);
public:
    float * modelMatrix;
    CELL::matrix4   matModel;

private:
    GrasslandVertex * VERTEX_DATA;
};


#endif //NATIVECPPAPP_GRASSLAND_H
