//
// Created by nicky on 2019/7/11.
//

#ifndef NATIVECPPAPP_CUBETBN_HPP
#define NATIVECPPAPP_CUBETBN_HPP


#include <GLES3/gl3.h>
#include "../common/CELLMath.hpp"

class CubeTBN {
public:
    struct V3N3UV2 {
        float x, y, z; //位置坐标
        float nx, ny, nz; //法向量
        float u,v; //纹理坐标
    };

    struct V3N3UV2TBN6
    {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
        float tx,ty,tz;
        float bx,by,bz;
    };

public:
    V3N3UV2TBN6 _data[36];
    GLuint      _texMaterial;
    GLuint      _texNormal;

    CubeTBN(const CELL::float3& halfSize) {
        init(halfSize);
    }

    void        init(const CELL::float3 &halfSize)
    {
        V3N3UV2 verts[] =
        {
                // 前
                {-halfSize.x, +halfSize.y, +halfSize.z, 0.0f,  0.0f,  +1.0f, 0.0f,0.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z, 0.0f,  0.0f,  +1.0f, 1.0f,0.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z, 0.0f,  0.0f,  +1.0f, 0.0f,1.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z, 0.0f,  0.0f,  +1.0f, 1.0f,0.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z, 0.0f,  0.0f,  +1.0f, 1.0f,1.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z, 0.0f,  0.0f,  +1.0f, 0.0f,1.0f},
                // 后
                {+halfSize.x, -halfSize.y, -halfSize.z, 0.0f,  0.0f,  -1.0f, 1.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z, 0.0f,  0.0f,  -1.0f, 1.0f,1.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z, 0.0f,  0.0f,  -1.0f, 0.0f,0.0f},
                {-halfSize.x, +halfSize.y, -halfSize.z, 0.0f,  0.0f,  -1.0f, 1.0f,0.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z, 0.0f,  0.0f,  -1.0f, 0.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z, 0.0f,  0.0f,  -1.0f, 1.0f,1.0f},
                // 左
                {-halfSize.x, +halfSize.y, +halfSize.z, -1.0f, 0.0f,  0.0f,  0.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z, -1.0f, 0.0f,  0.0f,  1.0f,1.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z, -1.0f, 0.0f,  0.0f,  1.0f,0.0f},
                {-halfSize.x, +halfSize.y, -halfSize.z, -1.0f, 0.0f,  0.0f,  0.0f,1.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z, -1.0f, 0.0f,  0.0f,  1.0f,1.0f},
                {-halfSize.x, +halfSize.y, +halfSize.z, -1.0f, 0.0f,  0.0f,  0.0f,0.0f},
                // 右
                {+halfSize.x, +halfSize.y, -halfSize.z, +1.0f, 0.0f,  0.0f,  0.0f,0.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z, +1.0f, 0.0f,  0.0f,  0.0f,1.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z, +1.0f, 0.0f,  0.0f,  1.0f,1.0f},
                {+halfSize.x, -halfSize.y, -halfSize.z, +1.0f, 0.0f,  0.0f,  1.0f,0.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z, +1.0f, 0.0f,  0.0f,  0.0f,0.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z, +1.0f, 0.0f,  0.0f,  1.0f,1.0f},
                // 上
                {-halfSize.x, +halfSize.y, +halfSize.z, 0.0f,  +1.0f, 0.0f,  0.0f,1.0f},
                {+halfSize.x, +halfSize.y, +halfSize.z, 0.0f,  +1.0f, 0.0f,  1.0f,1.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z, 0.0f,  +1.0f, 0.0f,  1.0f,0.0f},
                {-halfSize.x, +halfSize.y, -halfSize.z, 0.0f,  +1.0f, 0.0f,  0.0f,0.0f},
                {-halfSize.x, +halfSize.y, +halfSize.z, 0.0f,  +1.0f, 0.0f,  0.0f,1.0f},
                {+halfSize.x, +halfSize.y, -halfSize.z, 0.0f,  +1.0f, 0.0f,  1.0f,0.0f},
                // 下
                {+halfSize.x, -halfSize.y, -halfSize.z, 0.0f,  -1.0f, 0.0f,  1.0f,1.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z, 0.0f,  -1.0f, 0.0f,  1.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z, 0.0f,  -1.0f, 0.0f,  0.0f,1.0f},
                {+halfSize.x, -halfSize.y, +halfSize.z, 0.0f,  -1.0f, 0.0f,  1.0f,0.0f},
                {-halfSize.x, -halfSize.y, +halfSize.z, 0.0f,  -1.0f, 0.0f,  0.0f,0.0f},
                {-halfSize.x, -halfSize.y, -halfSize.z, 0.0f,  -1.0f, 0.0f,  0.0f,1.0f}
        };
        // 根据位置/纹理 -> TBN
        convertTBN(verts, _data);
    }

