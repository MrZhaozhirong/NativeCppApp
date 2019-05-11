#ifndef NATIVECPPAPP_SHADERHELPER_H
#define NATIVECPPAPP_SHADERHELPER_H


#include <GLES3/gl3.h>
#include "../common/zzr_common.h"

class ShaderHelper {

public:
    static GLuint buildProgram(const char * vertexShaderSource, const char * fragmentShaderSource);

    static GLuint compileVertexShader(const char * shaderCode);
    static GLuint compileFragmentShader(const char * shaderCode);
    static GLuint linkProgram(GLuint vertexShaderId, GLuint fragmentShaderId);
};


#endif