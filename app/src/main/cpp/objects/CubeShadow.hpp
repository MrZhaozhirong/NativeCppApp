//
// Created by nicky on 2019/9/27.
//

#ifndef NATIVECPPAPP_CUBESHADOW_HPP
#define NATIVECPPAPP_CUBESHADOW_HPP

#include "../program/IlluminateWithShadow.hpp"
#include "../common/CELLMath.hpp"
#include "../common/Camera3D.hpp"

class CubeShadow {
public:
    struct V3N3T2 {
        float x, y, z; //位置坐标
        float nx, ny, nz; //法向量
        float u,v; //纹理坐标
    };

public:
    IlluminateWithShadow    sprogram;
    GLuint                  mCubeSurfaceTexId;
    GLuint                  mShadowMapId;
    V3N3T2                  _data[36];
    CELL::matrix4           mModelMatrix;

    void        setShadowMap(GLuint texId) {
        mShadowMapId = texId;
    }
    void        setSurfaceTexture(GLuint texId) {
        mCubeSurfaceTexId = texId;
    }

    void        init(const CELL::float3 &halfSize)
    {
        V3N3T2 verts[] =
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
        memcpy(_data, verts, sizeof(verts));
        mModelMatrix.identify();

        sprogram.initialize();
    }



    void        render(matrix4 currentProjectionMatrix, matrix4 currentViewMatrix,
                       real3& lightPos,
                       matrix4 lightProjectionMatrix, matrix4 lightViewMatrix)
    {
        sprogram.begin();
        //CELL::matrix4   model   =   mModelMatrix;
        //CELL::matrix4   vp = camera.getProject() * camera.getView();
        //CELL::matrix4   mvp = (vp * model);
        glUniformMatrix4fv(sprogram._projection, 1, GL_FALSE, currentProjectionMatrix.data());
        glUniformMatrix4fv(sprogram._view, 1, GL_FALSE, currentViewMatrix.data());
        glUniformMatrix4fv(sprogram._model, 1, GL_FALSE, mModelMatrix.data());

        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,  mCubeSurfaceTexId);
        glUniform1i(sprogram._texture, 0);

        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,  mShadowMapId);
        glUniform1i(sprogram._shadowMap, 1);

        glUniform3f(sprogram._lightColor, 1.0f, 1.0f, 1.0f);
        glUniform3f(sprogram._lightPos, lightPos.x, lightPos.y, lightPos.z);
        matrix4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
        glUniformMatrix4fv(sprogram._lightSpaceMatrix, 1, GL_FALSE, lightSpaceMatrix.data());

        glVertexAttribPointer(static_cast<GLuint>(sprogram._position), 3, GL_FLOAT, GL_FALSE,
                              sizeof(CubeShadow::V3N3T2), &_data[0].x);
        glVertexAttribPointer(static_cast<GLuint>(sprogram._normal),   3, GL_FLOAT, GL_FALSE,
                              sizeof(CubeShadow::V3N3T2), &_data[0].nx);
        glVertexAttribPointer(static_cast<GLuint>(sprogram._uv),       2, GL_FLOAT, GL_FALSE,
                              sizeof(CubeShadow::V3N3T2), &_data[0].u);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        sprogram.end();
    }
};
#endif //NATIVECPPAPP_CUBESHADOW_HPP