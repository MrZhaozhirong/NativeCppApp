//
// Created by nicky on 2019/3/18.
//

#include "CubeShaderProgram.h"
#include "ShaderHelper.h"





CubeShaderProgram::CubeShaderProgram()
{
    const char * vertexShaderResourceStr = const_cast<char *>(" uniform mat4    u_Matrix;\n\
                                                                attribute vec4  a_Position;\n\
                                                                attribute vec2  a_uv;\n\
                                                                varying vec2    out_uv;\n\
                                                                void main()\n\
                                                                {\n\
                                                                      out_uv = a_uv;\n\
                                                                      gl_Position = u_Matrix * a_Position;\n\
                                                                }");

    const char * fragmentShaderResourceStr= const_cast<char *>("precision mediump float;\n\
                                                                uniform sampler2D _texture;\n\
                                                                varying vec2      out_uv;\n\
                                                                void main()\n\
                                                                {\n\
                                                                   gl_FragColor = texture2D(_texture, out_uv);\n\
                                                                }");

    programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);

    uMatrixLocation     = glGetUniformLocation(programId, "u_Matrix");
    aPositionLocation   = glGetAttribLocation(programId, "a_Position");
    aTexUvLocation      = glGetAttribLocation(programId, "a_uv");
    uTextureUnit        = glGetUniformLocation(programId, "_texture");
}


void CubeShaderProgram::setMVPUniforms(float* matrix){
    glUniformMatrix4fv(uMatrixLocation, 1, GL_FALSE, matrix);
}


CubeShaderProgram::~CubeShaderProgram() {

}
