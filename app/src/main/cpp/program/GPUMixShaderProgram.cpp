//
// Created by nicky on 2019/4/8.
//

#include "GPUMixShaderProgram.h"
#include "ShaderHelper.h"

/**
 * 正方体动态渐变 着色器程序
 */
GPUMixShaderProgram::GPUMixShaderProgram()
{
    const char * vertexShaderResourceStr  = "uniform mat4    u_Matrix;\n\
                                             attribute vec4  a_Position;\n\
                                             attribute vec2  a_uv;\n\
                                             varying vec2    out_uv;\n\
                                             void main()\n\
                                             {\n\
                                                 out_uv      =   a_uv;\n\
                                                 gl_Position =   u_Matrix * a_Position;\n\
                                             }";
    const char * fragmentShaderResourceStr= "precision mediump float;\n\
                                             uniform sampler2D _texture0;\n\
                                             uniform sampler2D _texture1;\n\
                                             uniform float     _mix;\n\
                                             varying vec2      out_uv;\n\
                                             void main()\n\
                                             {\n\
                                                 vec4 color0    =  texture2D(_texture0, out_uv);\n\
                                                 vec4 color1    =  texture2D(_texture1, out_uv);\n\
                                                 vec4 dstColor  =  color0 * (1.0 - _mix)  + color1 * _mix;\n\
                                                 gl_FragColor   =  mix(color0, color1, _mix);\n\
                                             }";

    //                                           gl_FragColor   =   dstColor;
    programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);

    uMatrixLocation     = glGetUniformLocation(programId, "u_Matrix");
    aPositionLocation   = glGetAttribLocation(programId, "a_Position");
    uMixLocation        = glGetUniformLocation(programId, "_mix");
    aTexUvLocation      = glGetAttribLocation(programId, "a_uv");
    uTextureUnit0       = glGetUniformLocation(programId, "_texture0");
    uTextureUnit1       = glGetUniformLocation(programId, "_texture1");
}

void GPUMixShaderProgram::setMVPUniforms(float* matrix){
    glUniformMatrix4fv(uMatrixLocation, 1, GL_FALSE, matrix);
}

void GPUMixShaderProgram::setMixUniform(double mix){
    glUniform1f(uMixLocation, static_cast<GLfloat>(mix));
}


GPUMixShaderProgram::~GPUMixShaderProgram() {

}