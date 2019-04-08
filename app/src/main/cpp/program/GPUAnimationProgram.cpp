//
// Created by nicky on 2019/4/2.
//

#include "GPUAnimationProgram.h"

#include "ShaderHelper.h"


GPUAnimationProgram::GPUAnimationProgram()
{
    const char * vertexShaderResourceStr = const_cast<char *> ("uniform mat4    u_Matrix;\n\
                                                                attribute vec4  a_Position;\n\
                                                                uniform vec3    u_AnimInfor;\n\
                                                                attribute vec2  a_uv;\n\
                                                                varying vec2    out_uv;\n\
                                                                void main()\n\
                                                                {\n\
                                                                      float uS  =  1.0/u_AnimInfor.y;\n\
                                                                      float vS  =  1.0/u_AnimInfor.x;\n\
                                                                      out_uv    =  a_uv * vec2(uS,vS);\n\
                                                                      int  row  =  int(u_AnimInfor.z)/int(u_AnimInfor.y);\n\
                                                                      float  col  =  mod((u_AnimInfor.z), (u_AnimInfor.x));\n\
                                                                      out_uv.x    +=  float(col) * uS;\n\
                                                                      out_uv.y    +=  float(row) * vS;\n\
                                                                      gl_Position = u_Matrix * a_Position;\n\
                                                                }");

    const char * fragmentShaderResourceStr= const_cast<char *>("precision mediump float;\n\
                                                                uniform sampler2D _texture;\n\
                                                                varying vec2      out_uv;\n\
                                                                void main()\n\
                                                                {\n\
                                                                   vec4 texture_color = texture2D(_texture, out_uv);\n\
                                                                   vec4 background_color = vec4(1.0, 1.0, 1.0, 1.0);\n\
                                                                   gl_FragColor = mix(background_color,texture_color, 0.9);\n\
                                                                }");

    programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);

    uMatrixLocation     = glGetUniformLocation(programId, "u_Matrix");
    uAnimInforLocation  = glGetUniformLocation(programId, "u_AnimInfor");

    aPositionLocation   = glGetAttribLocation(programId,  "a_Position");
    aTexUvLocation      = glGetAttribLocation(programId,  "a_uv");

    uTextureUnit        = glGetUniformLocation(programId, "_texture");
}


void GPUAnimationProgram::setMVPUniforms(float* matrix){
    glUniformMatrix4fv(uMatrixLocation, 1, GL_FALSE, matrix);
}

void GPUAnimationProgram::setAnimUniforms(int row,int col,int frame){
    glUniform3f(uAnimInforLocation, row, col, frame);
}


GPUAnimationProgram::~GPUAnimationProgram() {

}
