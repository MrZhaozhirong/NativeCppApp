//
// Created by nicky on 2019/4/8.
//

#include "GPUFlatSlidingProgram.h"
#include "ShaderHelper.h"

GPUFlatSlidingProgram::GPUFlatSlidingProgram()
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
                                             uniform float     offset;\n\
                                             varying vec2      out_uv;\n\
                                             void main()\n\
                                             {\n\
                                                 vec4 color0 = vec4(0,0,0,1);\n\
                                                 if(out_uv.x <= offset )\n\
                                                    color0 = texture2D(_texture1, vec2(out_uv.x + (1.0 - offset), out_uv.y));\n\
                                                 else\n\
                                                    color0 = texture2D(_texture0, vec2(out_uv.x - offset, out_uv.y));\n\
                                                 gl_FragColor   =  color0; \n\
                                             }";

    programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);

    uMatrixLocation     = glGetUniformLocation(programId, "u_Matrix");
    aPositionLocation   = glGetAttribLocation(programId,  "a_Position");
    aTexUvLocation      = glGetAttribLocation(programId,  "a_uv");
    uOffset             = glGetUniformLocation(programId, "offset");
    uTextureUnit0       = glGetUniformLocation(programId, "_texture0");
    uTextureUnit1       = glGetUniformLocation(programId, "_texture1");
}

void GPUFlatSlidingProgram::setMVPUniforms(float* matrix){
    glUniformMatrix4fv(uMatrixLocation, 1, GL_FALSE, matrix);
}

void GPUFlatSlidingProgram::setOffsetUniform(double offset){
    glUniform1f(uOffset, static_cast<GLfloat>(offset));
}



GPUFlatSlidingProgram::~GPUFlatSlidingProgram() {

}