    void setMaterialTexID(GLuint material_texID) {
        _texMaterial = material_texID;
    }
    void setMormalTexID(GLuint normal_texID) {
        _texNormal = normal_texID;
    }


private:
    void convertTBN(V3N3UV2* vertices,V3N3UV2TBN6* nmVerts)
    {
        for (size_t i = 0; i <36; i += 3) // 一次操作一个三角形的三个点
        {
            // copy xyz normal uv
            nmVerts[i + 0].x  = vertices[i + 0].x;
            nmVerts[i + 0].y  = vertices[i + 0].y;
            nmVerts[i + 0].z  = vertices[i + 0].z;
            nmVerts[i + 0].nx = vertices[i + 0].nx;
            nmVerts[i + 0].ny = vertices[i + 0].ny;
            nmVerts[i + 0].nz = vertices[i + 0].nz;
            nmVerts[i + 0].u  = vertices[i + 0].u;
            nmVerts[i + 0].v  = vertices[i + 0].v;

            nmVerts[i + 1].x  = vertices[i + 1].x;
            nmVerts[i + 1].y  = vertices[i + 1].y;
            nmVerts[i + 1].z  = vertices[i + 1].z;
            nmVerts[i + 1].nx = vertices[i + 1].nx;
            nmVerts[i + 1].ny = vertices[i + 1].ny;
            nmVerts[i + 1].nz = vertices[i + 1].nz;
            nmVerts[i + 1].u  = vertices[i + 1].u;
            nmVerts[i + 1].v  = vertices[i + 1].v;

            nmVerts[i + 2].x  = vertices[i + 2].x;
            nmVerts[i + 2].y  = vertices[i + 2].y;
            nmVerts[i + 2].z  = vertices[i + 2].z;
            nmVerts[i + 2].nx = vertices[i + 2].nx;
            nmVerts[i + 2].ny = vertices[i + 2].ny;
            nmVerts[i + 2].nz = vertices[i + 2].nz;
            nmVerts[i + 2].u  = vertices[i + 2].u;
            nmVerts[i + 2].v  = vertices[i + 2].v;

            // Shortcuts for vertices
            CELL::float3  v0  = CELL::float3(vertices[i + 0].x,vertices[i + 0].y,vertices[i + 0].z);
            CELL::float3  v1  = CELL::float3(vertices[i + 1].x,vertices[i + 1].y,vertices[i + 1].z);
            CELL::float3  v2  = CELL::float3(vertices[i + 2].x,vertices[i + 2].y,vertices[i + 2].z);
            CELL::float2  uv0 = CELL::float2(vertices[i + 0].u, vertices[i + 0].v);
            CELL::float2  uv1 = CELL::float2(vertices[i + 1].u, vertices[i + 1].v);
            CELL::float2  uv2 = CELL::float2(vertices[i + 2].u, vertices[i + 2].v);
            // 构建triangle平面的方向向量 (position delta, δ)
            CELL::float3  deltaPos1 = v1 - v0;
            CELL::float3  deltaPos2 = v2 - v0;
            // 构建UV平面的两个方向的向量 (uv delta, δ)
            CELL::float2 deltaUV1   = uv1 - uv0;
            CELL::float2 deltaUV2   = uv2 - uv0;

            float   r               = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x); // uv叉积作底
            CELL::float3 tangent    = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r; // 得出切线
            CELL::float3 binormal   = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r; // 得出副切线

            nmVerts[i + 0].tx = tangent.x;  nmVerts[i + 0].bx = binormal.x;
            nmVerts[i + 0].ty = tangent.y;  nmVerts[i + 0].by = binormal.y;
            nmVerts[i + 0].tz = tangent.z;  nmVerts[i + 0].bz = binormal.z;

            nmVerts[i + 1].tx = tangent.x;  nmVerts[i + 1].bx = binormal.x;
            nmVerts[i + 1].ty = tangent.y;  nmVerts[i + 1].by = binormal.y;
            nmVerts[i + 1].tz = tangent.z;  nmVerts[i + 1].bz = binormal.z;

            nmVerts[i + 2].tx = tangent.x;  nmVerts[i + 2].bx = binormal.x;
            nmVerts[i + 2].ty = tangent.y;  nmVerts[i + 2].by = binormal.y;
            nmVerts[i + 2].tz = tangent.z;  nmVerts[i + 2].bz = binormal.z;
        }
    }
};


#endif //NATIVECPPAPP_CUBETBN_HPP
