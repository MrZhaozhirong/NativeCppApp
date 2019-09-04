//
// Created by nicky on 2019/9/4.
//
#pragma once
#ifndef NATIVECPPAPP_CUBE_HPP
#define NATIVECPPAPP_CUBE_HPP

#include <GLES3/gl3.h>
#include <string.h>
#include "../common/CELLMath.hpp"
#include "../program/CubeComProgram.hpp"

class CubeCommon {
private:
    struct V3N3T2 {
        float x, y, z; //位置坐标
        float u,v; //纹理坐标
    };
public:
    CubeComProgram          sprogram;
    GLuint                  mCubeSurfaceTexId;
    V3N3T2                  _data[36];
    CELL::matrix4           mModelMatrix;


    void        init(const CELL::float3 &halfSize, GLuint tex)
    {
        V3N3T2 verts[] =
        {
                // 前
                {-halfSize.x, +halfSize.y, +halfSize.z,   0.0f,0.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z,   1.0f,0.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z,   0.0f,1.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z,   1.0f,0.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z,   1.0f,1.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z,   0.0f,1.0f},
                // 后
                {+halfSize.x, -halfSize.y, -halfSize.z,   1.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z,   1.0f,1.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z,   0.0f,0.0f},
                {-halfSize.x, +halfSize.y, -halfSize.z,   1.0f,0.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z,   0.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z,   1.0f,1.0f},
                // 左
                {-halfSize.x, +halfSize.y, +halfSize.z,   0.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z,   1.0f,1.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z,   1.0f,0.0f},
                {-halfSize.x, +halfSize.y, -halfSize.z,   0.0f,1.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z,   1.0f,1.0f},
                {-halfSize.x, +halfSize.y, +halfSize.z,   0.0f,0.0f},
                // 右
                {+halfSize.x, +halfSize.y, -halfSize.z,   0.0f,0.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z,   0.0f,1.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z,   1.0f,1.0f},
                {+halfSize.x, -halfSize.y, -halfSize.z,   1.0f,0.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z,   0.0f,0.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z,   1.0f,1.0f},
                // 上
                {-halfSize.x, +halfSize.y, +halfSize.z,   0.0f,1.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z,   1.0f,1.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z,   1.0f,0.0f},
                {-halfSize.x, +halfSize.y, -halfSize.z,   0.0f,0.0f},
                {-halfSize.x, +halfSize.y, +halfSize.z,   0.0f,1.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z,   1.0f,0.0f},
                // 下
                {+halfSize.x, -halfSize.y, -halfSize.z,   1.0f,1.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z,   1.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z,   0.0f,1.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z,   1.0f,0.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z,   0.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z,   0.0f,1.0f}
        };
        memcpy(_data, verts, sizeof(verts));
        mModelMatrix.identify();
        mCubeSurfaceTexId = tex;
    }
};
#endif //CUBE_HPP
