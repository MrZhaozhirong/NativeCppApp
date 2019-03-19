//
// Created by nicky on 2019/3/18.
//

#include <GLES2/gl2.h>
#include "CubeIndex.h"



CubeIndex::CubeIndex() {
    CUBE_VERTEX_DATA = new short[48];
    short * p = CUBE_VERTEX_DATA;
    p[0]=-1;  p[1]=1;    p[2]=1;    p[3]=1;   p[4]=0;   p[5]=0;
    p[6]=1;   p[7]=1;    p[8]= 1;   p[9]=1;   p[10]=0;  p[11]=1;
    p[12]=-1; p[13]=-1;  p[14]= 1;  p[15]=0;  p[16]=0;  p[17]=1;
    p[18]=1;  p[19]=-1;  p[20]= 1;  p[21]=0;  p[22]=1;  p[23]=0;
    p[24]=-1; p[25]= 1;  p[26]=-1;  p[27]=0;  p[28]=1;  p[29]=0;
    p[30]=1;  p[31]=1;   p[32]=-1;  p[33]=0;  p[34]=0;  p[35]=1;
    p[36]=-1; p[37]=-1;  p[38]=-1;  p[39]=1;  p[40]=0;  p[41]=1;
    p[42]=1;  p[43]=-1;  p[44]=-1;  p[45]=1;  p[46]=0;  p[47]=0;
    //{
    //        //x,   y,    z     R,  G,  B
    //        -1,   1,   1,   1, 0, 0, // 0 left top near
    //        1,   1,   1,    1, 0, 1, // 1 right top near
    //        -1,  -1,   1,   0, 0, 1, // 2 left bottom near
    //        1,  -1,   1,    0, 1, 0, // 3 right bottom near
    //        -1,   1,  -1,   0, 1, 0, // 4 left top ar
    //        1,   1,  -1,    0, 0, 1, // 5 right top far
    //        -1,  -1,  -1,   1, 0, 1, // 6 left bottom far
    //        1,  -1,  -1,    1, 0, 0  // 7 right bottom far
    //};
    CUBE_INDEX = new short[36];
    CUBE_INDEX[0]=1; CUBE_INDEX[0]=0; CUBE_INDEX[0]=2;
    CUBE_INDEX[0]=1; CUBE_INDEX[0]=2; CUBE_INDEX[0]=3;
    CUBE_INDEX[0]=5; CUBE_INDEX[0]=4; CUBE_INDEX[0]=6;
    CUBE_INDEX[0]=5; CUBE_INDEX[0]=6; CUBE_INDEX[0]=7;
    CUBE_INDEX[0]=4; CUBE_INDEX[0]=0; CUBE_INDEX[0]=2;
    CUBE_INDEX[0]=4; CUBE_INDEX[0]=2; CUBE_INDEX[0]=6;
    CUBE_INDEX[0]=5; CUBE_INDEX[0]=1; CUBE_INDEX[0]=3;
    CUBE_INDEX[0]=5; CUBE_INDEX[0]=3; CUBE_INDEX[0]=7;
    CUBE_INDEX[0]=5; CUBE_INDEX[0]=4; CUBE_INDEX[0]=0;
    CUBE_INDEX[0]=5; CUBE_INDEX[0]=0; CUBE_INDEX[0]=1;
    CUBE_INDEX[0]=7; CUBE_INDEX[0]=6; CUBE_INDEX[0]=2;
    CUBE_INDEX[0]=7; CUBE_INDEX[0]=2; CUBE_INDEX[0]=3;
    //{
    //        //front
    //        1, 0, 2,
    //        1, 2, 3,
    //        //back
    //        5, 4, 6,
    //        5, 6, 7,
    //        //left
    //        4, 0, 2,
    //        4, 2, 6,
    //        //right
    //        5, 1, 3,
    //        5, 3, 7,
    //        //top
    //        5, 4, 0,
    //        5, 0, 1,
    //        //bottom
    //        7, 6, 2,
    //        7, 2, 3
    //};
}

CubeIndex::~CubeIndex() {
    delete [] CUBE_VERTEX_DATA;
    delete [] CUBE_INDEX;
}

void CubeIndex::bindData(CubeShaderProgram* shaderProgram) {
    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aPositionLocation),
                          POSITION_COMPONENT_COUNT, GL_SHORT,
                          GL_FALSE, STRIDE,
                          CUBE_VERTEX_DATA);

    glVertexAttribPointer(static_cast<GLuint>(shaderProgram->aColorLocation),
                          COLOR_COMPONENT_COUNT, GL_SHORT,
                          GL_FALSE, STRIDE,
                          &CUBE_VERTEX_DATA[POSITION_COMPONENT_COUNT]);
}

void CubeIndex::draw() {
    // 正方体 六个面，每个面两个三角形，每个三角形三个点
    glDrawElements(GL_TRIANGLES, 6*2*3, GL_SHORT, CUBE_INDEX );
}


