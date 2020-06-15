//
// Created by nicky on 2020/6/9.
//

#include "GpuBaseFilter.hpp"

#ifndef GPU_SHARPEN_FILTER_HPP
#define GPU_SHARPEN_FILTER_HPP


class GpuSharpenFilter : public GpuBaseFilter {
public:
    virtual int getTypeId() { return FILTER_TYPE_SHARPEN; }

    GpuSharpenFilter()
    {
        SHARPEN_VERTEX_SHADER      ="attribute vec4 position;\n\
                                    attribute vec4 inputTextureCoordinate;\n\
                                    varying vec2 textureCoordinate;\n\
                                    varying vec2 leftTextureCoordinate;\n\
                                    varying vec2 rightTextureCoordinate;\n\
                                    varying vec2 topTextureCoordinate;\n\
                                    varying vec2 bottomTextureCoordinate;\n\
                                    varying float centerMultiplier;\n\
                                    varying float edgeMultiplier;\n\
                                    void main()\n\
                                    {\n\
                                       gl_Position = position;\n\
                                       textureCoordinate = inputTextureCoordinate.xy;\n\
                                    }";
    }

    ~GpuSharpenFilter()
    {

    }


private:
    std::string SHARPEN_VERTEX_SHADER;
    std::string SHARPEN_FRAGMENT_SHADER;
};