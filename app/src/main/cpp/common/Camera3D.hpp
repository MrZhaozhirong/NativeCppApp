//
// Created by zzr on 2019/4/11.
//
#pragma once
#ifndef NATIVECPPAPP_CAMERA3D_HPP
#define NATIVECPPAPP_CAMERA3D_HPP


#include "CELLMath.hpp"
using namespace CELL;

class Camera3D
{
public:
    real3       _eye;
    real3       _up;
    real3       _right;
    real3       _target;
    real3       _dir;

    Camera3D(   const real3& target = real3(0,0,0),
                const real3& eye = real3(0,10,10),
                const real3& right = real3(1,0,0)   )
    {
        _target     =   target;
        _eye        =   eye;
        _right      =   right;
        _dir        =   normalize(_target - _eye);
    }
};

#endif //NATIVECPPAPP_CAMERA3D_HPP
