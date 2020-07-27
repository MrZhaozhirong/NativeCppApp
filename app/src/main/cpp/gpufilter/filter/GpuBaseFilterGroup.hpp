//
// Created by zzr on 2020/7/25.
//

#ifndef GPU_FILTER_GROUP_HPP
#define GPU_FILTER_GROUP_HPP

#include <list>
#include "GpuBaseFilter.hpp"
// STL容器介绍
// https://www.cnblogs.com/linuxAndMcu/p/10254542.html
class GpuBaseFilterGroup  {

public:
    void addFilter(GpuBaseFilter filter) {
        mFilterList.push_back(filter);
    }

private:
    std::list<GpuBaseFilter> mFilterList;
};
#endif // GPU_FILTER_GROUP_HPP
