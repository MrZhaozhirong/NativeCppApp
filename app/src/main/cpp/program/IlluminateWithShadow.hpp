//
// Created by nicky on 2019/9/20.
//

#include "ShaderProgram.h"
#include "ShaderHelper.h"

#ifndef SHADER_ILLUMINATE_WITH_SHADOW_HPP
#define SHADER_ILLUMINATE_WITH_SHADOW_HPP

class IlluminateWithShadow : public ShaderProgram
{
public:
    GLint       _position;
    GLint       _normal;
    GLint       _uv;

    GLint       _projection;
    GLint       _view;
    GLint       _model;
    GLint       _lightSpaceMatrix;

    GLint       _lightColor;
    GLint       _texture;
    GLint       _shadowMap;
    GLint       _lightPos;
    GLint       _viewPos;// 需要镜面发射才使用
public:
    virtual void    initialize()
    {
        const char* vs  =   "#version 320 es\n"
                            "in  vec3    position;\n"
                            "in  vec3    normal;\n"
                            "in  vec2    uv;\n"
                            "\n"
                            "out VS_OUT {\n"
                            "    vec3 FragPosWorldSpace;\n"
                            "    vec3 Normal;\n"
                            "    vec2 TexCoords;\n"
                            "    vec4 FragPosLightSpace;\n"
                            "} vs_out;\n"
                            "\n"
                            "uniform mat4 projection;\n"
                            "uniform mat4 view;\n"
                            "uniform mat4 model;\n"
                            "uniform mat4 lightSpaceMatrix;\n"
                            "\n"
                            "void main()\n"
                            "{\n"
                            "    gl_Position = projection * view * model * vec4(position, 1.0f);\n"
                            "    vs_out.TexCoords = uv; // 纹理uv坐标传递到fs，进行插值\n"
                            "    vs_out.Normal = transpose(inverse(mat3(model))) * normal;\n"
                            "    //vs_out.Normal = inverse(mat3(model)) * normal;\n"
                            "    vs_out.FragPosWorldSpace = vec3(model * vec4(position, 1.0));\n"
                            "    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPosWorldSpace, 1.0);\n"
                            "}";

        const char* fs  =   "#version 320 es\n"
                            "precision mediump float;\n"
                            "uniform vec3        _lightColor;\n"
                            "uniform sampler2D   _texture;\n"
                            "uniform sampler2D   _shadowMap;\n"
                            "uniform vec3        _lightPos;\n"
                            "uniform vec3        _viewPos;\n"
                            "\n"
                            "in  VS_OUT {\n"
                            "    vec3 FragPosWorldSpace;\n"
                            "    vec3 Normal;\n"
                            "    vec2 TexCoords;\n"
                            "    vec4 FragPosLightSpace;\n"
                            "} fs_in;\n"
                            "\n"
                            "out     vec4        fragColor;\n"
                            "\n"
                            "float ShadowCalculation(vec4 fragPosLightSpace,float bias)\n"
                            "{\n"
                            "    // 执行透视除法，剔除透视效果(2D->3D)\n"
                            "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;\n"
                            "    // 上面的projCoords的xyz分量都是[-1,1]，将它变换到[0,1]的范围才好和深度图比较\n"
                            "    projCoords = projCoords * 0.5 + 0.5;\n"
                            "    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)\n"
                            "    float shadowDepth = texture(_shadowMap, projCoords.xy).r;\n"
                            "    // 取得当前片元在光源视角下的深度\n"
                            "    float currentDepth = projCoords.z;\n"
                            "    // 检查当前片元是否在阴影中\n"
                            "    float shadow = currentDepth-bias > shadowDepth ? 1.0 : 0.0;\n"
                            "    if(projCoords.z > 1.0) shadow = 0.0;\n"
                            "    return shadow;\n"
                            "}\n"
                            "void main()\n"
                            "{\n"
                            "    vec3 color = texture(_texture, fs_in.TexCoords).rgb;\n"
                            "    vec3 normal = normalize(fs_in.Normal);\n"
                            "    vec3 lightColor = _lightColor;\n"
                            "    // 周围环境\n"
                            "    vec3 ambient = 0.5 * color;\n"
                            "    // 漫反射\n"
                            "    vec3 lightDir = normalize(_lightPos - fs_in.FragPosWorldSpace);\n"
                            "    float diff = max(dot(lightDir, normal), 0.0);\n"
                            "    vec3 diffuse = diff * lightColor;\n"
                            "    // 镜面发射\n"
                            "    //#vec3 viewDir = normalize(viewPos - fs_in.FragPos);\n"
                            "    //#vec3 reflectDir = reflect(-lightDir, normal);\n"
                            "    //#float spec = 0.0;\n"
                            "    //#vec3 halfwayDir = normalize(lightDir + viewDir);\n"
                            "    //#spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);\n"
                            "    //#vec3 specular = spec * lightColor;\n"
                            "    // 阴影失真\n"
                            "    //float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);\n"
                            "    // 计算阴影\n"
                            "    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, 0.0005);\n"
                            "    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;\n"
                            "    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color;\n"
                            "    fragColor = vec4(lighting, 1.0f);\n"
                            "}";

        programId   =   ShaderHelper::buildProgram(vs, fs);

        _position   =   glGetAttribLocation(programId,  "position");
        _normal     =   glGetAttribLocation(programId,  "normal");
        _uv         =   glGetAttribLocation(programId,  "uv");

        _projection =   glGetUniformLocation(programId, "projection");
        _view       =   glGetUniformLocation(programId, "view");
        _model      =   glGetUniformLocation(programId, "model");
        _lightSpaceMatrix = glGetUniformLocation(programId, "lightSpaceMatrix");

        _lightColor =   glGetUniformLocation(programId, "_lightColor");
        _texture    =   glGetUniformLocation(programId, "_texture");
        _shadowMap  =   glGetUniformLocation(programId, "_shadowMap");
        _lightPos   =   glGetUniformLocation(programId, "_lightPos");
    }

    /**
    *   使用shader
    */
    virtual void    begin()
    {
        glEnableVertexAttribArray(_position);
        glEnableVertexAttribArray(_normal);
        glEnableVertexAttribArray(_uv);
        glUseProgram(programId);
    }
    /**
    *   结束
    */
    virtual void    end()
    {
        glDisableVertexAttribArray(_position);
        glDisableVertexAttribArray(_normal);
        glDisableVertexAttribArray(_uv);
        glUseProgram(0);
    }
};
#endif //SHADER_ILLUMINATE_WITH_SHADOW_HPP



