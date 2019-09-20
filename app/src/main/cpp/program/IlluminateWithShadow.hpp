//
// Created by nicky on 2019/9/20.
//

#ifndef SHADER_ILLUMINATE_WITH_SHADOW_HPP
#define SHADER_ILLUMINATE_WITH_SHADOW_HPP

#endif //SHADER_ILLUMINATE_WITH_SHADOW_HPP


#version 320 es
in  vec3    position;
in  vec3    normal;
in  vec2    texCoords;

out vec2    TexCoords;
out VS_OUT {
    vec3 FragPosWorldSpace;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.TexCoords = texCoords; // 纹理uv坐标传递到fs，进行插值
    vs_out.Normal = transpose(inverse(mat3(model))) * normal;
    vs_out.FragPosWorldSpace = vec3(model * vec4(position, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPosWorldSpace, 1.0);
}







#version 320 es
uniform vec3        _lightColor;
uniform sampler2D   _texture;
uniform sampler2D   _shadowMap;

uniform vec3        _lightPos;
uniform vec3        _viewPos;

in  VS_OUT {
    vec3 FragPosWorldSpace;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out     vec4        fragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法，剔除透视效果(2D->3D)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 上面的projCoords的xyz分量都是[-1,1]，将它变换到[0,1]的范围才好和深度图比较
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float shadowDepth = texture(_shadowMap, projCoords.xy).r;
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    float shadow = currentDepth > shadowDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 color = texture(_texture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = _lightColor;
    // 周围环境
    vec3 ambient = 0.15 * color;
    // 漫反射
    vec3 lightDir = normalize(_lightPos - fs_in.FragPosWorldSpace);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    //vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = 0.0;
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    //vec3 specular = spec * lightColor;
    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color;

    fragColor = vec4(lighting, 1.0f);
}