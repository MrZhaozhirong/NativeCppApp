//
// Created by nicky on 2019/3/18.
//

#include <GLES2/gl2.h>
#include <string.h>
#include "CubeIndex.h"
#include "../common/CELLMath.hpp"
#include "../program/GPUAnimationProgram.h"


CubeIndex::CubeIndex() {
    modelMatrix = new float[16];
    CELL::Matrix::setIdentityM(modelMatrix, 0);

    // CubeShaderProgram
    // GPUAnimationProgram 共用
    CUBE_VERTEX_DATA = new int8_t[60];
    int8_t * p = CUBE_VERTEX_DATA;
    p[0]=-1;   p[1]=1;    p[2]=1;    p[3]=0;   p[4]=0;
    p[5]=1;    p[6]=1;    p[7]= 1;   p[8]=1;   p[9]=0;
    p[10]=-1;  p[11]=-1;  p[12]= 1;  p[13]=0;  p[14]=1;
    p[15]=1;   p[16]=-1;  p[17]= 1;  p[18]=1;  p[19]=1;
    p[20]=-1;  p[21]= 1;  p[22]=-1;  p[23]=1;  p[24]=0;
    p[25]=1;   p[26]=1;   p[27]=-1;  p[28]=0;  p[29]=0;
    p[30]=-1;  p[31]=-1;  p[32]=-1;  p[33]=1;  p[34]=1;
    p[35]=1;   p[36]=-1;  p[37]=-1;  p[38]=0;  p[39]=1;

    p[40]=-1;  p[41]= 1;  p[42]=-1;  p[43]=0;  p[44]=0;
    p[45]=1;   p[46]=1;   p[47]=-1;  p[48]=1;  p[49]=0;
    p[50]=-1;  p[51]=1;   p[52]=1;   p[53]=0;  p[54]=1;
    p[55]=1;   p[56]=1;   p[57]= 1;  p[58]=1;  p[59]=1;
    //{
    //        //x,   y,  z    s, t,
    //        -1,   1,   1,   0, 0,  // 0 left top near
    //        1,   1,   1,    0, 1,  // 1 right top near
    //        -1,  -1,   1,   1, 0,  // 2 left bottom near
    //        1,  -1,   1,    1, 1,  // 3 right bottom near
    //        -1,   1,  -1,   1, 0,  // 4 left top far
    //        1,   1,  -1,    0, 0,  // 5 right top far
    //        -1,  -1,  -1,   1, 1,  // 6 left bottom far
    //        1,  -1,  -1,    1, 0,  // 7 right bottom far
    //        这样安排的纹理坐标点，四周是正常的，但是顶底是不正常，
    //        所以顶底要重新安排一组
    //        -1,   1,  -1,   0, 0,  // 8  left top far
    //        1,   1,  -1,    1, 0,  // 9  right top far
    //        -1,   1,   1,   0, 1,  // 10 left top near
    //        1,   1,   1,    1, 1,  // 11 right top near
    //};

    CUBE_INDEX = new int8_t[24];
    CUBE_INDEX[0 ]= 8;  CUBE_INDEX[1 ]= 9;  CUBE_INDEX[2 ]=10;  CUBE_INDEX[3 ]=11;
    CUBE_INDEX[4 ]= 6;  CUBE_INDEX[5 ]= 7;  CUBE_INDEX[6 ]=2;   CUBE_INDEX[7 ]=3;
    CUBE_INDEX[8 ]= 0;  CUBE_INDEX[9 ]= 1;  CUBE_INDEX[10]=2;   CUBE_INDEX[11]=3;
    CUBE_INDEX[12]= 4;  CUBE_INDEX[13]= 5;  CUBE_INDEX[14]=6;   CUBE_INDEX[15]=7;
    CUBE_INDEX[16]= 4;  CUBE_INDEX[17]= 0;  CUBE_INDEX[18]=6;   CUBE_INDEX[19]=2;
    CUBE_INDEX[20]= 1;  CUBE_INDEX[21]= 5;  CUBE_INDEX[22]=3;   CUBE_INDEX[23]=7;
    //{
    //    //top
    //    8,9,10,11,
    //    //bottom
    //    6,7,2,3
    //    //front
    //    0,1,2,3,
    //    //back
    //    4,5,6,7,
    //    //left
    //    4,0,6,2,
    //    //right
    //    1,5,3,7,
    //};
}

CubeIndex::~CubeIndex() {
    delete [] CUBE_VERTEX_DATA;
    delete [] CUBE_INDEX;
    delete [] modelMatrix;
}

void CubeIndex::bindData(GPUFlatSlidingProgram *shaderProgram) {
    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aPositionLocation),
                          POSITION_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          CUBE_VERTEX_DATA);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aPositionLocation));

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aTexUvLocation),
                          TEXTURE_COORDINATE_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          &CUBE_VERTEX_DATA[POSITION_COMPONENT_COUNT]);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aTexUvLocation));
}

void CubeIndex::bindData(GPUMixShaderProgram* shaderProgram) {
    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aPositionLocation),
                          POSITION_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          CUBE_VERTEX_DATA);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aPositionLocation));

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aTexUvLocation),
                          TEXTURE_COORDINATE_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          &CUBE_VERTEX_DATA[POSITION_COMPONENT_COUNT]);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aTexUvLocation));
}

void CubeIndex::bindData(GPUAnimationProgram* shaderProgram) {
    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aPositionLocation),
                          POSITION_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          CUBE_VERTEX_DATA);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aPositionLocation));

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aTexUvLocation),
                          TEXTURE_COORDINATE_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          &CUBE_VERTEX_DATA[POSITION_COMPONENT_COUNT]);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aTexUvLocation));
}

void CubeIndex::bindData(CubeShaderProgram* shaderProgram) {
    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aPositionLocation),
                          POSITION_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          CUBE_VERTEX_DATA);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aPositionLocation));

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aTexUvLocation),
                          TEXTURE_COORDINATE_COMPONENT_COUNT, GL_BYTE,
                          GL_FALSE, STRIDE,
                          &CUBE_VERTEX_DATA[POSITION_COMPONENT_COUNT]);
    glEnableVertexAttribArray(static_cast<GLuint>(shaderProgram->aTexUvLocation));
}

void CubeIndex::draw() {
    // 正方体 六个面，每个面两个三角形，每个三角形三个点
    //glDrawElements(GL_TRIANGLES, 6*2*3, GL_UNSIGNED_BYTE, CUBE_INDEX );
    // 正方体 六个面，每个面四个点
    glDrawElements(GL_TRIANGLE_STRIP, 6*4, GL_UNSIGNED_BYTE, CUBE_INDEX );
}




