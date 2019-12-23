//
// Created by nicky on 2019/12/23.
//

#ifndef GPU_COLOR_INVERT_FILTER_HPP
#define GPU_COLOR_INVERT_FILTER_HPP

#include "GpuBaseFilter.hpp"
/**
 * 反转图像中的所有颜色。
 */
class GpuColorInvertFilter : public GpuBaseFilter {
public:
    int getTypeId() { return FILTER_TYPE_COLOR_INVERT; }

    GpuColorInvertFilter()
    {
        COLOR_INVERT_FRAGMENT_SHADER="precision mediump float;\n\
                                    varying highp vec2 textureCoordinate;\n\
                                    uniform sampler2D SamplerRGB;\n\
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
                                       vec4 textureColor = vec4(yuv2rgb(textureCoordinate), 1.0);\n\
                                       gl_FragColor = vec4((1.0 - textureColor.rgb), textureColor.w);\n\
                                    }";
    }
    ~GpuColorInvertFilter() {
        if(!COLOR_INVERT_FRAGMENT_SHADER.empty()) COLOR_INVERT_FRAGMENT_SHADER.clear();
    }

    void init() {
        GpuBaseFilter::init(NO_FILTER_VERTEX_SHADER.c_str(), COLOR_INVERT_FRAGMENT_SHADER.c_str());
    }

private:
    std::string COLOR_INVERT_FRAGMENT_SHADER;
};
#endif //GPU_COLOR_INVERT_FILTER_HPP
