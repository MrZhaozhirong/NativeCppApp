//
// Created by nicky on 2019/3/18.
//

#include <GLES3/gl3.h>
#include "ShaderProgram.h"
#include "../common/zzr_common.h"
#include "ShaderHelper.h"


ShaderProgram::ShaderProgram() {
    programId = 0;
}

ShaderProgram::ShaderProgram(const char *vertexShaderResourceStr, const char *fragmentShaderResourceStr) {
    programId = ShaderHelper::buildProgram(vertexShaderResourceStr, fragmentShaderResourceStr);
}

ShaderProgram::~ShaderProgram() {
    deleteProgram();
}

void ShaderProgram::userProgram() {
    //glUseProgram(programId);
    if(programId!=0){
        glUseProgram(programId);
    } else {
        LOGW("ShaderProgram is invalid.");
    }
}

int ShaderProgram::getShaderProgramId() {
    return programId;
}

void ShaderProgram::deleteProgram() {
    glDeleteProgram(programId);
    programId = 0;
}



