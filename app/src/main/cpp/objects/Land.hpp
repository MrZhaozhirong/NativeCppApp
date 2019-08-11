//
// Created by zzr on 2019/8/11.
//

#ifndef LAND_HPP
#define LAND_HPP

#include <GLES3/gl3.h>
#include "../common/CELLMath.hpp"
#include "../program/GrasslandProgram.hpp"

class Land {
public:
    struct V3N3UV2 {
        float x, y, z; //位置坐标
        float nx, ny, nz; //法向量
        float u,v; //纹理坐标
    };

public:
    V3N3UV2          _data[6];
    CELL::matrix4    _modelMatrix;
    GLuint           _TexId;
    GrasslandProgram _sprogram;


    void        init(const float size, GLuint tex)
    {
        float   gSizeX = 10;
        float   gSizeZ = 10;
        float   gPos = 0;

        V3N3UV2 verts[] =
        {
                {-gSizeX, gPos,-gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
                { gSizeX, gPos,-gSizeZ, 0,1,0, size, 0.0f}, // right far
                { gSizeX, gPos, gSizeZ, 0,1,0, size, size}, // right near

                {-gSizeX, gPos,-gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
                { gSizeX, gPos, gSizeZ, 0,1,0, size, size}, // right near
                {-gSizeX, gPos, gSizeZ, 0,1,0, 0.0f, size}, // left near
        };
        memcpy(_data, verts, sizeof(verts));
        _TexId = tex;
        _modelMatrix.identify();
    }



};
#endif //LAND_HPP
