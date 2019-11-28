//
// Created by nicky on 2019/11/28.
//

#include "GpuFilterRender.h"

GpuFilterRender::GpuFilterRender()
{

}

GpuFilterRender::~GpuFilterRender()
{

}

void GpuFilterRender::surfaceCreated(ANativeWindow *window)
{

}

void GpuFilterRender::surfaceChanged(int width, int height)
{
    this->mViewHeight = height;
    this->mViewWidth = width;
}

void GpuFilterRender::surfaceDestroyed()
{

}

void GpuFilterRender::renderOnDraw(double elpasedInMilliSec)
{

}
