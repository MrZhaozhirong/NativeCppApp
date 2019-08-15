//
// Created by nicky on 2019/8/15.
//

#ifndef PIPICTURE_HPP
#define PIPICTURE_HPP

#include <GLES3/gl3.h>
#include "../program/DepthOrthoShader.hpp"
#include "../common/CELLMath.hpp"

class PIPicture {
public:
    struct V3UV2 {
        float x, y, z;
        float u,v;
    };

public:
    V3UV2               _data[6];
    GLuint              _texId;
    DepthOrthoShader    _depthOrthoShader;
    float               _width;
    float               _height;


    void        init(const float width, const float height)
    {
        V3UV2  rect[] =
        {
                {0,       0,        0.0f,   0.0f,  1.0f},
                {width/4, 0,        0.0f,   1.0f,  1.0f},
                {0,       height/4, 0.0f,   0.0f,  0.0f},

                {width/4, 0,        0.0f,   1.0f,  1.0f},
                {width/4, height/4, 0.0f,   1.0f,  0.0f},
                {0,       height/4, 0.0f,   0.0f,  0.0f},
        };
        memcpy(_data, rect, sizeof(rect));

        _depthOrthoShader.initialize();

        _width = width;
        _height = height;
    }

    void        setTextureId(GLuint texId)
    {
        _texId = texId;
    }

    void        render()
    {
        _depthOrthoShader.begin();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _texId);
        CELL::matrix4   orth    =   CELL::ortho<float>(0,_width,_height,0, 100,-100);
        glUniform1i(_depthOrthoShader._tex, 0);
        glUniformMatrix4fv(_depthOrthoShader._mvp, 1, GL_FALSE, orth.data());
        glVertexAttribPointer(_depthOrthoShader._position,    3, GL_FLOAT, GL_FALSE, sizeof(V3UV2), _data);
        glVertexAttribPointer(_depthOrthoShader._uv,          2, GL_FLOAT, GL_FALSE, sizeof(V3UV2), &_data[0].u);
        glDrawArrays(GL_TRIANGLES,0,6);
        _depthOrthoShader.end();
    }
};

#endif //PIPICTURE_HPP
