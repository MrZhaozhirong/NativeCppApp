//
// Created by zzr on 2020/7/25.
//

#ifndef GPU_FILTER_GROUP_HPP
#define GPU_FILTER_GROUP_HPP

#include <list>
#include <vector>
#include "GpuBaseFilter.hpp"
// STL容器介绍
// https://www.cnblogs.com/linuxAndMcu/p/10254542.html
class GpuBaseFilterGroup : public GpuBaseFilter {
    // GpuBaseFilter virtual method
public:
    virtual void onOutputSizeChanged(int width, int height) {
        if (mFilterList.empty()) return;

        destroyFrameBufferObjs();

        std::vector<GpuBaseFilter>::iterator itr;
        for(itr=mFilterList.begin(); itr!=mFilterList.end(); itr++)
        {
            GpuBaseFilter filter = *itr;
            filter.onOutputSizeChanged(width, height);
        }

        createFrameBufferObjs(width, height);
    }

    virtual void destroy() {
        destroyFrameBufferObjs();

        std::vector<GpuBaseFilter>::iterator itr;
        for(itr=mFilterList.begin(); itr!=mFilterList.end(); itr++)
        {
            GpuBaseFilter filter = *itr;
            filter.destroy();
        }
        mFilterList.clear();

        GpuBaseFilter::destroy();
    }
private:

    void createFrameBufferObjs(int _width, int _height ) {
        const int num = mFilterList.size() -1;
        // 最后一次draw是在显示屏幕上
        mFBO_IDs = new GLuint[num];
        mFBO_TextureIDs = new GLuint[num];
        glGenFramebuffers(num, mFBO_IDs);
        glGenTextures(num, mFBO_TextureIDs);

        for (int i = 0; i < num; i++) {
            glBindTexture(GL_TEXTURE_2D, mFBO_TextureIDs[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_REPEAT
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, mFBO_IDs[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFBO_TextureIDs[i], 0);

            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    void destroyFrameBufferObjs() {
        if (mFBO_TextureIDs != NULL) {
            glDeleteTextures(length(mFBO_TextureIDs), mFBO_TextureIDs);
            delete[] mFBO_TextureIDs;
            mFBO_TextureIDs = NULL;
        }
        if (mFBO_IDs != NULL) {
            glDeleteFramebuffers(length(mFBO_IDs), mFBO_IDs);
            delete[] mFBO_IDs;
            mFBO_IDs = NULL;
        }
    }


    inline int length(GLuint arr[]) {
        return sizeof(arr) / sizeof(arr[0]);
    }



public:
    std::vector<GpuBaseFilter> mFilterList;
    void addFilter(GpuBaseFilter filter) {
        mFilterList.push_back(filter);
    }

    GLuint* mFBO_IDs;
    GLuint* mFBO_TextureIDs;
};
#endif // GPU_FILTER_GROUP_HPP
