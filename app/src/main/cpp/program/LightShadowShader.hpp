//
// Created by nicky on 2019/9/11.
//

#ifndef NATIVECPPAPP_LIGHTSHADOWSHADER_HPP
#define NATIVECPPAPP_LIGHTSHADOWSHADER_HPP

#include "ShaderProgram.h"
#include "ShaderHelper.h"
// use to debug
class LightShadowShader : public ShaderProgram {
public:
    GLint _matShadow;
    GLint _matMV;
    GLint _matPRJ;
    GLint _attrPos;
    GLint _attrNormal;
    GLint _shadowMap;
    GLint _color;
public:
    virtual void initialize() {
        const char *vs =   "attribute   vec3    attrPos;\n\
                            attribute   vec3    attrNormal;\n\
                            uniform     mat4    matPRJ;\n\
                            uniform     mat4    matMV;\n\
                            uniform     mat4    matShadow;\n\
                            varying     vec4    shadowPosition;\n\
                            void main()\n\
                            {\n\
                                    //vec4 position = matMV * vec4(attrPos, 1.0);\n\
                                    shadowPosition = matShadow * vec4(attrPos, 1.0);\n\
                                    gl_Position = matPRJ * matMV * vec4(attrPos, 1.0);\n\
                            }";

        const char *fs =   "precision mediump float;\n\
                            uniform     sampler2D   shadowMap;\n\
                            uniform     vec4        color;\n\
                            varying     vec4        shadowPosition;\n\
                            void main()\n\
                            {\n\
                                    float NdotLD     = 1.0;\n\
                                    vec4  shPosition = shadowPosition;\n\
                                    shPosition -= 0.005;\n\
                                    vec3  shadowUnProjPosition = shPosition.xyz / shPosition.w;\n\
                                    if(shadowUnProjPosition.x>=0.0 && shadowUnProjPosition.x<1.0 &&\n\
                                        shadowUnProjPosition.y>=0.0 && shadowUnProjPosition.y<1.0 &&\n\
                                        shadowUnProjPosition.z>=0.0 && shadowUnProjPosition.z<1.0 )\n\
                                    {\n\
                                        vec4 shadowDepth = texture2D(shadowMap, shadowUnProjPosition.xy);\n\
                                        if(shadowDepth.x < shadowUnProjPosition.z) \n\
                                        {\n\
                                            NdotLD = 0.5;\n\
                                        }\n\
                                    }\n\
                                    gl_FragColor = color * NdotLD;\n\
                            }";

        programId = ShaderHelper::buildProgram(vs, fs);

        _matShadow = glGetUniformLocation(programId, "matShadow");
        _matMV = glGetUniformLocation(programId, "matMV");
        _matPRJ = glGetUniformLocation(programId, "matPRJ");

        _attrPos = glGetAttribLocation(programId, "attrPos");
        _attrNormal = glGetAttribLocation(programId, "attrNormal");

        _shadowMap = glGetUniformLocation(programId, "shadowMap");
        _color = glGetUniformLocation(programId, "color");
    }


    /**
    *   使用shader
    */
    virtual void begin() {
        glEnableVertexAttribArray(_attrPos);
        glEnableVertexAttribArray(_attrNormal);
        glUseProgram(programId);
    }

    /**
    *   结束
    */
    virtual void end() {
        glDisableVertexAttribArray(_attrPos);
        glDisableVertexAttribArray(_attrNormal);
        glUseProgram(0);
    }

};

#endif //NATIVECPPAPP_LIGHTSHADOWSHADER_HPP
