//
// Created by nicky on 2019/9/30.
//

#ifndef NATIVECPPAPP_LANDSHADOW_HPP
#define NATIVECPPAPP_LANDSHADOW_HPP

#include <GLES3/gl3.h>
#include "../common/CELLMath.hpp"
#include "../program/IlluminateWithShadow.hpp"
#include "../common/Camera3D.hpp"

class LandShadow {
public:
    struct V3N3T2 {
        float x, y, z; //位置坐标
        float nx, ny, nz; //法向量
        float u,v; //纹理坐标
    };

public:
    V3N3T2                  _data[6];
    CELL::matrix4           _modelMatrix;
    GLuint                  _texId;
    GLuint                  _ShadowMapId;
    IlluminateWithShadow    sprogram;

    void        setShadowMap(GLuint texId) {
        _ShadowMapId = texId;
    }
    void        setSurfaceTexture(GLuint texId) {
        _texId = texId;
    }

    void        init(const float size, const float y_pos)
    {
        float   gSizeX = 10;
        float   gSizeZ = 10;

        V3N3T2 verts[] =
        {
            {-gSizeX, y_pos, -gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
            { gSizeX, y_pos, -gSizeZ, 0,1,0, size, 0.0f}, // right far
            { gSizeX, y_pos,  gSizeZ, 0,1,0, size, size}, // right near
            {-gSizeX, y_pos, -gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
            { gSizeX, y_pos,  gSizeZ, 0,1,0, size, size}, // right near
            {-gSizeX, y_pos,  gSizeZ, 0,1,0, 0.0f, size}  // left near
        };
        memcpy(_data, verts, sizeof(verts));

        _modelMatrix.identify();

        sprogram.initialize();
    }


    void        render(Camera3D& camera, real3& lightPos, matrix4 lightSpaceMatrix)
    {
        sprogram.userProgram();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texId);
        glUniform1i(sprogram._texture, 0);
        glActiveTexture(GL_TEXTURE1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,  _ShadowMapId);
        glUniform1i(sprogram._shadowMap, 1);

        //CELL::matrix4   model = _modelMatrix;
        //CELL::matrix4   vp = camera.getProject() * camera.getView();
        //CELL::matrix4   mvp = (vp * model);
        glUniformMatrix4fv(sprogram._projection, 1, GL_FALSE,
                           reinterpret_cast<const GLfloat *>(camera.getProject().data()));
        glUniformMatrix4fv(sprogram._view, 1, GL_FALSE,
                           reinterpret_cast<const GLfloat *>(camera.getView().data()));
        glUniformMatrix4fv(sprogram._model, 1, GL_FALSE, _modelMatrix.data());

        glUniform3f(sprogram._lightColor, 1.0f, 1.0f, 1.0f);
        glUniform3f(sprogram._lightPos, lightPos.x, lightPos.y, lightPos.z);
        glUniformMatrix4fv(sprogram._lightSpaceMatrix, 1, GL_FALSE, lightSpaceMatrix.data());

        glVertexAttribPointer(static_cast<GLuint>(sprogram._position), 3, GL_FLOAT, GL_FALSE,
                              sizeof(LandShadow::V3N3T2), &_data[0].x);
        glVertexAttribPointer(static_cast<GLuint>(sprogram._normal),   3, GL_FLOAT, GL_FALSE,
                              sizeof(LandShadow::V3N3T2), &_data[0].nx);
        glVertexAttribPointer(static_cast<GLuint>(sprogram._texCoords),2, GL_FLOAT, GL_FALSE,
                              sizeof(LandShadow::V3N3T2), &_data[0].u);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        sprogram.end();
    }


};

#endif //NATIVECPPAPP_LANDSHADOW_HPP
