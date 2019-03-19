//
// Created by nicky on 2019/3/18.
//

#include <GLES2/gl2.h>
#include "CubeIndex.h"

CubeIndex::POSITION_COMPONENT_COUNT = 3;
CubeIndex::COLOR_COMPONENT_COUNT = 3;
CubeIndex::STRIDE = 3 + 3 ;

CubeIndex::CubeIndex() {
    CUBE_VERTEX_DATA = new char[]{
            //x,   y,    z     R,  G,  B
            -1,   1,   1,   1, 0, 0, // 0 left top near
            1,   1,   1,    1, 0, 1, // 1 right top near
            -1,  -1,   1,   0, 0, 1, // 2 left bottom near
            1,  -1,   1,    0, 1, 0, // 3 right bottom near
            -1,   1,  -1,   0, 1, 0, // 4 left top ar
            1,   1,  -1,    0, 0, 1, // 5 right top far
            -1,  -1,  -1,   1, 0, 1, // 6 left bottom far
            1,  -1,  -1,    1, 0, 0, // 7 right bottom far
    };
    CUBE_INDEX = new char[]{
            //front
            1, 0, 2,
            1, 2, 3,
            //back
            5, 4, 6,
            5, 6, 7,
            //left
            4, 0, 2,
            4, 2, 6,
            //right
            5, 1, 3,
            5, 3, 7,
            //top
            5, 4, 0,
            5, 0, 1,
            //bottom
            7, 6, 2,
            7, 2, 3
    };
}

CubeIndex::~CubeIndex() {
    delete [] CUBE_VERTEX_DATA;
    delete [] CUBE_INDEX;
}

void CubeIndex::bindData(CubeShaderProgram* shaderProgram) {
    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aPositionLocation),
                          POSITION_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          CUBE_VERTEX_DATA);

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aColorLocation),
                          COLOR_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          &CUBE_VERTEX_DATA[POSITION_COMPONENT_COUNT]);
}

void CubeIndex::draw() {
    // 正方体 六个面，每个面两个三角形，每个三角形三个点
    glDrawElements(GL_TRIANGLES, 6*2*3, GL_UNSIGNED_BYTE, CUBE_INDEX );
}


