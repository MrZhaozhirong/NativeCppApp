//
// Created by nicky on 2019/3/18.
//

#include <GLES2/gl2.h>
#include <string.h>
#include "CubeIndex.h"
#include "../common/CELLMath.hpp"


CubeIndex::CubeIndex() {
    modelMatrix = new float[16];
    CELLMath::Matrix::setIdentityM(modelMatrix, 0);


    CUBE_VERTEX_DATA = new int8_t[40];
    int8_t * p = CUBE_VERTEX_DATA;
    p[0]=-1;   p[1]=1;    p[2]=1;    p[3]=0;   p[4]=0;
    p[5]=1;    p[6]=1;    p[7]= 1;   p[8]=0;   p[9]=1;
    p[10]=-1;  p[11]=-1;  p[12]= 1;  p[13]=1;  p[14]=0;
    p[15]=1;   p[16]=-1;  p[17]= 1;  p[18]=1;  p[19]=1;
    p[20]=-1;  p[21]= 1;  p[22]=-1;  p[23]=1;  p[24]=0;
    p[25]=1;   p[26]=1;   p[27]=-1;  p[28]=0;  p[29]=0;
    p[30]=-1;  p[31]=-1;  p[32]=-1;  p[33]=1;  p[34]=1;
    p[35]=1;   p[36]=-1;  p[37]=-1;  p[38]=1;  p[39]=0;
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
    //};




    CUBE_INDEX = new int8_t[24];
    CUBE_INDEX[0]=0;  CUBE_INDEX[1]=1;  CUBE_INDEX[2]=2;  CUBE_INDEX[3]=3;
    CUBE_INDEX[4]=4;  CUBE_INDEX[5]=5;  CUBE_INDEX[6]=6;  CUBE_INDEX[7]=7;
    CUBE_INDEX[8]=4;  CUBE_INDEX[9]=0;  CUBE_INDEX[10]=6; CUBE_INDEX[11]=2;
    CUBE_INDEX[12]=1; CUBE_INDEX[13]=5; CUBE_INDEX[14]=3; CUBE_INDEX[15]=7;
    CUBE_INDEX[16]=4; CUBE_INDEX[17]=5; CUBE_INDEX[18]=0; CUBE_INDEX[19]=1;
    CUBE_INDEX[20]=6; CUBE_INDEX[21]=7; CUBE_INDEX[22]=2; CUBE_INDEX[23]=3;
    //{
    //    //front
    //    0,1,2,3,
    //    //back
    //    4,5,6,7,
    //    //left
    //    4,0,6,2,
    //    //right
    //    1,5,3,7,
    //    //top
    //    4,5,0,1,
    //    //bottom
    //    6,7,2,3
    //};
}

CubeIndex::~CubeIndex() {
    delete [] CUBE_VERTEX_DATA;
    delete [] CUBE_INDEX;
    delete [] modelMatrix;
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


