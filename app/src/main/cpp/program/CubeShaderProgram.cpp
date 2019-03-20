//
// Created by nicky on 2019/3/18.
//

#include "CubeShaderProgram.h"
#include "ShaderHelper.h"






CubeShaderProgram::CubeShaderProgram()
{
    const char * vertexShaderResourceStr = const_cast<char *>(" uniform mat4 u_Matrix;\n\
                                                                attribute vec4 a_Position;\n\
                                                                attribute vec4 a_Color;\n\
                                                                varying vec4 v_Color;\n\
                                                                void main()\n\
                                                                {\n\
                                                                      v_Color = a_Color;\n\
                                                                      gl_Position = u_Matrix * a_Position;\n\
                                                                      gl_PointSize = 10.0;\n\
                                                                }");

    const char * fragmentShaderResourceStr= const_cast<char *>("precision mediump float;\n\
                                                                varying vec4 v_Color;\n\
                                                                void main()\n\
                                                                {\n\
                                                                   gl_FragColor = v_Color;\n\
                                                                }");

    programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);

    uMatrixLocation = glGetUniformLocation(programId, "u_Matrix");
    aColorLocation = glGetAttribLocation(programId, "a_Color");
    aPositionLocation = glGetAttribLocation(programId, "a_Position");
}


void CubeShaderProgram::setUniforms(float* matrix){
    glUniformMatrix4fv(uMatrixLocation, 1, GL_FALSE, matrix);
}


CubeShaderProgram::~CubeShaderProgram() {

}
