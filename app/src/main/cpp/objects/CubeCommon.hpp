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
#include "../common/Camera3D.hpp"

class CubeCommon {
private:
    struct V3T2 {
        float x, y, z; //位置坐标
        float u,v; //纹理坐标
    };
public:
    CubeComProgram          sprogram;
    GLuint                  mCubeSurfaceTexId;
    V3T2                    _data[36];
    CELL::matrix4           mModelMatrix;


    void        init(const CELL::float3 &halfSize, GLuint tex)
    {
        V3T2 verts[] =
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


    void        render(Camera3D& camera)
    {
        sprogram.begin();
        CELL::matrix4   model   =   mModelMatrix;
        CELL::matrix4   vp = camera.getProject() * camera.getView();
        CELL::matrix4   mvp = (vp * model);
        glUniformMatrix4fv(sprogram._mvp, 1, GL_FALSE, mvp.data());

        //glActiveTexture(GL_TEXTURE0);
        //glEnable(GL_TEXTURE_2D);
        //glBindTexture(GL_TEXTURE_2D,  mCubeSurfaceTexId);
        //glUniform1i(sprogram._texture, 0);
        glUniform3f(sprogram._lightColor, 1.0f, 0.0f, 0.0f);

        glVertexAttribPointer(static_cast<GLuint>(sprogram._position), 3, GL_FLOAT, GL_FALSE,
                              sizeof(CubeCommon::V3T2), &_data[0].x);
        glVertexAttribPointer(static_cast<GLuint>(sprogram._uv),       2, GL_FLOAT, GL_FALSE,
                              sizeof(CubeCommon::V3T2), &_data[0].u);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        sprogram.end();
    }
};
#endif //CUBE_HPP
