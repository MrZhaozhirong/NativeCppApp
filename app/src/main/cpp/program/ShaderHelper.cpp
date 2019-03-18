
#include "ShaderHelper.h"
#include <stdlib.h>

bool validateProgram(GLuint programObjectId) {
    glValidateProgram(programObjectId);
    GLint validateStatus = GL_FALSE;
    glGetProgramiv(programObjectId, GL_VALIDATE_STATUS, &validateStatus);
    LOGI("Result of validating program : %d",validateStatus);
    return validateStatus!=GL_FALSE;
}

bool checkGlError(const char* funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}

GLuint compileShader(GLenum type, const char * shaderCode) {
    GLuint shaderObjectId = glCreateShader(type);
    if (shaderObjectId == 0){
        LOGW(" Warning! Could not create new shader, glGetError:"+glGetError());
        return 0;
    }
    glShaderSource(shaderObjectId,  1, &shaderCode, NULL);

    glCompileShader(shaderObjectId);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shaderObjectId, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shaderObjectId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*) malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {
                glGetShaderInfoLog(shaderObjectId, infoLogLen, NULL, infoLog);
                LOGE("Could not compile %s shader:\n%s\n",
                     type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                      infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shaderObjectId);
        return 0;
    }
    return shaderObjectId;
}

GLuint ShaderHelper::compileVertexShader(const char *shaderCode) {
    return compileShader(GL_VERTEX_SHADER, shaderCode);
}

GLuint ShaderHelper::compileFragmentShader(const char *shaderCode) {
    return compileShader(GL_FRAGMENT_SHADER, shaderCode);
}

GLuint ShaderHelper::buildProgram(const char *vertexShaderSource, const char *fragmentShaderSource) {

    GLuint programObjectId = 0;
    GLuint vertexShader = compileVertexShader(vertexShaderSource);
    GLuint fragmentShader = compileFragmentShader(fragmentShaderSource);

    programObjectId = linkProgram(vertexShader,fragmentShader);

    validateProgram(programObjectId);

    return programObjectId;
}

GLuint ShaderHelper::linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId) {

    GLuint programObjectId = glCreateProgram();
    if (!programObjectId) {
        checkGlError("glCreateProgram");
        goto exit;
    }

    glAttachShader(programObjectId, vertexShaderId);
    glAttachShader(programObjectId, fragmentShaderId);

    GLint linked = GL_FALSE;
    glLinkProgram(programObjectId);
    glGetProgramiv(programObjectId, GL_LINK_STATUS, &linked);
    if (!linked) {
        LOGE("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(programObjectId, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(static_cast<size_t>(infoLogLen));
            if (infoLog) {
                glGetProgramInfoLog(programObjectId, infoLogLen, NULL, infoLog);
                LOGE("Could not linkProgram reason:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(programObjectId);
        programObjectId = 0;
    }

exit:
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return programObjectId;
}

