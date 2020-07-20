//
// Created by zzr on 2020/7/15.
//

#ifndef GPU_BOXBLUR_FILTER_HPP
#define GPU_BOXBLUR_FILTER_HPP

#include "GpuBaseFilter.hpp"

class GpuBoxBlurFilter : public GpuBaseFilter {
public:
    virtual int getTypeId() { return FILTER_TYPE_BOXBLUR; }

    GpuBoxBlurFilter()
    {
        BOXBLUR_VERTEX_SHADER     ="\n\
                            attribute vec4 position;\n\
                            attribute vec4 inputTextureCoordinate;\n\
                            uniform float widthFactor;\n\
                            uniform float heightFactor;\n\
                            \n\
                            varying vec2 textureCoordinate;\n\
                            varying vec2 leftTextureCoordinate;\n\
                            varying vec2 rightTextureCoordinate;\n\
                            \n\
                            varying vec2 topTextureCoordinate;\n\
                            varying vec2 topLeftTextureCoordinate;\n\
                            varying vec2 topRightTextureCoordinate;\n\
                            \n\
                            varying vec2 bottomTextureCoordinate;\n\
                            varying vec2 bottomLeftTextureCoordinate;\n\
                            varying vec2 bottomRightTextureCoordinate;\n\
                            void main()\n\
                            {\n\
                                gl_Position = position;\n\
                                vec2 widthStep = vec2(widthFactor, 0.0);\n\
                                vec2 heightStep = vec2(0.0, heightFactor);\n\
                                textureCoordinate = inputTextureCoordinate.xy;\n\
                                leftTextureCoordinate = inputTextureCoordinate.xy - widthStep;\n\
                                rightTextureCoordinate = inputTextureCoordinate.xy + widthStep;\n\
                                \n\
                                topTextureCoordinate = inputTextureCoordinate.xy - heightStep;\n\
                                topLeftTextureCoordinate = inputTextureCoordinate.xy - heightStep - widthStep;\n\
                                topRightTextureCoordinate = inputTextureCoordinate.xy - heightStep + widthStep;\n\
                                \n\
                                bottomTextureCoordinate = inputTextureCoordinate.xy + heightStep;\n\
                                bottomLeftTextureCoordinate = inputTextureCoordinate.xy + heightStep - widthStep;\n\
                                bottomRightTextureCoordinate = inputTextureCoordinate.xy + heightStep + widthStep;\n\
                             }";


        BOXBLUR_FRAGMENT_SHADER   ="\n\
                            precision highp float;\n\
                            uniform mediump mat3 convolutionMatrix;\n\
                            varying vec2 textureCoordinate;\n\
                            varying vec2 leftTextureCoordinate;\n\
                            varying vec2 rightTextureCoordinate;\n\
                            varying vec2 topTextureCoordinate;\n\
                            varying vec2 topLeftTextureCoordinate;\n\
                            varying vec2 topRightTextureCoordinate;\n\
                            varying vec2 bottomTextureCoordinate;\n\
                            varying vec2 bottomLeftTextureCoordinate;\n\
                            varying vec2 bottomRightTextureCoordinate;\n\
                            \n\
                            uniform sampler2D SamplerY;\n\
                            uniform sampler2D SamplerU;\n\
                            uniform sampler2D SamplerV;\n\
                            mat3 colorConversionMatrix = mat3(\n\
                                               1.0, 1.0, 1.0,\n\
                                               0.0, -0.39465, 2.03211,\n\
                                               1.13983, -0.58060, 0.0);\n\
                            vec3 yuv2rgb(vec2 pos)\n\
                            {\n\
                               vec3 yuv;\n\
                               yuv.x = texture2D(SamplerY, pos).r;\n\
                               yuv.y = texture2D(SamplerU, pos).r - 0.5;\n\
                               yuv.z = texture2D(SamplerV, pos).r - 0.5;\n\
                               return colorConversionMatrix * yuv;\n\
                            }\n\
                            void main()\n\
                            {\n\
                                mediump vec3 textureColor = yuv2rgb(textureCoordinate);\n\
                                mediump vec3 leftTextureColor = yuv2rgb(leftTextureCoordinate);\n\
                                mediump vec3 rightTextureColor = yuv2rgb(rightTextureCoordinate);\n\
                                mediump vec3 topTextureColor = yuv2rgb(topTextureCoordinate);\n\
                                mediump vec3 topLeftTextureColor = yuv2rgb(topLeftTextureCoordinate);\n\
                                mediump vec3 topRightTextureColor = yuv2rgb(topRightTextureCoordinate);\n\
                                mediump vec3 bottomTextureColor = yuv2rgb(bottomTextureCoordinate);\n\
                                mediump vec3 bottomLeftTextureColor = yuv2rgb(bottomLeftTextureCoordinate);\n\
                                mediump vec3 bottomRightTextureColor = yuv2rgb(bottomRightTextureCoordinate);\n\
                                vec3 fragmentColor = (leftTextureColor + textureColor + rightTextureColor);\n\
                                fragmentColor += (topLeftTextureColor + topTextureColor + topRightTextureColor);\n\
                                fragmentColor += (bottomLeftTextureColor + bottomTextureColor + bottomRightTextureColor);\n\
                                gl_FragColor = vec4(fragmentColor/9, 1.0);\n\
                            }";
    }

    ~GpuBoxBlurFilter() {
        if(!BOXBLUR_VERTEX_SHADER.empty()) BOXBLUR_VERTEX_SHADER.clear();
        if(!BOXBLUR_FRAGMENT_SHADER.empty()) BOXBLUR_FRAGMENT_SHADER.clear();
    }


    void init() {
        GpuBaseFilter::init(BOXBLUR_VERTEX_SHADER.c_str(), BOXBLUR_FRAGMENT_SHADER.c_str());
        mWidthFactorLocation = glGetUniformLocation(mGLProgId, "widthFactor");
        mHeightFactorLocation = glGetUniformLocation(mGLProgId, "heightFactor");
    }
private:
    std::string BOXBLUR_VERTEX_SHADER;
    std::string BOXBLUR_FRAGMENT_SHADER;

    GLint mWidthFactorLocation;
    GLint mHeightFactorLocation;
};
#endif //GPU_BOXBLUR_FILTER_HPP
