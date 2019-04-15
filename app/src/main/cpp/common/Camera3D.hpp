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
    real3       _eye;   //视点
    real3       _target;//观察点
    real3       _dir;   //观察向量：视点->观察点
    real3       _up;    //观察向量竖直向量
    real3       _right; //观察向量右向向量

    matrix4r    _matView;       //视图矩阵
    matrix4r    _matProj;       //投影矩阵
    matrix4r    _matWorld;      //世界坐标

    real2       _ViewPortSize;  //视口大小
    real3       _oldLength;

    Camera3D(   const real3& target = real3(0,0,0),
                const real3& eye = real3(0,10,10),
                const real3& right = real3(1,0,0)   )
    {
        _target     =   target;
        _eye        =   eye;
        _right      =   right;
        _up         =   normalize(cross(_right, _dir));
        _dir        =   normalize(_target - _eye);
        // 初始化单位矩阵
        _matView    =   CELL::matrix4r(1);
        _matProj    =   CELL::matrix4r(1);
        _matWorld   =   CELL::matrix4r(1);
    }

    ~Camera3D() {}

    // 计算当前摄像头 与 观察目标的方向
    void    calcDir()
    {
        _dir = normalize(_target - _eye);
    }

    real3   getEye() const
    {
        return _eye;
    }
    void    setEye(CELL::real3 val)
    {
        _eye    =   val;
    }
    real3   getTarget() const
    {
        return _target;
    }
    void    setTarget(CELL::real3 val)
    {
        _target = val;
    }
    void    setRight(CELL::real3 val)
    {
        _right  =   val;
    }
    real3   getRight() const
    {
        return  _right;
    }
    real3   getUp() const
    {
        return _up;
    }
    void    setUp(CELL::real3 val)
    {
        _up = val;
    }
    void    setViewPort(const real2& viewSize)
    {
        _ViewPortSize   =   viewSize;
    }
    void    setViewPort(real x,real y)
    {
        _ViewPortSize   =   real2(x,y);
    }
    real2   getViewPort()
    {
        return  _ViewPortSize;
    }

    void            setProject(const matrix4r& proj)
    {
        _matProj    =   proj;
    }
    const matrix4r& getProject() const
    {
        return  _matProj;
    }
    // 正交投影
    void    ortho( real left, real right, real bottom, real top, real zNear, real zFar )
    {
        _matProj    =   CELL::ortho(left,right,bottom,top,zNear,zFar);
    }
    // 透视投影
    void    perspective(real fovy, real aspect, real zNear, real zFar)
    {
        _matProj    =   CELL::perspective<real>(fovy,aspect,zNear,zFar);
    }




    /**
    *   世界坐标转化为窗口坐标
    */
    real2  worldToScreen( const real3& world)
    {
        real4  worlds(world.x,world.y,world.z,1);
        real4  screens;
        project(worlds,screens);
        return  real2(screens.x,screens.y);
    }
    bool    project( const real4& world, real4& screen )
    {
        screen  =   (_matProj * _matView * _matWorld) * world;
        if (screen.w == 0.0f)
        {
            return false;
        }
        screen.x    /=  screen.w;
        screen.y    /=  screen.w;
        screen.z    /=  screen.w;
        // map to range 0 - 1
        screen.x    =   screen.x * 0.5f + 0.5f;
        screen.y    =   screen.y * 0.5f + 0.5f;
        screen.z    =   screen.z * 0.5f + 0.5f;
        // map to viewport
        screen.x    =   screen.x * _ViewPortSize.x;
        screen.y    =   _ViewPortSize.y - (screen.y * _ViewPortSize.y);
        return  true;
    }


    /**
    *   窗口坐标转化为世界坐标
    */
    real3  screenToWorld(const real2& screen)
    {
        real4  screens(screen.x,screen.y,0,1);
        real4  world;
        unProject(screens,world);
        return  real3(world.x,world.y,world.z);
    }
    real3  screenToWorld(real x,real y)
    {
        real4  screens(x,y,0,1);
        real4  world;
        unProject(screens,world);
        return  real3(world.x,world.y,world.z);
    }
    bool    unProject( const real4& screen, real4& world )
    {
        real4 v;
        v.x =   screen.x;
        v.y =   screen.y;
        v.z =   screen.z;
        v.w =   1.0;
        // map from viewport to 0 - 1
        v.x =   (v.x) /_ViewPortSize.x;
        v.y =   (_ViewPortSize.y - v.y) /_ViewPortSize.y;
        //v.y = (v.y - _viewPort.Y) / _viewPort.Height;
        // map to range -1 to 1
        v.x =   v.x * 2.0f - 1.0f;
        v.y =   v.y * 2.0f - 1.0f;
        v.z =   v.z * 2.0f - 1.0f;

        CELL::matrix4r  inverse = (_matProj * _matView * _matWorld).inverse();

        v   =   v * inverse;
        if (v.w == 0.0f)
        {
            return false;
        }
        world   =   v / v.w;
        return true;
    }

    /**
    *   下面的函数的功能是将摄像机的观察方向绕某个方向轴旋转一定的角度
    *   改变观察者的位置，目标的位置不变化
    */
    virtual void    rotateViewY(real angle)
    {
        real        len(0);
        matrix4r    mat(1);
        mat.rotate(angle, real3(0, 1, 0));
        _dir        =   _dir * mat;
        _up         =   _up * mat;
        _right      =   CELL::normalize(cross(_dir, _up));

        len         =   CELL::length(_eye - _target);
        _eye        =   _target - _dir * len;

        _matView    =   CELL::lookAt<real>(_eye, _target, _up);
    }
    virtual void    rotateViewX(real angle)
    {
        real        len(0);
        matrix4r    mat(1);
        mat.rotate(angle, _right);
        _dir        =   _dir * mat;
        _up         =   _up * mat;
        _right      =   CELL::normalize(cross(_dir,_up));
        len         =   CELL::length(_eye - _target);
        _eye        =   _target - _dir * len;
        _matView    =   CELL::lookAt(_eye,_target,_up);
    }

    virtual void    rotateViewXByCenter(real angle,real3  pos)
    {
        //! 计算眼睛到鼠标点的方向
        real3  vDir    =   pos - _eye;
        /// 计算旋转点和眼睛之间的距离
        real   len1    =   length(vDir);
        /// 旋转点和眼睛直接的方向
        vDir    =   normalize(vDir);
        real   len     =   0;

        matrix4r mat(1);
        mat.rotate(angle, _right);

        vDir    =   vDir * mat;
        /// 推倒出眼睛的位置
        _eye    =   pos - vDir * len1;

        _dir    =   _dir * mat;
        _up     =   _up * mat;
        _right  =   CELL::normalize(CELL::cross(_dir, _up));
        len     =   CELL::length(_eye - _target);
        /// 推导出观察中心点的位置
        _target     =   _eye + _dir * len;
        _matView    =   CELL::lookAt<real>(_eye, _target, _up);
    }

    virtual void    rotateViewYByCenter(real angle,real3  pos)
    {
        real        len(0);
        real        len1(0);
        matrix4r    mat(1);
        mat.rotate(angle, real3(0, 1, 0));

        real3   vDir = pos - _eye;

        len1    =   CELL::length(vDir);
        vDir    =   CELL::normalize(vDir);
        vDir    =   vDir * mat;
        _eye    =   pos - vDir * len1;

        _dir    =   _dir * mat;
        _up     =   _up * mat;
        _right  =   CELL::normalize(cross(_dir, _up));
        len     =   CELL::length(_eye - _target);
        _target =   _eye + _dir * len;
        _matView=   CELL::lookAt<real>(_eye, _target, _up);
    }

    virtual void    rotateViewZByCenter(real angle,real3  pos)
    {
        real        len(0);
        real        len1(0);
        matrix4r    mat(1);
        mat.rotate(angle, real3(0, 0, 1));

        real3   vDir = pos - _eye;

        len1    =   CELL::length(vDir);
        vDir    =   CELL::normalize(vDir);
        vDir    =   vDir * mat;
        _eye    =   pos - vDir * len1;

        _dir    =   _dir * mat;
        _up     =   _up * mat;
        _right  =   CELL::normalize(cross(_dir, _up));
        len     =   CELL::length(_eye - _target);
        _target =   _eye + _dir * len;
        _matView=   CELL::lookAt<real>(_eye, _target, _up);
    }

    /**
     *	指定点推进摄像机
     */
    virtual void    scaleCameraByPos(const real3& pos,real present)
    {
        real3   dir     =   CELL::normalize(pos - _eye);

        real    dis     =   CELL::length(pos - _eye) * present;

        real    disCam  =   CELL::length(_target - _eye) * present;

        real3   dirCam  =   CELL::normalize(_target - _eye);

        _eye    =   pos - dir * dis;
        _target =   _eye + dirCam * disCam;

        update();
    }

    const matrix4r&  getView() const
    {
        return  _matView;
    }
    virtual void    update()
    {
        _matView    =   CELL::lookAt(_eye,_target,_up);
    }

    Ray createRayFromScreen(int x, int y)
    {
        real4  minWorld;
        real4  maxWorld;
        real4  screen(real(x),real(y),0,1);
        real4  screen1(real(x),real(y),1,1);
        unProject(screen,minWorld);
        unProject(screen1,maxWorld);

        Ray     ray;
        ray.setOrigin(real3(minWorld.x,minWorld.y,minWorld.z));
        real3  dir(maxWorld.x-minWorld.x, maxWorld.y-minWorld.y, maxWorld.z-minWorld.z);
        ray.setDirection(normalize(dir));
        return  ray;
    }
};

#endif //NATIVECPPAPP_CAMERA3D_HPP
