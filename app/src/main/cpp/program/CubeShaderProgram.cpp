//
// Created by nicky on 2019/3/18.
//

#include "CubeShaderProgram.h"
#include "ShaderHelper.h"






CubeShaderProgram::CubeShaderProgram()
{
    char * vertexShaderResourceStr = const_cast<char *>(" uniform mat4 u_Matrix;\
                                                          attribute vec4 a_Position;\
                                                          attribute vec4 a_Color;\
                                                          varying vec4 v_Color;\
                                                          void main()\
                                                          {\
                                                                v_Color = a_Color;\
                                                                gl_Position = u_Matrix * a_Position;\
                                                                gl_PointSize = 10.0;\
                                                          }");

    char * fragmentShaderResourceStr= const_cast<char *>("precision mediump float;\
                                                          varying vec4 v_Color;\
                                                          void main()\
                                                          {\
                                                             gl_FragColor = v_Color;\
                                                          }");

    programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);

    uMatrixLocation = glGetUniformLocation(programId, "u_Matrix");
    aColorLocation = glGetAttribLocation(programId, "a_Color");
    aPositionLocation = glGetAttribLocation(programId, "a_Position");
}


void CubeShaderProgram::setUniforms(float* matrix){
    glUniformMatrix4fv(uMatrixLocation, sizeof(matrix) / sizeof(matrix[0]), GL_FALSE, matrix);
}


CubeShaderProgram::~CubeShaderProgram() {

}
