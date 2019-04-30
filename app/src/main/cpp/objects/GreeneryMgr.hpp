//
// Created by nicky on 2019/4/30.
//

#ifndef GREENERYMGR_HPP
#define GREENERYMGR_HPP

#include <GLES2/gl2.h>
#include "../common/CELLMath.hpp"
#include "../common/Camera3D.hpp"
#include "../common/constructormagic.h"

using namespace CELL;
using namespace std;

class GreeneryMgr {

public:
    std::vector<CELL::float3>   mGrassesPos;
    GLuint                      mTexGrasses;

private:
    GLuint                      vbo;

public:
    /**
     *	初始化
     */
    void    init(GLuint tex)
    {
        mTexGrasses   =   tex;
        // 初始化shader
        //_program.initialize();

        //  草堆数据的布局
        for (float x = 0 ; x < 6 ; x += 2)
        {
            for (float z = 0 ; z < 6 ; z += 2)
            {
                mGrassesPos.push_back(float3(x,0,z));
            }
        }

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mGrassesPos.size()*sizeof(float3)),
                     &mGrassesPos.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void    render(Camera3D& camera)
    {

    }




private:
    DISALLOW_EVIL_CONSTRUCTORS(GreeneryMgr);
};


#endif // GREENERYMGR_HPP
