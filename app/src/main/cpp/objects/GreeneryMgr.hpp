//
// Created by nicky on 2019/4/30.
//
#pragma once
#ifndef GREENERY_MGR_HPP
#define GREENERY_MGR_HPP


#include <GLES3/gl3.h>
#include "../common/CELLMath.hpp"
#include "../common/Camera3D.hpp"
#include "../common/constructormagic.h"
#include "../program/GreeneryShaderProgram.hpp"
#include <vector>

//using namespace CELL;
//using namespace std;

class GreeneryMgr {

private:
    std::vector<CELL::float3>   mGrassesPos;
    GLuint                      mTexGrasses;
    GLuint                      vbo;
    GreeneryShaderProgram       _program;

public:
    GreeneryMgr() {}
    ~GreeneryMgr() {}

    /**
     *	初始化
     */
    void    init(GLuint tex)
    {
        mTexGrasses   =   tex;
        // 初始化shader
        _program.initialize();

        //  每一棵小草堆的位置索引
        for (float x = -3 ; x < 6 ; x += 3)
        {
            for (float z = -3 ; z < 6 ; z += 3)
            {
                if(x==0&&z==0) continue;
                mGrassesPos.push_back(CELL::float3(x,-1,z));
            }
        }
        // 把草堆实例的位置索引寄存到vbo
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mGrassesPos.size()*sizeof(CELL::float3)),
                     &mGrassesPos.front(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    void    render(Camera3D& camera)
    {
        CELL::matrix4   MVP     =   camera._matProj * camera._matView;

        _program.begin();
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D,  mTexGrasses );
            glUniform1i(_program._texture, 0);

            glUniform3f(_program._rightDir,
                        static_cast<GLfloat>(camera._right.x),
                        static_cast<GLfloat>(camera._right.y),
                        static_cast<GLfloat>(camera._right.z));
            glUniform3f(_program._upDir, 0,1,0);
            //glUniform3f(_program._upDir,
            //            static_cast<GLfloat>(camera._up.x),
            //            static_cast<GLfloat>(camera._up.y),
            //            static_cast<GLfloat>(camera._up.z));

            glUniformMatrix4fv(_program._mvp, 1, GL_FALSE, MVP.data());

            /// 这个将vbo点数据传递给shader
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glVertexAttribPointer(_program._position, 3, GL_FLOAT, GL_FALSE, sizeof(CELL::float3), 0);
            /// 启用_position顶点属性的多实例特性
            glVertexAttribDivisor(_program._position, 1);
            /// 绘制函数,完成绘制
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mGrassesPos.size());
            /// 禁用_position顶点属性的多实例特性
            glVertexAttribDivisor(_program._position, 0);
            /// 解绑vbo
            glBindBuffer(GL_ARRAY_BUFFER,0);
        }
        _program.end();
    }




private:
    DISALLOW_EVIL_CONSTRUCTORS(GreeneryMgr);
};


#endif // GREENERY_MGR_HPP
