//
// Created by zzr on 2019/8/11.
//

#ifndef LAND_HPP
#define LAND_HPP

#include <GLES3/gl3.h>
#include "../common/CELLMath.hpp"
#include "../program/GrasslandProgram.hpp"
#include "../common/Camera3D.hpp"

class Land {
public:
    struct V3N3UV2 {
        float x, y, z; //位置坐标
        float nx, ny, nz; //法向量
        float u,v; //纹理坐标
    };

public:
    V3N3UV2             _data[6];
    CELL::matrix4       _modelMatrix;
    GLuint              _texId;
    GrasslandProgram*   _sprogram;

    void        init(const float size, const float y_pos, GLuint tex)
    {
        float   gSizeX = 10;
        float   gSizeZ = 10;

        V3N3UV2 verts[] =
        {
            {-gSizeX, y_pos, -gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
            { gSizeX, y_pos, -gSizeZ, 0,1,0, size, 0.0f}, // right far
            { gSizeX, y_pos,  gSizeZ, 0,1,0, size, size}, // right near
            {-gSizeX, y_pos, -gSizeZ, 0,1,0, 0.0f, 0.0f}, // left far
            { gSizeX, y_pos,  gSizeZ, 0,1,0, size, size}, // right near
            {-gSizeX, y_pos,  gSizeZ, 0,1,0, 0.0f, size}  // left near
        };
        memcpy(_data, verts, sizeof(verts));
        _texId = tex;
        _modelMatrix.identify();

        if( _sprogram != nullptr) {
            delete _sprogram;
        }
        _sprogram = new GrasslandProgram();
    }

    void        render(Camera3D& camera)
    {
        _sprogram->ShaderProgram::userProgram();

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texId);
        glUniform1i(_sprogram->uTextureUnit, 0);

        CELL::matrix4   vp =   camera.getProject() * camera.getView();
        CELL::matrix4 land_mvp = vp * _modelMatrix;
        glUniformMatrix4fv(_sprogram->uMVPMatrixLocation, 1, GL_FALSE, land_mvp.data());

        glEnableVertexAttribArray(static_cast<GLuint>(_sprogram->aPositionLocation));
        glEnableVertexAttribArray(static_cast<GLuint>(_sprogram->aNormalLocation));
        glEnableVertexAttribArray(static_cast<GLuint>(_sprogram->aTexUvLocation));

        glVertexAttribPointer(static_cast<GLuint>(_sprogram->aPositionLocation),
                              3, GL_FLOAT, GL_FALSE, sizeof(V3N3UV2), &_data[0].x);
        glVertexAttribPointer(static_cast<GLuint>(_sprogram->aNormalLocation),
                              3, GL_FLOAT, GL_FALSE, sizeof(V3N3UV2), &_data[0].nx);
        glVertexAttribPointer(static_cast<GLuint>(_sprogram->aTexUvLocation),
                              2, GL_FLOAT, GL_FALSE, sizeof(V3N3UV2), &_data[0].u);

        glDrawArrays(GL_TRIANGLES, 0, 2*3);

        glDisableVertexAttribArray(static_cast<GLuint>(_sprogram->aPositionLocation));
        glDisableVertexAttribArray(static_cast<GLuint>(_sprogram->aNormalLocation));
        glDisableVertexAttribArray(static_cast<GLuint>(_sprogram->aTexUvLocation));

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }


    void        destroy()
    {
        if( _sprogram!= NULL) {
            delete _sprogram;
            _sprogram = NULL;
        }
    }
};
#endif //LAND_HPP
