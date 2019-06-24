//
// Created by zzr on 2019/3/18.
//
#pragma once
#ifndef NATIVECPPAPP_CELLMATH_HPP
#define NATIVECPPAPP_CELLMATH_HPP


#include <cstdio>
#include <cassert>
#include <cmath>
#include <stdlib.h>
#include <vector>
#include <map>
#include <limits>

namespace CELL
{

    #define PI                      3.14159265358979323
    // android/opengl/Matrix下的主要常用方法  对应的cpp实现
    // http://androidxref.com/8.1.0_r33/xref/frameworks/base/core/jni/android/opengl/util.cpp#multiplyMM
    #define I(_i, _j) ((_j)+ 4*(_i))

    class Matrix
    {
    public:
        /**
         * Multiplies two 4x4 matrices together and stores the result in a third 4x4 matrix.
         * In matrix notation: result = lhs x rhs.
         */
        static void multiplyMM(float* re, const float* lhs, const float* rhs)
        {
            for (int i=0 ; i<4 ; i++) {
                const float rhs_i0 = rhs[ I(i,0) ];
                float ri0 = lhs[ I(0,0) ] * rhs_i0;
                float ri1 = lhs[ I(0,1) ] * rhs_i0;
                float ri2 = lhs[ I(0,2) ] * rhs_i0;
                float ri3 = lhs[ I(0,3) ] * rhs_i0;
                for (int j=1 ; j<4 ; j++) {
                    const float rhs_ij = rhs[ I(i,j) ];
                    ri0 += lhs[ I(j,0) ] * rhs_ij;
                    ri1 += lhs[ I(j,1) ] * rhs_ij;
                    ri2 += lhs[ I(j,2) ] * rhs_ij;
                    ri3 += lhs[ I(j,3) ] * rhs_ij;
                }
                re[ I(i,0) ] = ri0;
                re[ I(i,1) ] = ri1;
                re[ I(i,2) ] = ri2;
                re[ I(i,3) ] = ri3;
            }
        }


        /**
         * Multiplies a 4 element vector by a 4x4 matrix and stores the result in a
         * 4-element column vector. In matrix notation: result = lhs x rhs
         */
        static void multiplyMV(float* r, const float* lhs, const float* rhs)
        {
            mx4transform(rhs[0], rhs[1], rhs[2], rhs[3], lhs, r);
        }

        static inline
        void mx4transform(float x, float y, float z, float w, const float* pM, float* pDest)
        {
            pDest[0] = pM[0 + 4 * 0] * x + pM[0 + 4 * 1] * y + pM[0 + 4 * 2] * z + pM[0 + 4 * 3] * w;
            pDest[1] = pM[1 + 4 * 0] * x + pM[1 + 4 * 1] * y + pM[1 + 4 * 2] * z + pM[1 + 4 * 3] * w;
            pDest[2] = pM[2 + 4 * 0] * x + pM[2 + 4 * 1] * y + pM[2 + 4 * 2] * z + pM[2 + 4 * 3] * w;
            pDest[3] = pM[3 + 4 * 0] * x + pM[3 + 4 * 1] * y + pM[3 + 4 * 2] * z + pM[3 + 4 * 3] * w;
        }

        /**
         * Sets matrix m to the identity matrix.
         */
        static void setIdentityM(float* sm, int smOffset)
        {
            for (int i=0 ; i<16 ; i++) {
                sm[smOffset + i] = 0;
            }
            for(int i = 0; i < 16; i += 5) {
                sm[smOffset + i] = 1.0f;
            }
        }

        /**
         * Defines a projection matrix in terms of a field of view angle,
         * an aspect ratio, and z clip planes.
         */
        static void perspectiveM(float* m, float yFovInDegrees, float aspect, float n, float f)
        {
            float angleInRadians = (float) (yFovInDegrees * PI / 360.0);
            float a = (float) (1.0/ tan(angleInRadians));
            //矩阵都是先列后行
            m[0] = a / aspect;  m[4] = 0;   m[8] = 0;               m[12] = 0;
            m[1] = 0;           m[5] = a;   m[9] = 0;               m[13] = 0;
            m[2] = 0;           m[6] = 0;   m[10] = -((f+n)/(f-n)); m[14] = -((2*f*n)/(f-n));
            m[3] = 0;           m[7] = 0;   m[11] = -1;             m[15] = 0;
        }


        /**
         * Defines a viewing transformation in terms of an eye point, a center of
         * view, and an up vector.
         */
        static void setLookAtM(float* rm, int rmOffset,
                               float eyeX, float eyeY, float eyeZ,
                               float centerX, float centerY, float centerZ,
                               float upX, float upY, float upZ)
        {
            // See the OpenGL GLUT documentation for gluLookAt for a description
            // of the algorithm. We implement it in a straightforward way:
            float fx = centerX - eyeX;
            float fy = centerY - eyeY;
            float fz = centerZ - eyeZ;
            // Normalize f
            float rlf = 1.0f / length(fx, fy, fz);
            fx *= rlf;
            fy *= rlf;
            fz *= rlf;
            // compute s = f x up (x means "cross product")
            float sx = fy * upZ - fz * upY;
            float sy = fz * upX - fx * upZ;
            float sz = fx * upY - fy * upX;
            // and normalize s
            float rls = 1.0f / length(sx, sy, sz);
            sx *= rls;
            sy *= rls;
            sz *= rls;
            // compute u = s x f
            float ux = sy * fz - sz * fy;
            float uy = sz * fx - sx * fz;
            float uz = sx * fy - sy * fx;

            rm[rmOffset + 0] = sx;
            rm[rmOffset + 1] = ux;
            rm[rmOffset + 2] = -fx;
            rm[rmOffset + 3] = 0.0f;
            rm[rmOffset + 4] = sy;
            rm[rmOffset + 5] = uy;
            rm[rmOffset + 6] = -fy;
            rm[rmOffset + 7] = 0.0f;
            rm[rmOffset + 8] = sz;
            rm[rmOffset + 9] = uz;
            rm[rmOffset + 10] = -fz;
            rm[rmOffset + 11] = 0.0f;
            rm[rmOffset + 12] = 0.0f;
            rm[rmOffset + 13] = 0.0f;
            rm[rmOffset + 14] = 0.0f;
            rm[rmOffset + 15] = 1.0f;

            translateM(rm, rmOffset, -eyeX, -eyeY, -eyeZ);
        }

        static inline
        float length(float x, float y, float z) {
            return (float) sqrt(x * x + y * y + z * z);
        }

        // Translates matrix m by x, y, and z in place.
        static void translateM(float* m, int mOffset, float x, float y, float z)
        {
            for (int i=0 ; i<4 ; i++) {
                int mi = mOffset + i;
                m[12 + mi] += m[mi] * x + m[4 + mi] * y + m[8 + mi] * z;
            }
        }

        // Scales matrix m in place by sx, sy, and sz.
        static void scaleM(float* m, int mOffset, float x, float y, float z)
        {
            for (int i=0 ; i<4 ; i++) {
                int mi = mOffset + i;
                m[     mi] *= x;
                m[ 4 + mi] *= y;
                m[ 8 + mi] *= z;
            }
        }
    };


    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////**********************************///////////////////////////////////////////////////////////
    //////////* CELLMath Version2 for Camera3D *///////////////////////////////////////////////////////////
    //////////**********************************///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////

    #define TWO_PI                  6.28318530717958647
    #define HALF_PI                 1.57079632679489661
    #define DEG2RAD(theta)          (0.01745329251994329 * theta)
    #define RAD2DEG                 57.2957795130823208
    #define LOG2                    0.69314718055994529
    #define WGS_84_RADIUS_EQUATOR   6378137.0
    #define WGS_84_RADIUS_POLAR     6356752.3142
    #define MIN(a, b)                ((a) < (b) ? (a) : (b))
    #define MAX(a, b)                ((a) > (b) ? (a) : (b))
    #ifndef FLT_MAX
    #define FLT_MAX                 3.402823466e+38F
    #endif

    #ifndef FLT_MIN
    #define FLT_MIN                 1.175494351e-38F
    #endif

    #define MAKE_INT(a, b)          ((int)(((short)(((int)(a)) & 0xffff)) | ((int)((short)(((int)(b)) & 0xffff))) << 16))

    typedef unsigned char byte;
    typedef long long int64;
    typedef unsigned short ushort;
    typedef unsigned int uint;
    typedef unsigned long ulong;

    template<class T>
    inline T tmin(T a, T b) {
        return a < b ? a : b;
    }

    template<class T>
    inline T tmax(T a, T b) {
        return a > b ? a : b;
    }


    union LargeInt {
        struct __LARGE_INT {
            unsigned int LowPart;
            unsigned int HighPart;
        } _largeInt;
        int64 int64Data;
    };


    inline float unitRandom() {
        return float(rand()) / float(RAND_MAX);
    }

    //-----------------------------------------------------------------------
    inline float rangeRandom(float fLow, float fHigh) {
        return (fHigh - fLow) * unitRandom() + fLow;
    }

    /**
    *   产生64位数字
    */
    inline int64 makeInt64(unsigned low, unsigned hi) {
        LargeInt intdata;
        intdata._largeInt.HighPart = low;
        intdata._largeInt.LowPart = hi;
        return intdata.int64Data;
    }


    template<typename T>
    struct tvec2 {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef tvec2<T> type;

        value_type x;
        value_type y;


        value_type &operator[](size_type i) {
            assert(i < this->length());
            return (&x)[i];
        }

        value_type const &operator[](size_type i) const {
            assert(i < this->length());
            return (&x)[i];
        }

        tvec2() :
                x(value_type(0)),
                y(value_type(0)) {}

        tvec2(tvec2<T> const &v) :
                x(v.x),
                y(v.y) {}

        tvec2(value_type const &s) :
                x(s),
                y(s) {}

        tvec2(value_type const &s1, value_type const &s2) :
                x(s1),
                y(s2) {}

        template<typename U>
        tvec2(U const &x) :
                x(value_type(x)),
                y(value_type(x)) {}

        template<typename U, typename V>
        tvec2(U const &a, V b) :
                x(value_type(a)),
                y(value_type(b)) {}


        template<typename U>
        tvec2(tvec2<U> const &v) :
                x(value_type(v.x)),
                y(value_type(v.y)) {}

        tvec2<T> &operator=(tvec2<T> const &v) {
            this->x = v.x;
            this->y = v.y;
            return *this;
        }

        template<typename U>
        tvec2<T> &operator=(tvec2<U> const &v) {
            this->x = T(v.x);
            this->y = T(v.y);
            return *this;
        }

        template<typename U>
        tvec2<T> &operator+=(U const &s) {
            this->x += T(s);
            this->y += T(s);
            return *this;
        }

        template<typename U>
        tvec2<T> &operator+=(tvec2<U> const &v) {
            this->x += T(v.x);
            this->y += T(v.y);
            return *this;
        }


        template<typename U>
        tvec2<T> &operator-=(U const &s) {
            this->x -= T(s);
            this->y -= T(s);
            return *this;
        }

        template<typename U>
        tvec2<T> &operator-=(tvec2<U> const &v) {
            this->x -= T(v.x);
            this->y -= T(v.y);
            return *this;
        }

        template<typename U>
        tvec2<T> &operator*=(U s) {
            this->x *= T(s);
            this->y *= T(s);
            return *this;
        }

        template<typename U>
        tvec2<T> &operator*=(tvec2<U> const &v) {
            this->x *= T(v.x);
            this->y *= T(v.y);
            return *this;
        }

        template<typename U>
        tvec2<T> &operator/=(U s) {
            this->x /= T(s);
            this->y /= T(s);
            return *this;
        }

        template<typename U>
        tvec2<T> &operator/=(tvec2<U> const &v) {
            this->x /= T(v.x);
            this->y /= T(v.y);
            return *this;
        }

        tvec2<T> &operator++() {
            ++this->x;
            ++this->y;
            return *this;
        }

        tvec2<T> &operator--() {
            --this->x;
            --this->y;
            return *this;
        }

        void makeCeil(tvec2<T> cmp) {
            if (cmp.x > x) x = cmp.x;
            if (cmp.y > y) y = cmp.y;
        }

        void makeFloor(tvec2<T> cmp) {
            if (cmp.x < x) x = cmp.x;
            if (cmp.y < y) y = cmp.y;
        }
    };

    template<typename T>
    tvec2<T> rotate(tvec2<T> const &v, T angle) {
        tvec2 <T> res;
        T const c(cos(DEG2RAD(angle)));
        T const s(sin(DEG2RAD(angle)));
        res.x = v.x * c - v.y * s;
        res.y = v.x * s + v.y * c;
        return res;
    }

    template<typename T>
    bool operator==(tvec2<T> const &v1, tvec2<T> const &v2) {
        return (v1.x == v2.x) && (v1.y == v2.y);
    }

    template<typename T>
    bool operator!=(tvec2<T> const &v1, tvec2<T> const &v2) {
        return (v1.x != v2.x) || (v1.y != v2.y);
    }

    template<typename T>
    tvec2<T> operator+(tvec2<T> const &v, T const &s) {
        return tvec2<T>(
                v.x + T(s),
                v.y + T(s));
    }

    template<typename T>
    tvec2<T> operator+(T const &s, tvec2<T> const &v) {
        return tvec2<T>(
                T(s) + v.x,
                T(s) + v.y);
    }

    template<typename T>
    tvec2<T> operator+(tvec2<T> const &v1, tvec2<T> const &v2) {
        return tvec2<T>(
                v1.x + T(v2.x),
                v1.y + T(v2.y));
    }

    template<typename T>
    tvec2<T> operator-(tvec2<T> const &v, T const &s) {
        return tvec2<T>(
                v.x - T(s),
                v.y - T(s));
    }

    template<typename T>
    tvec2<T> operator-(T const &s, tvec2<T> const &v) {
        return tvec2<T>(
                T(s) - v.x,
                T(s) - v.y);
    }

    template<typename T>
    tvec2<T> operator-(tvec2<T> const &v1, tvec2<T> const &v2) {
        return tvec2<T>(
                v1.x - T(v2.x),
                v1.y - T(v2.y));
    }

    template<typename T>
    tvec2<T> operator*(tvec2<T> const &v, T const &s) {
        return tvec2<T>(
                v.x * T(s),
                v.y * T(s));
    }

    template<typename T>
    tvec2<T> operator*(T const &s, tvec2<T> const &v) {
        return tvec2<T>(
                T(s) * v.x,
                T(s) * v.y);
    }

    template<typename T>
    tvec2<T> operator*(tvec2<T> const &v1, tvec2<T> const &v2) {
        return tvec2<T>(
                v1.x * T(v2.x),
                v1.y * T(v2.y));
    }

    template<typename T>
    tvec2<T> operator/(tvec2<T> const &v, T const &s) {
        return tvec2<T>(
                v.x / T(s),
                v.y / T(s));
    }

    template<typename T>
    tvec2<T> operator/(T const &s, tvec2<T> const &v) {
        return tvec2<T>(
                T(s) / v.x,
                T(s) / v.y);
    }

    template<typename T>
    tvec2<T> operator/(tvec2<T> const &v1, tvec2<T> const &v2) {
        return tvec2<T>(
                v1.x / T(v2.x),
                v1.y / T(v2.y)
        );
    }

    template<typename T>
    tvec2<T> operator-(tvec2<T> const &v) {
        return tvec2<T>(
                -v.x,
                -v.y
        );
    }

    template<typename T>
    tvec2<T> operator++(tvec2<T> const &v, int) {
        return tvec2<T>(
                v.x + T(1),
                v.y + T(1)
        );
    }

    template<typename T>
    tvec2<T> operator--(tvec2<T> const &v, int) {
        return tvec2<T>(
                v.x - T(1),
                v.y - T(1)
        );
    }


    template<typename T>
    struct tvec3 {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef tvec3<T> type;

        value_type x;
        value_type y;
        value_type z;

        size_type length() const {
            return 3;
        }

        value_type &operator[](size_type i) {
            assert(i < this->length());
            return (&x)[i];
        }

        value_type const &operator[](size_type i) const {
            assert(i < this->length());
            return (&x)[i];
        }

        inline tvec3() :
                x(value_type(0)),
                y(value_type(0)),
                z(value_type(0)) {}

        inline tvec3(tvec3<T> const &v) :
                x(v.x),
                y(v.y),
                z(v.z) {}

        inline tvec3(value_type s) :
                x(s),
                y(s),
                z(s) {}

        inline tvec3(value_type s0, value_type s1, value_type s2) :
                x(s0),
                y(s1),
                z(s2) {}


        template<typename U>
        tvec3(U s) :
                x(value_type(s)),
                y(value_type(s)),
                z(value_type(s)) {}

        template<typename A, typename B, typename C>
        tvec3(A x, B y, C z) :
                x(value_type(x)),
                y(value_type(y)),
                z(value_type(z)) {}

        template<typename A, typename B>
        tvec3(tvec2<A> const &v, B s) :
                x(value_type(v.x)),
                y(value_type(v.y)),
                z(value_type(s)) {}

        template<typename A, typename B>
        tvec3(A s, tvec2<B> const &v) :
                x(value_type(s)),
                y(value_type(v.x)),
                z(value_type(v.y)) {}

        template<typename U>
        tvec3(tvec3<U> const &v) :
                x(value_type(v.x)),
                y(value_type(v.y)),
                z(value_type(v.z)) {}


        tvec3<T> &operator=(tvec3<T> const &v) {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            return *this;
        }

        template<typename U>
        tvec3<T> &operator=(tvec3<U> const &v) {
            this->x = T(v.x);
            this->y = T(v.y);
            this->z = T(v.z);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator+=(U const &s) {
            this->x += T(s);
            this->y += T(s);
            this->z += T(s);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator+=(tvec3<U> const &v) {
            this->x += T(v.x);
            this->y += T(v.y);
            this->z += T(v.z);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator-=(U const &s) {
            this->x -= T(s);
            this->y -= T(s);
            this->z -= T(s);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator-=(tvec3<U> const &v) {
            this->x -= T(v.x);
            this->y -= T(v.y);
            this->z -= T(v.z);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator*=(U const &s) {
            this->x *= T(s);
            this->y *= T(s);
            this->z *= T(s);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator*=(tvec3<U> const &v) {
            this->x *= T(v.x);
            this->y *= T(v.y);
            this->z *= T(v.z);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator/=(U const &s) {
            this->x /= T(s);
            this->y /= T(s);
            this->z /= T(s);
            return *this;
        }

        template<typename U>
        tvec3<T> &operator/=(tvec3<U> const &v) {
            this->x /= T(v.x);
            this->y /= T(v.y);
            this->z /= T(v.z);
            return *this;
        }

        tvec3<T> &operator++() {
            ++this->x;
            ++this->y;
            ++this->z;
            return *this;
        }

        tvec3<T> &operator--() {
            --this->x;
            --this->y;
            --this->z;
            return *this;
        }

        void makeFloor(const tvec3<T> &cmp) {
            if (cmp.x < x) x = cmp.x;
            if (cmp.y < y) y = cmp.y;
            if (cmp.z < z) z = cmp.z;
        }

        void makeCeil(const tvec3<T> &cmp) {
            if (cmp.x > x) x = cmp.x;
            if (cmp.y > y) y = cmp.y;
            if (cmp.z > z) z = cmp.z;
        }

        T lengthf() const {
            return (T) sqrtf(x * x + y * y + z * z);
        }

    };

    template<typename T>
    bool operator>(const tvec3<T> &left, const tvec3<T> &right) {
        return left.x > right.x && left.y > right.y && left.z > right.z;
    }

    template<typename T>
    bool operator<(const tvec3<T> &left, const tvec3<T> &right) {
        return left.x < right.x && left.y < right.y && left.z < right.z;
    }

    template<typename T>
    tvec3<T> rotateX(const tvec3<T> &v, T angle) {
        tvec3 <T> res(v);
        T c = cos(T(DEG2RAD(angle)));
        T s = sin(T(DEG2RAD(angle)));

        res.y = v.y * c - v.z * s;
        res.z = v.y * s + v.z * c;
        return res;
    }

    template<typename T>
    tvec3<T> rotateY(tvec3<T> const &v, T angle) {
        tvec3 <T> res = v;

        T c = cos(T(DEG2RAD(angle)));
        T s = sin(T(DEG2RAD(angle)));

        res.x = v.x * c + v.z * s;
        res.z = -v.x * s + v.z * c;
        return res;
    }

    template<typename T>
    tvec3<T> rotateZ(tvec3<T> const &v, T angle) {

        tvec3 <T> res = v;

        T c = cos(DEG2RAD(angle));

        T s = sin(DEG2RAD(angle));

        res.x = v.x * c - v.y * s;
        res.y = v.x * s + v.y * c;
        return res;
    }

    /**
    *   两个向量的夹角
    *   定义两个向量 A,B
    *   A·B = |A|*|B|*cos(@)
    *   cos(@) = A·B/|A|*|B|
    *   @ = acos(@)
    */
    template<typename T>
    T angleBetweenVector(const tvec3<T> &a, const tvec3<T> &b) {
#define Mag(V) (sqrtf(V.x*V.x + V.y*V.y + V.z*V.z))
        T dotProduct = dot(a, b);
        T vectorsMagnitude = Mag(a) * Mag(b);
        T angle = acos(dotProduct / vectorsMagnitude);
        T result = angle * T(RAD2DEG);
        if (_isnan(result)) {
            return T(0);
        } else {
            return result;
        }
    }

    template<typename T>
    inline bool _isnan(T t) {
        return t == t;
    }

    template<typename T>
    T angleBetweenVector(const tvec2<T> &a, const tvec2<T> &b) {
#define Mag2D(V)    (sqrtf(V.x*V.x + V.y*V.y))

        T dotProduct = dot(a, b);
        T vectorsMagnitude = Mag2D(a) * Mag2D(b);
        T angle = acos(dotProduct / vectorsMagnitude);
        T result = angle * T(RAD2DEG);
        if (_isnan(result)) {
            return T(0);
        } else {
            return result;
        }
    }

    template<typename T>
    static T clamp(T val, T minval, T maxval) {
        assert (minval < maxval && "Invalid clamp range");
        return MAX(MIN(val, maxval), minval);
    }

    template<typename T>
    inline T acosEx(T val) {
        if (T(-1.0f) < val) {
            if (val < 1.0f)
                return T(acos(val));
            else
                return T(0);
        } else {
            return T(PI);
        }
    }

    template<typename T>
    inline T angleBetween(const tvec3<T> &a, const tvec3<T> &b) {
        T lenProduct = a.lengthf() * b.lengthf();

        // Divide by zero check
        if (lenProduct < 1e-6f)
            lenProduct = 1e-6f;

        float f = dot(a, b) / lenProduct;

        f = clamp(f, T(-1.0), T(1.0));
        return acosEx(f);

    }

    /**
    *   点在多边形里
    *   如果点在多边形中，则，点与边的夹角之和 == 360
    */
    template<typename T>
    bool insidePolyon(const tvec3<T> &point, const tvec3<T> polygon[], size_t count) {
        tvec3 <T> vA, vB;
        T angle = T(0.0);
        for (size_t i = 0; i < count; ++i) {
            vA = polygon[i] - point;
            vB = polygon[(i + 1) % count] - point;
            angle += angleBetweenVector(vA, vB);
        }
        if (abs(angle - 360) >= 0.5f) {
            return true;
        }
        return false;
    }

    template<typename T>
    bool insidePolyon(const tvec2<T> &point, const tvec2<T> polygon[], size_t count) {
        T angle = T(0.0);
        tvec2 <T> vA, vB;
        for (size_t i = 0; i < count; ++i) {
            vA = polygon[i] - point;
            vB = polygon[(i + 1) % count] - point;
            tvec3 <T> a(vA.x, vA.y, 0);
            tvec3 <T> b(vB.x, vB.y, 0);
            angle += angleBetweenVector<T>(a, b);
        }
        if (abs(angle - 360) >= 0.5f) {
            return true;
        }
        return false;
    }

    template<typename T>
    bool pointinTriangle(tvec3<T> A, tvec3<T> B, tvec3<T> C, tvec3<T> P) {
        tvec3 <T> v0 = C - A;
        tvec3 <T> v1 = B - A;
        tvec3 <T> v2 = P - A;

        float dot00 = dot(v0, v0);
        float dot01 = dot(v0, v1);
        float dot02 = dot(v0, v2);
        float dot11 = dot(v1, v1);
        float dot12 = dot(v1, v2);

        float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

        float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
        if (u < 0 || u > 1) // if u out of range, return directly
        {
            return false;
        }

        float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
        if (v < 0 || v > 1) // if v out of range, return directly
        {
            return false;
        }

        return u + v <= 1;
    }


    template<typename T>
    bool pointinTriangle(tvec2<T> A, tvec2<T> B, tvec2<T> C, tvec2<T> P) {
        return pointinTriangle(
                tvec3<T>(A.x, A.y, 0),
                tvec3<T>(B.x, B.y, 0),
                tvec3<T>(C.x, C.y, 0),
                tvec3<T>(P.x, P.y, 0));
    }


    /**
    *   射线与三角形相交
    */
    template<typename T>
    bool intersectTriangle(
            const tvec3<T> &orig,
            const tvec3<T> &dir,
            tvec3<T> &v0,
            tvec3<T> &v1,
            tvec3<T> &v2,
            T *t,
            T *u,
            T *v
    ) {
        // Find vectors for two edges sharing vert0
        tvec3 <T> edge1 = v1 - v0;
        tvec3 <T> edge2 = v2 - v0;

        // Begin calculating determinant - also used to calculate U parameter
        tvec3 <T> pvec;
        pvec = cross(dir, edge2);

        // If determinant is near zero, ray lies in plane of triangle
        T det = dot(edge1, pvec);

        tvec3 <T> tvec;
        if (det > 0) {
            tvec = orig - v0;
        } else {
            tvec = v0 - orig;
            det = -det;
        }
        if (det < 0.0001f)
            return false;
        // Calculate U parameter and test bounds
        *u = dot(tvec, pvec);
        if (*u < 0.0f || *u > det)
            return false;

        // Prepare to test V parameter
        tvec3 <T> qvec;
        qvec = cross(tvec, edge1);

        // Calculate V parameter and test bounds
        *v = dot(dir, qvec);
        if (*v < T(0.0f) || *u + *v > det)
            return false;

        *t = dot(edge2, qvec);
        T fInvDet = T(1.0) / det;
        *t *= fInvDet;
        *u *= fInvDet;
        *v *= fInvDet;

        return true;
    }

    /**
    *   计算三角形面积
    */
    template<typename T>
    T calcTriangleArea(const tvec3<T> &pt1, const tvec3<T> &pt2, const tvec3<T> &pt3) {
        tvec3 <T> e1 = pt2 - pt1;
        tvec3 <T> e2 = pt3 - pt1;
        tvec3 <T> e3 = cross(e1, e2);
        return length(e3) * T(0.5);
    }


    template<typename T>
    bool operator==(tvec3<T> const &v1, tvec3<T> const &v2) {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
    }

    template<typename T>
    bool operator!=(tvec3<T> const &v1, tvec3<T> const &v2) {
        return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z);
    }


    template<typename T>
    tvec3<T> operator+(tvec3<T> const &v, T const &s) {
        return tvec3<T>(
                v.x + T(s),
                v.y + T(s),
                v.z + T(s));
    }

    template<typename T>
    tvec3<T> operator+(T const &s, tvec3<T> const &v) {
        return tvec3<T>(
                T(s) + v.x,
                T(s) + v.y,
                T(s) + v.z);
    }

    template<typename T>
    tvec3<T> operator+(tvec3<T> const &v1, tvec3<T> const &v2) {
        return tvec3<T>(
                v1.x + T(v2.x),
                v1.y + T(v2.y),
                v1.z + T(v2.z));
    }

    template<typename T>
    tvec3<T> operator-(tvec3<T> const &v, T const &s) {
        return tvec3<T>(
                v.x - T(s),
                v.y - T(s),
                v.z - T(s));
    }

    template<typename T>
    tvec3<T> operator-(T const &s, tvec3<T> const &v) {
        return tvec3<T>(
                T(s) - v.x,
                T(s) - v.y,
                T(s) - v.z);
    }

    template<typename T>
    tvec3<T> operator-(tvec3<T> const &v1, tvec3<T> const &v2) {
        return tvec3<T>(
                v1.x - T(v2.x),
                v1.y - T(v2.y),
                v1.z - T(v2.z));
    }

    template<typename T>
    tvec3<T> operator*(tvec3<T> const &v, T const &s) {
        return tvec3<T>(
                v.x * T(s),
                v.y * T(s),
                v.z * T(s));
    }

    template<typename T>
    tvec3<T> operator*(T const &s, tvec3<T> const &v) {
        return tvec3<T>(
                T(s) * v.x,
                T(s) * v.y,
                T(s) * v.z);
    }

    template<typename T>
    tvec3<T> operator*(tvec3<T> const &v1, tvec3<T> const &v2) {
        return tvec3<T>(
                v1.x * T(v2.x),
                v1.y * T(v2.y),
                v1.z * T(v2.z));
    }

    template<typename T>
    tvec3<T> operator/(tvec3<T> const &v, T const &s) {
        return tvec3<T>(
                v.x / T(s),
                v.y / T(s),
                v.z / T(s));
    }

    template<typename T>
    tvec3<T> operator/(T const &s, tvec3<T> const &v) {
        return tvec3<T>(
                T(s) / v.x,
                T(s) / v.y,
                T(s) / v.z);
    }

    template<typename T>
    tvec3<T> operator/(tvec3<T> const &v1, tvec3<T> const &v2) {
        return tvec3<T>(
                v1.x / T(v2.x),
                v1.y / T(v2.y),
                v1.z / T(v2.z));
    }

    template<typename T>
    tvec3<T> operator-(tvec3<T> const &v) {
        return tvec3<T>(
                -v.x,
                -v.y,
                -v.z);
    }

    template<typename T>
    tvec3<T> operator++(tvec3<T> const &v, int) {
        return tvec3<T>(
                v.x + T(1),
                v.y + T(1),
                v.z + T(1));
    }

    template<typename T>
    tvec3<T> operator--(tvec3<T> const &v, int) {
        return tvec3<T>(
                v.x - T(1),
                v.y - T(1),
                v.z - T(1));
    }

    template<typename T>
    struct tvec4 {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef tvec4<T> type;


        value_type x, y, z, w;

        size_type length() const {
            return 4;
        }

        value_type &operator[](size_type i) {
            assert(i < this->length());
            return (&x)[i];
        }

        value_type const &operator[](size_type i) const {
            assert(i < this->length());
            return (&x)[i];
        }

        tvec4() :
                x(value_type(0)),
                y(value_type(0)),
                z(value_type(0)),
                w(value_type(0)) {}

        tvec4(tvec3<T> const &v, T s) :
                x(v.x),
                y(v.y),
                z(v.z),
                w(s) {}

        tvec4(T s) :
                x(s),
                y(s),
                z(s),
                w(s) {}

        tvec4(tvec4<T> const &v) :
                x(v.x),
                y(v.y),
                z(v.z),
                w(v.w) {}

        template<typename A, typename B>
        tvec4(tvec3<A> const &v, B s):
                x(value_type(v.x)),
                y(value_type(v.y)),
                z(value_type(v.z)),
                w(value_type(s)) {}

        template<typename A, typename B>
        tvec4(A s, tvec3<B> const &v):
                x(value_type(s)),
                y(value_type(v.x)),
                z(value_type(v.y)),
                w(value_type(v.z)) {}

        template<typename U>
        tvec4(tvec4<U> const &v) :
                x(value_type(v.x)),
                y(value_type(v.y)),
                z(value_type(v.z)),
                w(value_type(v.w)) {}

        tvec4
                (
                        value_type s1,
                        value_type s2,
                        value_type s3,
                        value_type s4
                ) :
                x(s1),
                y(s2),
                z(s3),
                w(s4) {}

        tvec4<T> &operator=(tvec4<T> const &v) {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            this->w = v.w;
            return *this;
        }

        template<typename U>
        tvec4<T> &operator=(tvec4<U> const &v) {
            this->x = T(v.x);
            this->y = T(v.y);
            this->z = T(v.z);
            this->w = T(v.w);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator+=(U const &s) {
            this->x += T(s);
            this->y += T(s);
            this->z += T(s);
            this->w += T(s);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator+=(tvec4<U> const &v) {
            this->x += T(v.x);
            this->y += T(v.y);
            this->z += T(v.z);
            this->w += T(v.w);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator-=(U const &s) {
            this->x -= T(s);
            this->y -= T(s);
            this->z -= T(s);
            this->w -= T(s);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator-=(tvec4<U> const &v) {
            this->x -= T(v.x);
            this->y -= T(v.y);
            this->z -= T(v.z);
            this->w -= T(v.w);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator*=(U const &s) {
            this->x *= T(s);
            this->y *= T(s);
            this->z *= T(s);
            this->w *= T(s);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator*=(tvec4<U> const &v) {
            this->x *= T(v.x);
            this->y *= T(v.y);
            this->z *= T(v.z);
            this->w *= T(v.w);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator/=(U const &s) {
            this->x /= T(s);
            this->y /= T(s);
            this->z /= T(s);
            this->w /= T(s);
            return *this;
        }

        template<typename U>
        tvec4<T> &operator/=(tvec4<U> const &v) {
            this->x /= T(v.x);
            this->y /= T(v.y);
            this->z /= T(v.z);
            this->w /= T(v.w);
            return *this;
        }

        tvec4<T> &operator++() {
            ++this->x;
            ++this->y;
            ++this->z;
            ++this->w;
            return *this;
        }

        tvec4<T> &operator--() {
            --this->x;
            --this->y;
            --this->z;
            --this->w;
            return *this;
        }
    };

    template<typename T>
    tvec4<T> rotateX(const tvec4<T> &v, T angle) {
        tvec4 <T> res(v);
        T c = cos(DEG2RAD(angle));
        T s = sin(DEG2RAD(angle));

        res.y = v.y * c - v.z * s;
        res.z = v.y * s + v.z * c;
        return res;
    }

    template<typename T>
    tvec4<T> rotateY(tvec4<T> const &v, T angle) {
        tvec4 <T> res = v;

        T c = cos(DEG2RAD(angle));
        T s = sin(DEG2RAD(angle));

        res.x = v.x * c + v.z * s;
        res.z = -v.x * s + v.z * c;
        return res;
    }

    template<typename T>
    tvec4<T> rotateZ(tvec4<T> const &v, T angle) {

        tvec4 <T> res = v;

        T c = cos(DEG2RAD(angle));
        T s = sin(DEG2RAD(angle));

        res.x = v.x * c - v.y * s;
        res.y = v.x * s + v.y * c;
        return res;
    }

    template<typename T>
    tvec4<T> operator+(tvec4<T> const &v, T const &s) {
        return tvec4<T>(
                v.x + s,
                v.y + s,
                v.z + s,
                v.w + s);
    }

    template<typename T>
    tvec4<T> operator+(T const &s, tvec4<T> const &v) {
        return tvec4<T>(
                s + v.x,
                s + v.y,
                s + v.z,
                s + v.w);
    }

    template<typename T>
    tvec4<T> operator+(tvec4<T> const &v1, tvec4<T> const &v2) {
        return tvec4<T>(
                v1.x + v2.x,
                v1.y + v2.y,
                v1.z + v2.z,
                v1.w + v2.w);
    }

    template<typename T>
    tvec4<T> operator-(tvec4<T> const &v, T const &s) {
        return tvec4<T>(
                v.x - s,
                v.y - s,
                v.z - s,
                v.w - s);
    }

    template<typename T>
    tvec4<T> operator-(T const &s, tvec4<T> const &v) {
        return tvec4<T>(
                s - v.x,
                s - v.y,
                s - v.z,
                s - v.w);
    }

    template<typename T>
    tvec4<T> operator-
            (
                    tvec4<T> const &v1,
                    tvec4<T> const &v2
            ) {
        return tvec4<T>(
                v1.x - v2.x,
                v1.y - v2.y,
                v1.z - v2.z,
                v1.w - v2.w);
    }

    template<typename T>
    tvec4<T> operator*(tvec4<T> const &v, T const &s) {
        return tvec4<T>(
                v.x * s,
                v.y * s,
                v.z * s,
                v.w * s);
    }

    template<typename T>
    tvec4<T> operator*(T const &s, tvec4<T> const &v) {
        return tvec4<T>(
                s * v.x,
                s * v.y,
                s * v.z,
                s * v.w);
    }

    template<typename T>
    tvec4<T> operator*(tvec4<T> const &v1, tvec4<T> const &v2) {
        return tvec4<T>(
                v1.x * v2.x,
                v1.y * v2.y,
                v1.z * v2.z,
                v1.w * v2.w);
    }

    template<typename T>
    tvec4<T> operator/(tvec4<T> const &v, T const &s) {
        return tvec4<T>(
                v.x / s,
                v.y / s,
                v.z / s,
                v.w / s);
    }

    template<typename T>
    tvec4<T> operator/(T const &s, tvec4<T> const &v) {
        return tvec4<T>(
                s / v.x,
                s / v.y,
                s / v.z,
                s / v.w);
    }

    template<typename T>
    tvec4<T> operator/(tvec4<T> const &v1, tvec4<T> const &v2) {
        return tvec4<T>(
                v1.x / v2.x,
                v1.y / v2.y,
                v1.z / v2.z,
                v1.w / v2.w);
    }

    template<typename T>
    tvec4<T> operator-(tvec4<T> const &v) {
        return tvec4<T>(
                -v.x,
                -v.y,
                -v.z,
                -v.w);
    }


    template<typename T>
    bool operator==
            (
                    tvec4<T> const &v1,
                    tvec4<T> const &v2
            ) {
        return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z) && (v1.w == v2.w);
    }

    template<typename T>
    bool operator!=(tvec4<T> const &v1, tvec4<T> const &v2) {
        return (v1.x != v2.x) || (v1.y != v2.y) || (v1.z != v2.z) || (v1.w != v2.w);
    }

    template<typename T>
    class trect {
    public:
        trect(T left = 0, T top = 0, T right = 0, T bottom = 0) {
            _left = left;
            _top = top;
            _right = right;
            _bottom = bottom;
        }

        void fromCenter(T x, T y, T size) {
            _left = x - size * T(0.5f);
            _top = y - size * T(0.5f);
            _right = x + size * T(0.5f);
            _bottom = y + size * T(0.5f);
        }

        void fromCenter(T x, T y, T sizeX, T sizeY) {
            _left = x - sizeX * T(0.5f);
            _top = y - sizeY * T(0.5f);
            _right = x + sizeX * T(0.5f);
            _bottom = y + sizeY * T(0.5f);
        }

        bool ptInRect(T x, T y) {
            return x >= _left && x <= _right && y >= _top && y <= _bottom;
        }

        tvec2<T> center() const {
            return tvec2<T>((_left + _right) * T(0.5f), (_bottom + _top) * T(0.5f));
        }

        tvec2<T> halSize() const {
            return tvec2<T>((_right - _left) * T(0.5f), (_bottom - _top) * T(0.5f));
        }

    public:
        T _left;
        T _top;
        T _right;
        T _bottom;
    };

    template<typename T>
    struct tmat2x2 {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef tvec2<T> col_type;
        typedef tvec2<T> row_type;
        typedef tmat2x2<T> type;
        typedef tmat2x2<T> transpose_type;


    public:
        tmat2x2<T> _inverse() const {
            value_type Determinant = this->value[0][0] * this->value[1][1] - this->value[1][0] * this->value[0][1];

            tmat2x2 <T> Inverse(
                    +this->value[1][1] / Determinant,
                    -this->value[0][1] / Determinant,
                    -this->value[1][0] / Determinant,
                    +this->value[0][0] / Determinant);
            return Inverse;
        }

    private:
        col_type value[2];

    public:

        size_type length() const {
            return 4;
        }

        static size_type col_size() {
            return 2;
        }

        static size_type row_size() {
            return 2;
        }

        col_type &operator[](size_type i) {
            assert(i < this->length());
            return this->value[i];
        }

        col_type const &operator[](size_type i) const {
            assert(i < this->length());
            return this->value[i];
        }

        tmat2x2() {
            this->value[0] = col_type(1, 0);
            this->value[1] = col_type(0, 1);
        }

        tmat2x2(tmat2x2<T> const &m) {
            this->value[0] = m.value[0];
            this->value[1] = m.value[1];
        }

        tmat2x2(value_type s) {
            value_type const Zero(0);
            this->value[0] = col_type(s, Zero);
            this->value[1] = col_type(Zero, s);
        }

        tmat2x2(value_type x0, value_type y0, value_type x1, value_type y1) {
            this->value[0] = col_type(x0, y0);
            this->value[1] = col_type(x1, y1);
        }

        tmat2x2(col_type const &v0, col_type const &v1) {
            this->value[0] = v0;
            this->value[1] = v1;
        }

        template<typename U>
        tmat2x2(U s) {
            value_type const Zero(0);
            this->value[0] = tvec2<T>(value_type(s), Zero);
            this->value[1] = tvec2<T>(Zero, value_type(s));
        }

        template<typename X1, typename Y1, typename X2, typename Y2>
        tmat2x2(X1 x1, Y1 y1, X2 x2, Y2 y2) {
            this->value[0] = col_type(value_type(x1), value_type(y1));
            this->value[1] = col_type(value_type(x2), value_type(y2));
        }

        template<typename V1, typename V2>
        tmat2x2
                (
                        tvec2<V1> const &v1,
                        tvec2<V2> const &v2
                ) {
            this->value[0] = col_type(v1);
            this->value[1] = col_type(v2);
        }

        template<typename U>
        tmat2x2(tmat2x2<U> const &m) {
            this->value[0] = col_type(m[0]);
            this->value[1] = col_type(m[1]);
        }

        tmat2x2<T> &operator=(tmat2x2<T> const &m) {
            this->value[0] = m[0];
            this->value[1] = m[1];
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator=
                (
                        tmat2x2<U> const &m
                ) {
            this->value[0] = m[0];
            this->value[1] = m[1];
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator+=(U const &s) {
            this->value[0] += s;
            this->value[1] += s;
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator+=
                (
                        tmat2x2<U> const &m
                ) {
            this->value[0] += m[0];
            this->value[1] += m[1];
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator-=(U const &s) {
            this->value[0] -= s;
            this->value[1] -= s;
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator-=(tmat2x2<U> const &m) {
            this->value[0] -= m[0];
            this->value[1] -= m[1];
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator*=(U const &s) {
            this->value[0] *= s;
            this->value[1] *= s;
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator*=(tmat2x2<U> const &m) {
            return (*this = *this * m);
        }

        template<typename U>
        tmat2x2<T> &operator/=(U const &s) {
            this->value[0] /= s;
            this->value[1] /= s;
            return *this;
        }

        template<typename U>
        tmat2x2<T> &operator/=(tmat2x2<U> const &m) {
            return (*this = *this / m);
        }

        tmat2x2<T> &operator++() {
            ++this->value[0];
            ++this->value[1];
            return *this;
        }

        tmat2x2<T> &operator--() {
            --this->value[0];
            --this->value[1];
            return *this;
        };
    };

    template<typename T>
    tmat2x2<T> rotate(T angle) {
        T c = cos(DEG2RAD(angle));
        T s = sin(DEG2RAD(angle));
        return tmat2x2<T>(c, s, -s, c);
    }

    template<typename T>
    tmat2x2<T> operator+(tmat2x2<T> const &m, T const &s) {
        return tmat2x2<T>(m[0] + s, m[1] + s);
    }

    template<typename T>
    tmat2x2<T> operator+(T const &s, tmat2x2<T> const &m) {
        return tmat2x2<T>(m[0] + s, m[1] + s);
    }

    template<typename T>
    tmat2x2<T> operator+(tmat2x2<T> const &m1, tmat2x2<T> const &m2) {
        return tmat2x2<T>(m1[0] + m2[0], m1[1] + m2[1]);
    }

    template<typename T>
    tmat2x2<T> operator-(tmat2x2<T> const &m, T const &s) {
        return tmat2x2<T>(m[0] - s, m[1] - s);
    }

    template<typename T>
    tmat2x2<T> operator-(T const &s, tmat2x2<T> const &m) {
        return tmat2x2<T>(s - m[0], s - m[1]);
    }

    template<typename T>
    tmat2x2<T> operator-(tmat2x2<T> const &m1, tmat2x2<T> const &m2) {
        return tmat2x2<T>(m1[0] - m2[0], m1[1] - m2[1]);
    }

    template<typename T>
    tmat2x2<T> operator*(tmat2x2<T> const &m, T const &s) {
        return tmat2x2<T>(m[0] * s, m[1] * s);
    }

    template<typename T>
    tmat2x2<T> operator*(T const &s, tmat2x2<T> const &m) {
        return tmat2x2<T>(m[0] * s, m[1] * s);
    }

    template<typename T>
    tvec2<T> operator*(tmat2x2<T> const &m, tvec2<T> const &v) {
        return tvec2<T>(
                m[0][0] * v.x + m[1][0] * v.y,
                m[0][1] * v.x + m[1][1] * v.y);
    }

    template<typename T>
    tvec2<T> operator*(tvec2<T> const &v, tmat2x2<T> const &m) {
        return tvec2<T>(
                v.x * m[0][0] + v.y * m[0][1],
                v.x * m[1][0] + v.y * m[1][1]);
    }

    template<typename T>
    tmat2x2<T> operator*(tmat2x2<T> const &m1, tmat2x2<T> const &m2) {
        return tmat2x2<T>(
                m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1],
                m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1],
                m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1],
                m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1]);
    }


    template<typename T>
    struct tmat3x3 {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef tvec3<T> col_type;
        typedef tvec3<T> row_type;
        typedef tmat3x3<T> type;
        typedef tmat3x3<T> transpose_type;


    private:
        // Data
        col_type value[3];

    public:
        size_type length() const {
            return 9;
        }

        size_type col_size() {
            return 3;
        }

        size_type row_size() {
            return 3;
        }

        T const *   data() const
        {
            return  &this->value[0][0];
        }

        tmat3x3() {
            value_type const Zero(0);
            value_type const One(1);
            this->value[0] = col_type(One, Zero, Zero);
            this->value[1] = col_type(Zero, One, Zero);
            this->value[2] = col_type(Zero, Zero, One);
        }

        tmat3x3
                (
                        tmat3x3<T> const &m
                ) {
            this->value[0] = m.value[0];
            this->value[1] = m.value[1];
            this->value[2] = m.value[2];
        }


        tmat3x3(value_type const &s) {
            value_type const Zero(0);
            this->value[0] = col_type(s, Zero, Zero);
            this->value[1] = col_type(Zero, s, Zero);
            this->value[2] = col_type(Zero, Zero, s);
        }


        tmat3x3
                (
                        value_type const &x0, value_type const &y0, value_type const &z0,
                        value_type const &x1, value_type const &y1, value_type const &z1,
                        value_type const &x2, value_type const &y2, value_type const &z2
                ) {
            this->value[0] = col_type(x0, y0, z0);
            this->value[1] = col_type(x1, y1, z1);
            this->value[2] = col_type(x2, y2, z2);
        }

        tmat3x3
                (
                        col_type const &v0,
                        col_type const &v1,
                        col_type const &v2
                ) {
            this->value[0] = v0;
            this->value[1] = v1;
            this->value[2] = v2;
        }

        template<typename U>
        tmat3x3(U const &s) {
            value_type const Zero(0);
            this->value[0] = tvec3<T>(value_type(s), Zero, Zero);
            this->value[1] = tvec3<T>(Zero, value_type(s), Zero);
            this->value[2] = tvec3<T>(Zero, Zero, value_type(s));
        }

        template<
                typename X1, typename Y1, typename Z1,
                typename X2, typename Y2, typename Z2,
                typename X3, typename Y3, typename Z3>
        tmat3x3
                (
                        X1 const &x1, Y1 const &y1, Z1 const &z1,
                        X2 const &x2, Y2 const &y2, Z2 const &z2,
                        X3 const &x3, Y3 const &y3, Z3 const &z3
                ) {
            this->value[0] = col_type(value_type(x1), value_type(y1), value_type(z1));
            this->value[1] = col_type(value_type(x2), value_type(y2), value_type(z2));
            this->value[2] = col_type(value_type(x3), value_type(y3), value_type(z3));
        }

        template<typename V1, typename V2, typename V3>
        tmat3x3
                (
                        tvec3<V1> const &v1,
                        tvec3<V2> const &v2,
                        tvec3<V3> const &v3
                ) {
            this->value[0] = col_type(v1);
            this->value[1] = col_type(v2);
            this->value[2] = col_type(v3);
        }

        template<typename U>
        tmat3x3(tmat3x3<U> const &m) {
            this->value[0] = col_type(m[0]);
            this->value[1] = col_type(m[1]);
            this->value[2] = col_type(m[2]);
        }

        tmat3x3<T> _transpose() const {
            return tmat3x3<T>(
                    this->value[0][0], this->value[1][0], this->value[2][0],
                    this->value[0][1], this->value[1][1], this->value[2][1],
                    this->value[0][2], this->value[1][2], this->value[2][2]
            );
        }

        tmat3x3<T> _inverse() const {
            T S00 = value[0][0];
            T S01 = value[0][1];
            T S02 = value[0][2];

            T S10 = value[1][0];
            T S11 = value[1][1];
            T S12 = value[1][2];

            T S20 = value[2][0];
            T S21 = value[2][1];
            T S22 = value[2][2];

            tmat3x3 <T> Inverse(
                    S11 * S22 - S21 * S12,
                    S12 * S20 - S22 * S10,
                    S10 * S21 - S20 * S11,
                    S02 * S21 - S01 * S22,
                    S00 * S22 - S02 * S20,
                    S01 * S20 - S00 * S21,
                    S12 * S01 - S11 * S02,
                    S10 * S02 - S12 * S00,
                    S11 * S00 - S10 * S01);

            T Determinant = S00 * (S11 * S22 - S21 * S12)
                            - S10 * (S01 * S22 - S21 * S02)
                            + S20 * (S01 * S12 - S11 * S02);

            Inverse /= Determinant;
            return Inverse;
        }

        col_type &operator[](size_type i) {
            assert(i < this->length());
            return this->value[i];
        }

        col_type const &operator[](size_type i) const {
            assert(i < this->length());
            return this->value[i];
        }

        tmat3x3<T> &operator=(tmat3x3<T> const &m) {
            this->value[0] = m[0];
            this->value[1] = m[1];
            this->value[2] = m[2];
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator=(tmat3x3<U> const &m) {
            this->value[0] = m[0];
            this->value[1] = m[1];
            this->value[2] = m[2];
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator+=(U const &s) {
            this->value[0] += s;
            this->value[1] += s;
            this->value[2] += s;
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator+=(tmat3x3<U> const &m) {
            this->value[0] += m[0];
            this->value[1] += m[1];
            this->value[2] += m[2];
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator-=(U const &s) {
            this->value[0] -= s;
            this->value[1] -= s;
            this->value[2] -= s;
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator-=(tmat3x3<U> const &m) {
            this->value[0] -= m[0];
            this->value[1] -= m[1];
            this->value[2] -= m[2];
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator*=(U const &s) {
            this->value[0] *= s;
            this->value[1] *= s;
            this->value[2] *= s;
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator*=(tmat3x3<U> const &m) {
            return (*this = *this * m);
        }

        template<typename U>
        tmat3x3<T> &operator/=(U const &s) {
            this->value[0] /= s;
            this->value[1] /= s;
            this->value[2] /= s;
            return *this;
        }

        template<typename U>
        tmat3x3<T> &operator/=(tmat3x3<U> const &m) {
            return (*this = *this / m);
        }

        tmat3x3<T> &operator++() {
            ++this->value[0];
            ++this->value[1];
            ++this->value[2];
            return *this;
        }

        tmat3x3<T> &operator--() {
            --this->value[0];
            --this->value[1];
            --this->value[2];
            return *this;
        }
    };

    template<typename T>
    tmat3x3<T> operator+(tmat3x3<T> const &m, T const &s) {
        return tmat3x3<T>(
                m[0] + s,
                m[1] + s,
                m[2] + s);
    }

    template<typename T>
    tmat3x3<T> operator+(T const &s, tmat3x3<T> const &m) {
        return tmat3x3<T>(
                m[0] + s,
                m[1] + s,
                m[2] + s);
    }

    template<typename T>
    tmat3x3<T> operator+(tmat3x3<T> const &m1, tmat3x3<T> const &m2) {
        return tmat3x3<T>(
                m1[0] + m2[0],
                m1[1] + m2[1],
                m1[2] + m2[2]);
    }

    template<typename T>
    tmat3x3<T> operator-(tmat3x3<T> const &m, T const &s) {
        return tmat3x3<T>(
                m[0] - s,
                m[1] - s,
                m[2] - s);
    }

    template<typename T>
    tmat3x3<T> operator-(T const &s, tmat3x3<T> const &m) {
        return tmat3x3<T>(
                s - m[0],
                s - m[1],
                s - m[2]);
    }

    template<typename T>
    tmat3x3<T> operator-(tmat3x3<T> const &m1, tmat3x3<T> const &m2) {
        return tmat3x3<T>(
                m1[0] - m2[0],
                m1[1] - m2[1],
                m1[2] - m2[2]);
    }

    template<typename T>
    tmat3x3<T> operator*(tmat3x3<T> const &m, T const &s) {
        return tmat3x3<T>(
                m[0] * s,
                m[1] * s,
                m[2] * s);
    }

    template<typename T>
    tmat3x3<T> operator*(T const &s, tmat3x3<T> const &m) {
        return tmat3x3<T>(
                m[0] * s,
                m[1] * s,
                m[2] * s);
    }

    template<typename T>
    tvec3<T> operator*(tmat3x3<T> const &m, tvec3<T> const &v) {
        return tvec3<T>(
                m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
                m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
                m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);
    }

    template<typename T>
    tvec3<T> operator*(tvec3<T> const &v, tmat3x3<T> const &m) {
        return tvec3<T>(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
    }

    template<typename T>
    tmat3x3<T> operator*(tmat3x3<T> const &m1, tmat3x3<T> const &m2) {
        T const srcA00 = m1[0][0];
        T const srcA01 = m1[0][1];
        T const srcA02 = m1[0][2];
        T const srcA10 = m1[1][0];
        T const srcA11 = m1[1][1];
        T const srcA12 = m1[1][2];
        T const srcA20 = m1[2][0];
        T const srcA21 = m1[2][1];
        T const srcA22 = m1[2][2];

        T const srcB00 = m2[0][0];
        T const srcB01 = m2[0][1];
        T const srcB02 = m2[0][2];
        T const srcB10 = m2[1][0];
        T const srcB11 = m2[1][1];
        T const srcB12 = m2[1][2];
        T const srcB20 = m2[2][0];
        T const srcB21 = m2[2][1];
        T const srcB22 = m2[2][2];

        tmat3x3 <T> res;
        res[0][0] = srcA00 * srcB00 + srcA10 * srcB01 + srcA20 * srcB02;
        res[0][1] = srcA01 * srcB00 + srcA11 * srcB01 + srcA21 * srcB02;
        res[0][2] = srcA02 * srcB00 + srcA12 * srcB01 + srcA22 * srcB02;
        res[1][0] = srcA00 * srcB10 + srcA10 * srcB11 + srcA20 * srcB12;
        res[1][1] = srcA01 * srcB10 + srcA11 * srcB11 + srcA21 * srcB12;
        res[1][2] = srcA02 * srcB10 + srcA12 * srcB11 + srcA22 * srcB12;
        res[2][0] = srcA00 * srcB20 + srcA10 * srcB21 + srcA20 * srcB22;
        res[2][1] = srcA01 * srcB20 + srcA11 * srcB21 + srcA21 * srcB22;
        res[2][2] = srcA02 * srcB20 + srcA12 * srcB21 + srcA22 * srcB22;
        return res;
    }

    template<typename T>
    tmat3x3<T> operator/(tmat3x3<T> const &m, T const &s) {
        return tmat3x3<T>(
                m[0] / s,
                m[1] / s,
                m[2] / s);
    }

    template<typename T>
    tmat3x3<T> operator/(T const &s, tmat3x3<T> const &m) {
        return tmat3x3<T>(
                s / m[0],
                s / m[1],
                s / m[2]
        );
    }

    template<typename T>
    tvec3<T> operator/(tmat3x3<T> const &m, tvec3<T> const &v) {
        return m._inverse() * v;
    }

    template<typename T>
    tvec3<T> operator/(tvec3<T> const &v, tmat3x3<T> const &m) {
        return v * m._inverse();
    }

    template<typename T>
    tmat3x3<T> operator/(tmat3x3<T> const &m1, tmat3x3<T> const &m2) {
        return m1 * m2._inverse();
    }

    template<typename T>
    tmat3x3<T> const operator-(tmat3x3<T> const &m) {
        return tmat3x3<T>(
                -m[0],
                -m[1],
                -m[2]);
    }

    template<typename T>
    tmat3x3<T> const operator++(tmat3x3<T> const &m, int) {
        return tmat3x3<T>(
                m[0] + T(1),
                m[1] + T(1),
                m[2] + T(1));
    }

    template<typename T>
    tmat3x3<T> const operator--(tmat3x3<T> const &m, int) {
        return tmat3x3<T>(
                m[0] - T(1),
                m[1] - T(1),
                m[2] - T(1));
    }

    template<typename T>
    bool operator==(tmat3x3<T> const &m1, tmat3x3<T> const &m2) {
        return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]);
    }

    template<typename T>
    bool operator!=(tmat3x3<T> const &m1, tmat3x3<T> const &m2) {
        return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]);
    }


    template<typename T>
    struct tmat4x4 {
        typedef T value_type;
        typedef std::size_t size_type;
        typedef tvec4<T> col_type;
        typedef tvec4<T> row_type;
        typedef tmat4x4<T> type;
        typedef tmat4x4<T> transpose_type;


    public:
        tmat4x4<T> inverse() const {
            value_type subFactor00 = this->value[2][2] * this->value[3][3] - this->value[3][2] * this->value[2][3];
            value_type subFactor01 = this->value[2][1] * this->value[3][3] - this->value[3][1] * this->value[2][3];
            value_type subFactor02 = this->value[2][1] * this->value[3][2] - this->value[3][1] * this->value[2][2];
            value_type subFactor03 = this->value[2][0] * this->value[3][3] - this->value[3][0] * this->value[2][3];
            value_type subFactor04 = this->value[2][0] * this->value[3][2] - this->value[3][0] * this->value[2][2];
            value_type subFactor05 = this->value[2][0] * this->value[3][1] - this->value[3][0] * this->value[2][1];
            value_type subFactor06 = this->value[1][2] * this->value[3][3] - this->value[3][2] * this->value[1][3];
            value_type subFactor07 = this->value[1][1] * this->value[3][3] - this->value[3][1] * this->value[1][3];
            value_type subFactor08 = this->value[1][1] * this->value[3][2] - this->value[3][1] * this->value[1][2];
            value_type subFactor09 = this->value[1][0] * this->value[3][3] - this->value[3][0] * this->value[1][3];
            value_type subFactor10 = this->value[1][0] * this->value[3][2] - this->value[3][0] * this->value[1][2];
            value_type subFactor11 = this->value[1][1] * this->value[3][3] - this->value[3][1] * this->value[1][3];
            value_type SubFactor12 = this->value[1][0] * this->value[3][1] - this->value[3][0] * this->value[1][1];
            value_type subFactor13 = this->value[1][2] * this->value[2][3] - this->value[2][2] * this->value[1][3];
            value_type subFactor14 = this->value[1][1] * this->value[2][3] - this->value[2][1] * this->value[1][3];
            value_type subFactor15 = this->value[1][1] * this->value[2][2] - this->value[2][1] * this->value[1][2];
            value_type subFactor16 = this->value[1][0] * this->value[2][3] - this->value[2][0] * this->value[1][3];
            value_type subFactor17 = this->value[1][0] * this->value[2][2] - this->value[2][0] * this->value[1][2];
            value_type subFactor18 = this->value[1][0] * this->value[2][1] - this->value[2][0] * this->value[1][1];

            tmat4x4 <T> res(
                    +this->value[1][1] * subFactor00 - this->value[1][2] * subFactor01 +
                    this->value[1][3] * subFactor02,
                    -this->value[1][0] * subFactor00 + this->value[1][2] * subFactor03 -
                    this->value[1][3] * subFactor04,
                    +this->value[1][0] * subFactor01 - this->value[1][1] * subFactor03 +
                    this->value[1][3] * subFactor05,
                    -this->value[1][0] * subFactor02 + this->value[1][1] * subFactor04 -
                    this->value[1][2] * subFactor05,

                    -this->value[0][1] * subFactor00 + this->value[0][2] * subFactor01 -
                    this->value[0][3] * subFactor02,
                    +this->value[0][0] * subFactor00 - this->value[0][2] * subFactor03 +
                    this->value[0][3] * subFactor04,
                    -this->value[0][0] * subFactor01 + this->value[0][1] * subFactor03 -
                    this->value[0][3] * subFactor05,
                    +this->value[0][0] * subFactor02 - this->value[0][1] * subFactor04 +
                    this->value[0][2] * subFactor05,

                    +this->value[0][1] * subFactor06 - this->value[0][2] * subFactor07 +
                    this->value[0][3] * subFactor08,
                    -this->value[0][0] * subFactor06 + this->value[0][2] * subFactor09 -
                    this->value[0][3] * subFactor10,
                    +this->value[0][0] * subFactor11 - this->value[0][1] * subFactor09 +
                    this->value[0][3] * SubFactor12,
                    -this->value[0][0] * subFactor08 + this->value[0][1] * subFactor10 -
                    this->value[0][2] * SubFactor12,

                    -this->value[0][1] * subFactor13 + this->value[0][2] * subFactor14 -
                    this->value[0][3] * subFactor15,
                    +this->value[0][0] * subFactor13 - this->value[0][2] * subFactor16 +
                    this->value[0][3] * subFactor17,
                    -this->value[0][0] * subFactor14 + this->value[0][1] * subFactor16 -
                    this->value[0][3] * subFactor18,
                    +this->value[0][0] * subFactor15 - this->value[0][1] * subFactor17 +
                    this->value[0][2] * subFactor18);

            value_type determinant =
                    +this->value[0][0] * res[0][0]
                    + this->value[0][1] * res[1][0]
                    + this->value[0][2] * res[2][0]
                    + this->value[0][3] * res[3][0];

            res /= determinant;
            return res;
        }


    private:
        col_type value[4];
    public:

        size_type length() const {
            return 16;
        }

        size_type col_size() {
            return 4;
        }

        size_type row_size() {
            return 4;
        }

        void identify() {
            this->value[0] = col_type(1, 0, 0, 0);
            this->value[1] = col_type(0, 1, 0, 0);
            this->value[2] = col_type(0, 0, 1, 0);
            this->value[3] = col_type(0, 0, 0, 1);
        }

        col_type &operator[](size_type i) {
            assert(i < this->length());
            return this->value[i];
        }

        col_type const &operator[](size_type i) const {
            assert(i < this->length());
            return this->value[i];
        }

        tmat4x4(tmat4x4<T> const &m) {
            this->value[0] = m.value[0];
            this->value[1] = m.value[1];
            this->value[2] = m.value[2];
            this->value[3] = m.value[3];
        }

        tmat4x4(tmat3x3<T> const &m) {
            this->value[0] = col_type(m[0], value_type(0));
            this->value[1] = col_type(m[1], value_type(0));
            this->value[2] = col_type(m[2], value_type(0));
            this->value[3] = col_type(value_type(0), value_type(0), value_type(0), value_type(1));
        }

        tmat4x4() {
        }

        tmat4x4(value_type s) {
            value_type const Zero(0);
            this->value[0] = col_type(s, Zero, Zero, Zero);
            this->value[1] = col_type(Zero, s, Zero, Zero);
            this->value[2] = col_type(Zero, Zero, s, Zero);
            this->value[3] = col_type(Zero, Zero, Zero, s);
        }

        tmat4x4
                (
                        value_type const &x0, value_type const &y0, value_type const &z0, value_type const &w0,
                        value_type const &x1, value_type const &y1, value_type const &z1, value_type const &w1,
                        value_type const &x2, value_type const &y2, value_type const &z2, value_type const &w2,
                        value_type const &x3, value_type const &y3, value_type const &z3, value_type const &w3
                ) {
            this->value[0] = col_type(x0, y0, z0, w0);
            this->value[1] = col_type(x1, y1, z1, w1);
            this->value[2] = col_type(x2, y2, z2, w2);
            this->value[3] = col_type(x3, y3, z3, w3);
        }

        tmat4x4
                (
                        col_type const &v0,
                        col_type const &v1,
                        col_type const &v2,
                        col_type const &v3
                ) {
            this->value[0] = v0;
            this->value[1] = v1;
            this->value[2] = v2;
            this->value[3] = v3;
        }

        template<typename U>
        tmat4x4(tmat4x4<U> const &m) {
            this->value[0] = col_type(m[0]);
            this->value[1] = col_type(m[1]);
            this->value[2] = col_type(m[2]);
            this->value[3] = col_type(m[3]);
        }

        template<typename U>
        tmat4x4(U const &s) {
            value_type const Zero(0);
            this->value[0] = tvec4<T>(value_type(s), Zero, Zero, Zero);
            this->value[1] = tvec4<T>(Zero, value_type(s), Zero, Zero);
            this->value[2] = tvec4<T>(Zero, Zero, value_type(s), Zero);
            this->value[3] = tvec4<T>(Zero, Zero, Zero, value_type(s));
        }

        template<
                typename X1, typename Y1, typename Z1, typename W1,
                typename X2, typename Y2, typename Z2, typename W2,
                typename X3, typename Y3, typename Z3, typename W3,
                typename X4, typename Y4, typename Z4, typename W4>
        tmat4x4
                (
                        X1 const &x1, Y1 const &y1, Z1 const &z1, W1 const &w1,
                        X2 const &x2, Y2 const &y2, Z2 const &z2, W2 const &w2,
                        X3 const &x3, Y3 const &y3, Z3 const &z3, W3 const &w3,
                        X4 const &x4, Y4 const &y4, Z4 const &z4, W4 const &w4
                ) {
            this->value[0] = col_type(value_type(x1), value_type(y1), value_type(z1), value_type(w1));
            this->value[1] = col_type(value_type(x2), value_type(y2), value_type(z2), value_type(w2));
            this->value[2] = col_type(value_type(x3), value_type(y3), value_type(z3), value_type(w3));
            this->value[3] = col_type(value_type(x4), value_type(y4), value_type(z4), value_type(w4));
        }

        template<typename V1, typename V2, typename V3, typename V4>
        tmat4x4
                (
                        tvec4<V1> const &v1,
                        tvec4<V2> const &v2,
                        tvec4<V3> const &v3,
                        tvec4<V4> const &v4
                ) {
            this->value[0] = col_type(v1);
            this->value[1] = col_type(v2);
            this->value[2] = col_type(v3);
            this->value[3] = col_type(v4);
        }

        T const *data() const {
            return &this->value[0][0];
        }

        tmat4x4<T> &operator=(tmat4x4<T> const &m) {
            this->value[0] = m[0];
            this->value[1] = m[1];
            this->value[2] = m[2];
            this->value[3] = m[3];
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator=(tmat4x4<U> const &m) {
            this->value[0] = m[0];
            this->value[1] = m[1];
            this->value[2] = m[2];
            this->value[3] = m[3];
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator+=(U const &s) {
            this->value[0] += s;
            this->value[1] += s;
            this->value[2] += s;
            this->value[3] += s;
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator+=(tmat4x4<U> const &m) {
            this->value[0] += m[0];
            this->value[1] += m[1];
            this->value[2] += m[2];
            this->value[3] += m[3];
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator-=(U const &s) {
            this->value[0] -= s;
            this->value[1] -= s;
            this->value[2] -= s;
            this->value[3] -= s;
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator-=(tmat4x4<U> const &m) {
            this->value[0] -= m[0];
            this->value[1] -= m[1];
            this->value[2] -= m[2];
            this->value[3] -= m[3];
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator*=(U const &s) {
            this->value[0] *= s;
            this->value[1] *= s;
            this->value[2] *= s;
            this->value[3] *= s;
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator*=(tmat4x4<U> const &m) {
            return (*this = *this * m);
        }

        template<typename U>
        tmat4x4<T> &operator/=(U const &s) {
            this->value[0] /= s;
            this->value[1] /= s;
            this->value[2] /= s;
            this->value[3] /= s;
            return *this;
        }

        template<typename U>
        tmat4x4<T> &operator/=(tmat4x4<U> const &m) {
            return (*this = *this / m);
        }

        tmat4x4<T> &operator++() {
            ++this->value[0];
            ++this->value[1];
            ++this->value[2];
            ++this->value[3];
            return *this;
        }

        tmat4x4<T> &operator--() {
            --this->value[0];
            --this->value[1];
            --this->value[2];
            --this->value[3];
            return *this;
        }

        tmat4x4<T> &translate(value_type x, value_type y, value_type z) {
            this->value[0] = col_type(1, 0, 0, 0);
            this->value[1] = col_type(0, 1, 0, 0);
            this->value[2] = col_type(0, 0, 1, 0);
            this->value[3] = col_type(x, y, z, 1);
            return *this;
        }

        template<typename U>
        tmat4x4<T> &translate(U x, U y, U z) {
            this->value[0] = col_type(1, 0, 0, 0);
            this->value[1] = col_type(0, 1, 0, 0);
            this->value[2] = col_type(0, 0, 1, 0);
            this->value[3] = col_type(T(x), T(y), T(z), 1);
            return *this;
        }

        tmat4x4<T> &translate(tvec3<T> const &pos) {
            this->value[0] = col_type(1, 0, 0, 0);
            this->value[1] = col_type(0, 1, 0, 0);
            this->value[2] = col_type(0, 0, 1, 0);
            this->value[3] = col_type(pos.x, pos.y, pos.z, 1);
            return *this;
        }

        template<typename U>
        tmat4x4<T> &translate(tvec3<U> const &pos) {
            this->value[0] = col_type(1, 0, 0, 0);
            this->value[1] = col_type(0, 1, 0, 0);
            this->value[2] = col_type(0, 0, 1, 0);
            this->value[3] = col_type(T(pos.x), T(pos.y), T(pos.z), 1);
            return *this;
        }

        tmat4x4<T> &rotate(value_type angle, tvec3<T> const &v) {
            T a = DEG2RAD(angle);
            T c = cos(a);
            T s = sin(a);

            tvec3 <T> axis = normalize(v);

            tvec3 <T> temp = (T(1) - c) * axis;

            tmat4x4 <T> res;
            this->value[0][0] = c + temp[0] * axis[0];
            this->value[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
            this->value[0][2] = 0 + temp[0] * axis[2] - s * axis[1];
            this->value[0][3] = 0;

            this->value[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
            this->value[1][1] = c + temp[1] * axis[1];
            this->value[1][2] = 0 + temp[1] * axis[2] + s * axis[0];
            this->value[1][3] = 0;

            this->value[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
            this->value[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
            this->value[2][2] = c + temp[2] * axis[2];
            this->value[2][3] = 0;

            this->value[3][0] = 0;
            this->value[3][1] = 0;
            this->value[3][2] = 0;
            this->value[3][3] = 1;
            return *this;
        }

        tmat4x4<T> &rotateX(value_type angle) {
            T a = DEG2RAD(angle);
            T c = cos(a);
            T s = sin(a);

            this->value[0] = col_type(1, 0, 0, 0);
            this->value[1] = col_type(0, c, s, 0);
            this->value[2] = col_type(0, -s, c, 0);
            this->value[3] = col_type(0, 0, 0, 1);

            return *this;
        }

        template<typename U>
        tmat4x4<T> &rotateX(U angle) {
            T a = DEG2RAD(angle);
            T c = cos(a);
            T s = sin(a);

            this->value[0] = col_type(1, 0, 0, 0);
            this->value[1] = col_type(0, c, s, 0);
            this->value[2] = col_type(0, -s, c, 0);
            this->value[3] = col_type(0, 0, 0, 1);

            return *this;
        }

        tmat4x4<T> &rotateY(value_type angle) {
            T a = DEG2RAD(angle);
            T c = cos(a);
            T s = sin(a);

            this->value[0] = col_type(c, 0, -s, 0);
            this->value[1] = col_type(0, 1, 0, 0);
            this->value[2] = col_type(s, 0, c, 0);
            this->value[3] = col_type(0, 0, 0, 1);
            return *this;

        }

        template<typename U>
        tmat4x4<T> &rotateY(U angle) {
            T a = DEG2RAD(angle);
            T c = cos(a);
            T s = sin(a);

            this->value[0] = col_type(c, 0, -s, 0);
            this->value[1] = col_type(0, 1, 0, 0);
            this->value[2] = col_type(s, 0, c, 0);
            this->value[3] = col_type(0, 0, 0, 1);
            return *this;

        }

        tmat4x4<T> &rotateZ(value_type angle) {
            T a = T(DEG2RAD(angle));
            T c = cos(a);
            T s = sin(a);

            this->value[0] = col_type(c, s, 0, 0);
            this->value[1] = col_type(-s, c, 0, 0);
            this->value[2] = col_type(0, 0, 1, 0);
            this->value[3] = col_type(0, 0, 0, 1);
            return *this;
        }

        template<typename U>
        tmat4x4<T> &rotateZ(U angle) {
            T a = DEG2RAD(angle);

            T c = cos(a);
            T s = sin(a);

            this->value[0] = col_type(c, s, 0, 0);
            this->value[1] = col_type(-s, c, 0, 0);
            this->value[2] = col_type(0, 0, 1, 0);
            this->value[3] = col_type(0, 0, 0, 1);
            return *this;
        }


        tmat4x4<T> rotateXY(T angleX, T angleY) {
            T cosX = cos(DEG2RAD(angleX));
            T sinX = sin(DEG2RAD(angleX));
            T cosY = cos(DEG2RAD(angleY));
            T sinY = sin(DEG2RAD(angleY));


            this->value[0] = col_type(cosY, -sinX * sinY, cosX * sinY, 0);
            this->value[1] = col_type(0, cosX, sinX, 0);
            this->value[2] = col_type(-sinY, -sinX * cosY, cosX * cosY, 0);
            this->value[3] = col_type(0, 0, 0, 1);
            return *this;
        }

        tmat4x4<T> rotateYX(T angleX, T angleY) {
            T cosX = cos(DEG2RAD(angleX));
            T sinX = sin(DEG2RAD(angleX));
            T cosY = cos(DEG2RAD(angleY));
            T sinY = sin(DEG2RAD(angleY));


            this->value[0] = col_type(cosY, 0, sinY, 0);
            this->value[1] = col_type(-sinX * sinY, cosX, sinX * cosY, 0);
            this->value[2] = col_type(-cosX * sinY, -sinX, cosX * cosY, 0);
            this->value[3] = col_type(0, 0, 0, 1);

            return *this;
        }

        tmat4x4<T> rotateYXZ(T yaw, T pitch, T roll) {
            T tmp_ch = cos(DEG2RAD(yaw));
            T tmp_sh = sin(DEG2RAD(yaw));
            T tmp_cp = cos(DEG2RAD(pitch));
            T tmp_sp = sin(DEG2RAD(pitch));
            T tmp_cb = cos(DEG2RAD(roll));
            T tmp_sb = sin(DEG2RAD(roll));

            tmat4x4 <T> Result;
            this->value[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
            this->value[0][1] = tmp_sb * tmp_cp;
            this->value[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
            this->value[0][3] = T(0);
            this->value[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
            this->value[1][1] = tmp_cb * tmp_cp;
            this->value[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
            this->value[1][3] = T(0);
            this->value[2][0] = tmp_sh * tmp_cp;
            this->value[2][1] = -tmp_sp;
            this->value[2][2] = tmp_ch * tmp_cp;
            this->value[2][3] = T(0);
            this->value[3][0] = T(0);
            this->value[3][1] = T(0);
            this->value[3][2] = T(0);
            this->value[3][3] = T(1);

            return *this;
        }

        tmat4x4<T> yawPitchRoll(T yaw, T pitch, T roll) {
            T tmp_ch = cos(DEG2RAD(yaw));
            T tmp_sh = sin(DEG2RAD(yaw));
            T tmp_cp = cos(DEG2RAD(pitch));
            T tmp_sp = sin(DEG2RAD(pitch));
            T tmp_cb = cos(DEG2RAD(roll));
            T tmp_sb = sin(DEG2RAD(roll));

            this->value[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
            this->value[0][1] = tmp_sb * tmp_cp;
            this->value[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
            this->value[0][3] = T(0);
            this->value[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
            this->value[1][1] = tmp_cb * tmp_cp;
            this->value[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
            this->value[1][3] = T(0);
            this->value[2][0] = tmp_sh * tmp_cp;
            this->value[2][1] = -tmp_sp;
            this->value[2][2] = tmp_ch * tmp_cp;
            this->value[2][3] = T(0);
            this->value[3][0] = T(0);
            this->value[3][1] = T(0);
            this->value[3][2] = T(0);
            this->value[3][3] = T(1);

            return *this;
        }

        tmat4x4<T> &scale(tvec3<T> const &s) {
            this->value[0] = col_type(s[0], 0, 0, 0);
            this->value[1] = col_type(0, s[1], 0, 0);
            this->value[2] = col_type(0, 0, s[2], 0);
            this->value[3] = col_type(0, 0, 0, 1);

            return *this;
        }

        tmat4x4<T> &scale(value_type x, value_type y, value_type z) {
            this->value[0] = col_type(x, 0, 0, 0);
            this->value[1] = col_type(0, y, 0, 0);
            this->value[2] = col_type(0, 0, z, 0);
            this->value[3] = col_type(0, 0, 0, 1);

            return *this;
        }

        template<typename U>
        tmat4x4<T> &scale(U x, U y, U z) {
            this->value[0] = col_type(value_type(x), 0, 0, 0);
            this->value[1] = col_type(0, value_type(y), 0, 0);
            this->value[2] = col_type(0, 0, value_type(z), 0);
            this->value[3] = col_type(0, 0, 0, 1);

            return *this;
        }

        template<typename U, typename V, typename W>
        tmat4x4<T> &scale(U x, V y, W z) {
            this->value[0] = col_type(value_type(x), 0, 0, 0);
            this->value[1] = col_type(0, value_type(y), 0, 0);
            this->value[2] = col_type(0, 0, value_type(z), 0);
            this->value[3] = col_type(0, 0, 0, 1);
            return *this;
        }

        tmat4x4<T> transpose() const {
            return tmat4x4<T>(
                    this->value[0][0], this->value[1][0], this->value[2][0], this->value[3][0],
                    this->value[0][1], this->value[1][1], this->value[2][1], this->value[3][1],
                    this->value[0][2], this->value[1][2], this->value[2][2], this->value[3][2],
                    this->value[0][3], this->value[1][3], this->value[2][3], this->value[3][3]
            );
        }

        tmat4x4<T> extractMatrixRotation() const {
            return tmat4x4<T>(
                    this->value[0][0], this->value[0][1], this->value[0][2], 0.0,
                    this->value[1][0], this->value[1][1], this->value[1][2], 0.0,
                    this->value[2][0], this->value[2][1], this->value[2][2], 0.0,
                    0.0, 0.0, 0.0, 1.0
            );
        }
    };


    template<typename T>
    tmat4x4<T> rotateX(T angleX) {
        T cosX = cos(DEG2RAD(angleX));
        T sinX = sin(DEG2RAD(angleX));

        return tmat4x4<T>(
                T(1), T(0), T(0), T(0),
                T(0), cosX, sinX, T(0),
                T(0), -sinX, cosX, T(0),
                T(0), T(0), T(0), T(1));
    }

    template<typename T>
    tmat4x4<T> rotateY(T angleY) {
        T cosY = cos(DEG2RAD(angleY));
        T sinY = sin(DEG2RAD(angleY));

        return tmat4x4<T>(
                cosY, T(0), sinY, T(0),
                T(0), T(1), T(0), T(0),
                -sinY, T(0), cosY, T(0),
                T(0), T(0), T(0), T(1));
    }

    template<typename T>
    tmat4x4<T> rotateZ(T angleZ) {
        T cosZ = (T) cos(DEG2RAD(angleZ));
        T sinZ = (T) sin(DEG2RAD(angleZ));

        return tmat4x4<T>(
                cosZ, sinZ, T(0), T(0),
                -sinZ, cosZ, T(0), T(0),
                T(0), T(0), T(1), T(0),
                T(0), T(0), T(0), T(1));
    }

    template<typename T>
    tmat4x4<T> rotateXY(T angleX, T angleY) {
        T cosX = cos(DEG2RAD(angleX));
        T sinX = sin(DEG2RAD(angleX));
        T cosY = cos(DEG2RAD(angleY));
        T sinY = sin(DEG2RAD(angleY));

        return tmat4x4<T>(
                cosY, -sinX * sinY, cosX * sinY, T(0),
                T(0), cosX, sinX, T(0),
                -sinY, -sinX * cosY, cosX * cosY, T(0),
                T(0), T(0), T(0), T(1));
    }

    template<typename T>
    tmat4x4<T> rotateYX(T angleY, T angleX) {

        T cosX = cos(DEG2RAD(angleX));
        T sinX = sin(DEG2RAD(angleX));
        T cosY = cos(DEG2RAD(angleY));
        T sinY = sin(DEG2RAD(angleY));

        return tmat4x4<T>(
                cosY, T(0), sinY, T(0),
                -sinX * sinY, cosX, sinX * cosY, T(0),
                -cosX * sinY, -sinX, cosX * cosY, T(0),
                T(0), T(0), T(0), T(1));
    }

    template<typename T>
    tmat4x4<T> rotateXZ(T angleX, T angleZ) {
        return rotateX(angleX) * rotateZ(angleZ);
    }

    template<typename T>
    tmat4x4<T> rotateZX(T angleX, T angleZ) {
        return rotateZ(angleZ) * rotateX(angleX);
    }

    template<typename T>
    tmat4x4<T> rotateYXZ(T yaw, T pitch, T roll) {
        T tmp_ch = cos(DEG2RAD(yaw));
        T tmp_sh = sin(DEG2RAD(yaw));
        T tmp_cp = cos(DEG2RAD(pitch));
        T tmp_sp = sin(DEG2RAD(pitch));
        T tmp_cb = cos(DEG2RAD(roll));
        T tmp_sb = sin(DEG2RAD(roll));

        tmat4x4 <T> res;
        res[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
        res[0][1] = tmp_sb * tmp_cp;
        res[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
        res[0][3] = T(0);
        res[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
        res[1][1] = tmp_cb * tmp_cp;
        res[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
        res[1][3] = T(0);
        res[2][0] = tmp_sh * tmp_cp;
        res[2][1] = -tmp_sp;
        res[2][2] = tmp_ch * tmp_cp;
        res[2][3] = T(0);
        res[3][0] = T(0);
        res[3][1] = T(0);
        res[3][2] = T(0);
        res[3][3] = T(1);
        return res;
    }

    template<typename T>
    tmat4x4<T> yawPitchRoll(T yaw, T pitch, T roll) {
        T tmp_ch = cos(DEG2RAD(yaw));
        T tmp_sh = sin(DEG2RAD(yaw));
        T tmp_cp = cos(DEG2RAD(pitch));
        T tmp_sp = sin(DEG2RAD(pitch));
        T tmp_cb = cos(DEG2RAD(roll));
        T tmp_sb = sin(DEG2RAD(roll));


        tmat4x4 <T> res;
        res[0][0] = tmp_ch * tmp_cb + tmp_sh * tmp_sp * tmp_sb;
        res[0][1] = tmp_sb * tmp_cp;
        res[0][2] = -tmp_sh * tmp_cb + tmp_ch * tmp_sp * tmp_sb;
        res[0][3] = T(0);
        res[1][0] = -tmp_ch * tmp_sb + tmp_sh * tmp_sp * tmp_cb;
        res[1][1] = tmp_cb * tmp_cp;
        res[1][2] = tmp_sb * tmp_sh + tmp_ch * tmp_sp * tmp_cb;
        res[1][3] = T(0);
        res[2][0] = tmp_sh * tmp_cp;
        res[2][1] = -tmp_sp;
        res[2][2] = tmp_ch * tmp_cp;
        res[2][3] = T(0);
        res[3][0] = T(0);
        res[3][1] = T(0);
        res[3][2] = T(0);
        res[3][3] = T(1);
        return res;
    }

    template<typename T>
    void axisAngle
            (
                    tmat4x4<T> const &mat,
                    tvec3<T> &axis,
                    T &angle
            ) {
        T epsilon = (T) 0.01;
        T epsilon2 = (T) 0.1;

        if ((fabs(mat[1][0] - mat[0][1]) < epsilon) &&
            (fabs(mat[2][0] - mat[0][2]) < epsilon) &&
            (fabs(mat[2][1] - mat[1][2]) < epsilon)) {
            if ((fabs(mat[1][0] + mat[0][1]) < epsilon2) &&
                (fabs(mat[2][0] + mat[0][2]) < epsilon2) &&
                (fabs(mat[2][1] + mat[1][2]) < epsilon2) &&
                (fabs(mat[0][0] + mat[1][1] + mat[2][2] - (T) 3.0) < epsilon2)) {
                angle = (T) 0.0;
                axis.x = (T) 1.0;
                axis.y = (T) 0.0;
                axis.z = (T) 0.0;
                return;
            }
            angle = T(3.1415926535897932384626433832795);
            T xx = (mat[0][0] + (T) 1.0) / (T) 2.0;
            T yy = (mat[1][1] + (T) 1.0) / (T) 2.0;
            T zz = (mat[2][2] + (T) 1.0) / (T) 2.0;
            T xy = (mat[1][0] + mat[0][1]) / (T) 4.0;
            T xz = (mat[2][0] + mat[0][2]) / (T) 4.0;
            T yz = (mat[2][1] + mat[1][2]) / (T) 4.0;
            if ((xx > yy) && (xx > zz)) {
                if (xx < epsilon) {
                    axis.x = (T) 0.0;
                    axis.y = (T) 0.7071;
                    axis.z = (T) 0.7071;
                } else {
                    axis.x = sqrt(xx);
                    axis.y = xy / axis.x;
                    axis.z = xz / axis.x;
                }
            } else if (yy > zz) {
                if (yy < epsilon) {
                    axis.x = (T) 0.7071;
                    axis.y = (T) 0.0;
                    axis.z = (T) 0.7071;
                } else {
                    axis.y = sqrt(yy);
                    axis.x = xy / axis.y;
                    axis.z = yz / axis.y;
                }
            } else {
                if (zz < epsilon) {
                    axis.x = (T) 0.7071;
                    axis.y = (T) 0.7071;
                    axis.z = (T) 0.0;
                } else {
                    axis.z = sqrt(zz);
                    axis.x = xz / axis.z;
                    axis.y = yz / axis.z;
                }
            }
            return;
        }
        T s = sqrt(
                (mat[2][1] - mat[1][2]) * (mat[2][1] - mat[1][2]) + (mat[2][0] - mat[0][2]) * (mat[2][0] - mat[0][2]) +
                (mat[1][0] - mat[0][1]) * (mat[1][0] - mat[0][1]));
        if (abs(s) < T(0.001))
            s = (T) 1.0;
        angle = acos((mat[0][0] + mat[1][1] + mat[2][2] - (T) 1.0) / (T) 2.0);
        axis.x = (mat[1][2] - mat[2][1]) / s;
        axis.y = (mat[2][0] - mat[0][2]) / s;
        axis.z = (mat[0][1] - mat[1][0]) / s;
    }

    template<typename T>
    tmat4x4<T> axisAngleMatrix(tvec3<T> const &axis, T const angle) {
        T c = cos(angle);
        T s = sin(angle);
        T t = T(1) - c;
        tvec3 <T> n = normalize(axis);

        return tmat4x4<T>(
                t * n.x * n.x + c, t * n.x * n.y + n.z * s, t * n.x * n.z - n.y * s, T(0),
                t * n.x * n.y - n.z * s, t * n.y * n.y + c, t * n.y * n.z + n.x * s, T(0),
                t * n.x * n.z + n.y * s, t * n.y * n.z - n.x * s, t * n.z * n.z + c, T(0),
                T(0), T(0), T(0), T(1)
        );
    }

    template<typename T>
    tmat4x4<T> interpolate
            (
                    tmat4x4<T> const &m1,
                    tmat4x4<T> const &m2,
                    T const delta
            ) {
        tmat4x4 <T> m1rot = m1.extractMatrixRotation();
        tmat4x4 <T> dltRotation = m2 * m1rot.transpose();
        tvec3 <T> dltAxis;
        T dltAngle;
        axisAngle(dltRotation, dltAxis, dltAngle);
        tmat4x4 <T> out = axisAngleMatrix(dltAxis, dltAngle * delta) * m1rot;
        out[3][0] = m1[3][0] + delta * (m2[3][0] - m1[3][0]);
        out[3][1] = m1[3][1] + delta * (m2[3][1] - m1[3][1]);
        out[3][2] = m1[3][2] + delta * (m2[3][2] - m1[3][2]);
        return out;
    }

    template<typename T>
    tvec3<T> operator*(tvec3<T> const &v, tmat4x4<T> const &mat) {
        return tvec3<T>
                (
                        v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0] + 1 * mat[3][0],
                        v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1] + 1 * mat[3][1],
                        v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2] + 1 * mat[3][2]
                );
    }

    template<typename T>
    tmat4x4<T> operator+(tmat4x4<T> const &m, typename tmat4x4<T>::value_type s) {
        return tmat4x4<T>(
                m[0] + s,
                m[1] + s,
                m[2] + s,
                m[3] + s);
    }

    template<typename T>
    tmat4x4<T> operator+(typename tmat4x4<T>::value_type s, tmat4x4<T> const &m) {
        return tmat4x4<T>(
                m[0] + s,
                m[1] + s,
                m[2] + s,
                m[3] + s);
    }

    template<typename T>
    tmat4x4<T> operator+(tmat4x4<T> const &m1, tmat4x4<T> const &m2) {
        return tmat4x4<T>(
                m1[0] + m2[0],
                m1[1] + m2[1],
                m1[2] + m2[2],
                m1[3] + m2[3]);
    }

    template<typename T>
    tmat4x4<T> operator-(tmat4x4<T> const &m, typename tmat4x4<T>::value_type s) {
        return tmat4x4<T>(
                m[0] - s,
                m[1] - s,
                m[2] - s,
                m[3] - s);
    }

    template<typename T>
    tmat4x4<T> operator-(typename tmat4x4<T>::value_type s, tmat4x4<T> const &m) {
        return tmat4x4<T>(
                s - m[0],
                s - m[1],
                s - m[2],
                s - m[3]);
    }

    template<typename T>
    tmat4x4<T> operator-(tmat4x4<T> const &m1, tmat4x4<T> const &m2) {
        return tmat4x4<T>(
                m1[0] - m2[0],
                m1[1] - m2[1],
                m1[2] - m2[2],
                m1[3] - m2[3]);
    }

    template<typename T>
    tmat4x4<T> operator*(tmat4x4<T> const &m, typename tmat4x4<T>::value_type s) {
        return tmat4x4<T>(
                m[0] * s,
                m[1] * s,
                m[2] * s,
                m[3] * s);
    }

    template<typename T>
    tmat4x4<T> operator*(typename tmat4x4<T>::value_type s, tmat4x4<T> const &m) {
        return tmat4x4<T>(
                m[0] * s,
                m[1] * s,
                m[2] * s,
                m[3] * s);
    }

    template<typename T>
    typename tmat4x4<T>::col_type operator*(tmat4x4<T> const &m, typename tmat4x4<T>::row_type const &v) {
        return typename tmat4x4<T>::col_type(
                m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
                m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
                m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
                m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w);
    }

    template<typename T>
    typename tmat4x4<T>::row_type operator*(typename tmat4x4<T>::col_type const &v, tmat4x4<T> const &m) {
        return typename tmat4x4<T>::row_type(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
    }


    template<typename T>
    tmat4x4<T> operator*(tmat4x4<T> const &m1, tmat4x4<T> const &m2) {
        typename tmat4x4<T>::col_type const srcA0 = m1[0];
        typename tmat4x4<T>::col_type const srcA1 = m1[1];
        typename tmat4x4<T>::col_type const srcA2 = m1[2];
        typename tmat4x4<T>::col_type const srcA3 = m1[3];

        typename tmat4x4<T>::col_type const srcB0 = m2[0];
        typename tmat4x4<T>::col_type const srcB1 = m2[1];
        typename tmat4x4<T>::col_type const srcB2 = m2[2];
        typename tmat4x4<T>::col_type const srcB3 = m2[3];

        tmat4x4 <T> res;
        res[0] = srcA0 * srcB0[0] + srcA1 * srcB0[1] + srcA2 * srcB0[2] + srcA3 * srcB0[3];
        res[1] = srcA0 * srcB1[0] + srcA1 * srcB1[1] + srcA2 * srcB1[2] + srcA3 * srcB1[3];
        res[2] = srcA0 * srcB2[0] + srcA1 * srcB2[1] + srcA2 * srcB2[2] + srcA3 * srcB2[3];
        res[3] = srcA0 * srcB3[0] + srcA1 * srcB3[1] + srcA2 * srcB3[2] + srcA3 * srcB3[3];
        return res;
    }

    template<typename T>
    tmat4x4<T> operator/(tmat4x4<T> const &m, typename tmat4x4<T>::value_type s) {
        return tmat4x4<T>(
                m[0] / s,
                m[1] / s,
                m[2] / s,
                m[3] / s);
    }

    template<typename T>
    tmat4x4<T> operator/(typename tmat4x4<T>::value_type s, tmat4x4<T> const &m) {
        return tmat4x4<T>(
                s / m[0],
                s / m[1],
                s / m[2],
                s / m[3]);
    }

    template<typename T>
    typename tmat4x4<T>::col_type operator/(tmat4x4<T> const &m, typename tmat4x4<T>::row_type const &v) {
        return m.inverse() * v;
    }

    template<typename T>
    typename tmat4x4<T>::row_type operator/(typename tmat4x4<T>::col_type const &v, tmat4x4<T> const &m) {
        return v * m.inverse();
    }

    template<typename T>
    tmat4x4<T> operator/(tmat4x4<T> const &m1, tmat4x4<T> const &m2) {
        return m1 * m2.inverse();
    }

    template<typename T>
    tmat4x4<T> const operator-(tmat4x4<T> const &m) {
        return tmat4x4<T>(
                -m[0],
                -m[1],
                -m[2],
                -m[3]);
    }

    template<typename T>
    tmat4x4<T> const operator++(tmat4x4<T> const &m, int) {
        return tmat4x4<T>(
                m[0] + T(1),
                m[1] + T(1),
                m[2] + T(1),
                m[3] + T(1));
    }

    template<typename T>
    tmat4x4<T> const operator--(tmat4x4<T> const &m, int) {
        return tmat4x4<T>(
                m[0] - T(1),
                m[1] - T(1),
                m[2] - T(1),
                m[3] - T(1));
    }

    template<typename T>
    bool operator==(tmat4x4<T> const &m1, tmat4x4<T> const &m2) {
        return (m1[0] == m2[0]) && (m1[1] == m2[1]) && (m1[2] == m2[2]) && (m1[3] == m2[3]);
    }

    template<typename T>
    bool operator!=(tmat4x4<T> const &m1, tmat4x4<T> const &m2) {
        return (m1[0] != m2[0]) || (m1[1] != m2[1]) || (m1[2] != m2[2]) || (m1[3] != m2[3]);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //! 射线类
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename T>
    typename tvec2<T>::value_type length(tvec2<T> const &v) {
        typename tvec2<T>::value_type sqr = v.x * v.x + v.y * v.y;
        return sqrt(sqr);
    }

    template<typename T>
    typename tvec3<T>::value_type length(tvec3<T> const &v) {
        typename tvec3<T>::value_type sqr = v.x * v.x + v.y * v.y + v.z * v.z;
        return sqrt(sqr);
    }

    template<typename T>
    typename tvec4<T>::value_type length(tvec4<T> const &v) {
        typename tvec4<T>::value_type sqr = v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
        return sqrt(sqr);
    }

    template<typename T>
    typename tvec2<T>::value_type distance(tvec2<T> const &p0, tvec2<T> const &p1) {
        return length(p1 - p0);
    }

    template<typename T>
    typename tvec3<T>::value_type distance(tvec3<T> const &p0, tvec3<T> const &p1) {
        return length(p1 - p0);
    }

    template<typename T>
    typename tvec4<T>::value_type distance(tvec4<T> const &p0, tvec4<T> const &p1) {
        return length(p1 - p0);
    }

    template<typename T>
    typename tvec2<T>::value_type dot(tvec2<T> const &x, tvec2<T> const &y) {
        return x.x * y.x + x.y * y.y;
    }

    template<typename T>
    typename tvec3<T>::value_type dot(tvec3<T> const &x, tvec3<T> const &y) {
        return x.x * y.x + x.y * y.y + x.z * y.z;
    }

    template<typename T>
    typename tvec4<T>::value_type dot(tvec4<T> const &x, tvec4<T> const &y) {
        return x.x * y.x + x.y * y.y + x.z * y.z + x.w * y.w;
    }

    template<typename T>
    tvec3<T> cross(tvec3<T> const &x, tvec3<T> const &y) {
        return tvec3<T>
                (
                        x.y * y.z - y.y * x.z,
                        x.z * y.x - y.z * x.x,
                        x.x * y.y - y.x * x.y
                );
    }

    template<typename T>
    T inversesqrt(T x) {
        return T(1) / sqrt(x);
    }

    template<typename T>
    tvec2<T> normalize(tvec2<T> const &x) {
        typename tvec2<T>::value_type sqr = x.x * x.x + x.y * x.y;
        return x * inversesqrt(sqr);
    }

    template<typename T>
    tvec3<T> normalize(tvec3<T> const &x) {
        typename tvec3<T>::value_type sqr = x.x * x.x + x.y * x.y + x.z * x.z;
        return x * inversesqrt(sqr);
    }

    template<typename T>
    tvec4<T> normalize(tvec4<T> const &x) {
        typename tvec4<T>::value_type sqr = x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w;
        return x * inversesqrt(sqr);
    }


    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct tquat {
        typedef T value_type;
        typedef std::size_t size_type;

    public:
        value_type x;
        value_type y;
        value_type z;
        value_type w;

        size_type length() const {
            return 4;
        }

        tquat() :
                x(0),
                y(0),
                z(0),
                w(1) {}

        explicit tquat(value_type s, tvec3<T> const &v) :
                x(v.x),
                y(v.y),
                z(v.z),
                w(s) {
        }

        explicit tquat(tvec3<T> const &v, value_type s) :
                x(v.x),
                y(v.y),
                z(v.z),
                w(s) {
        }

        explicit tquat(value_type w, value_type x, value_type y, value_type z) :
                x(x),
                y(y),
                z(z),
                w(w) {}

        explicit tquat(tvec3<T> const &eulerAngle) {
            tvec3 <T> c = cos(eulerAngle * value_type(0.5));
            tvec3 <T> s = sin(eulerAngle * value_type(0.5));

            this->w = c.x * c.y * c.z + s.x * s.y * s.z;
            this->x = s.x * c.y * c.z - c.x * s.y * s.z;
            this->y = c.x * s.y * c.z + s.x * c.y * s.z;
            this->z = c.x * c.y * s.z - s.x * s.y * c.z;
        }

        explicit tquat(tmat3x3<T> const &m) {
            *this = quat_cast(m);
        }

        explicit tquat(tmat4x4<T> const &m) {
            *this = quat_cast(m);
        }

        value_type &operator[](int i) {
            return (&x)[i];
        }

        value_type const &operator[](int i) const {
            return (&x)[i];
        }

        tquat<T> &operator*=(value_type s) {
            this->w *= s;
            this->x *= s;
            this->y *= s;
            this->z *= s;
            return *this;
        }

        tquat<T> &operator=(const tquat<T> &right) {
            this->w = right.w;
            this->x = right.x;
            this->y = right.y;
            this->z = right.z;
            return *this;
        }

        tquat<T> &operator/=(value_type s) {
            this->w /= s;
            this->x /= s;
            this->y /= s;
            this->z /= s;
            return *this;
        }
    };

    template <typename T>
    tmat3x3<T> mat4_to_mat3(const tmat4x4<T> & m)
    {
        return  tmat3x3<T>(
                tvec3<T>(m[0][0],m[0][1],m[0][2])
                ,tvec3<T>(m[1][0],m[1][1],m[1][2])
                ,tvec3<T>(m[2][0],m[2][1],m[2][2]));
    }


    template<typename T>
    tmat4x4<T> makeTransform(tvec3<T> const &position, tvec3<T> const &scale, const tquat<T> &orientation) {
        tmat3x3 <T> rot3x3 = mat3_cast(orientation);

        return tmat4x4<T>
                (
                        scale.x * rot3x3[0][0], scale.x * rot3x3[0][1], scale.x * rot3x3[0][2], 0,
                        scale.y * rot3x3[1][0], scale.y * rot3x3[1][1], scale.y * rot3x3[1][2], 0,
                        scale.z * rot3x3[2][0], scale.z * rot3x3[2][1], scale.z * rot3x3[2][2], 0,
                        position.x, position.y, position.z, 1
                );
    }

    template<typename T>
    T dot(tquat<T> const &q1, tquat<T> const &q2) {
        return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    }

    template<typename T>
    tquat<T> cross(tquat<T> const &q1, tquat<T> const &q2) {
        return tquat<T>(
                q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
                q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
                q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
                q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x);
    }

    template<typename T>
    T length(tquat<T> const &q) {
        return sqrt(dot(q, q));
    }

    template<typename genType>
    genType mix(genType x, genType y, genType a) {
        return x + a * (y - x);
    }

    template<typename T>
    T epsilon() {
        return std::numeric_limits<T>::epsilon();
    }


    template<typename T>
    tquat<T> conjugate(tquat<T> const &q) {
        return tquat<T>(q.w, -q.x, -q.y, -q.z);
    }

    template<typename T>
    tquat<T> inverse(tquat<T> const &q) {
        return conjugate(q) / dot(q, q);
    }

    template<typename T>
    bool operator==(tquat<T> const &q1, tquat<T> const &q2) {
        return (q1.x == q2.x) && (q1.y == q2.y) && (q1.z == q2.z) && (q1.w == q2.w);
    }

    template<typename T>
    bool operator!=(tquat<T> const &q1, tquat<T> const &q2) {
        return (q1.x != q2.x) || (q1.y != q2.y) || (q1.z != q2.z) || (q1.w != q2.w);
    }

    template<typename T>
    tquat<T> operator-(tquat<T> const &q) {
        return tquat<T>(-q.w, -q.x, -q.y, -q.z);
    }

    template<typename T>
    tquat<T> operator+(tquat<T> const &q, tquat<T> const &p) {
        return tquat<T>(
                q.w + p.w,
                q.x + p.x,
                q.y + p.y,
                q.z + p.z
        );
    }

    template<typename T>
    tquat<T> operator*(tquat<T> const &q, tquat<T> const &p) {
        return tquat<T>(
                q.w * p.w - q.x * p.x - q.y * p.y - q.z * p.z,
                q.w * p.x + q.x * p.w + q.y * p.z - q.z * p.y,
                q.w * p.y + q.y * p.w + q.z * p.x - q.x * p.z,
                q.w * p.z + q.z * p.w + q.x * p.y - q.y * p.x
        );
    }

    template<typename T>
    tvec3<T> operator*(tquat<T> const &q, tvec3<T> const &v) {
        typename tquat<T>::value_type two(2);

        tvec3 <T> uv;
        tvec3 <T> uuv;
        tvec3 <T> quatVector(q.x, q.y, q.z);
        uv = cross(quatVector, v);
        uuv = cross(quatVector, uv);
        uv *= two * q.w;
        uuv *= two;
        return v + uv + uuv;
    }

    template<typename T>
    tvec3<T> operator*(tvec3<T> const &v, tquat<T> const &q) {
        return inverse(q) * v;
    }


    template<typename T>
    tquat<T> operator*(tquat<T> const &q, typename tquat<T>::value_type s) {
        return tquat<T>(q.w * s, q.x * s, q.y * s, q.z * s);
    }

    template<typename T>
    tquat<T> operator*(typename tquat<T>::value_type s, tquat<T> const &q) {
        return q * s;
    }

    template<typename T>
    tquat<T> operator/(tquat<T> const &q, typename tquat<T>::value_type s) {
        return tquat<T>(q.w / s, q.x / s, q.y / s, q.z / s);
    }


    template<typename T>
    tquat<T> mix(tquat<T> const &x, tquat<T> const &y, T const &a) {
        T cosTheta = dot(x, y);
        if (cosTheta > T(1) - epsilon<T>()) {
            return tquat<T>(
                    mix(x.w, y.w, a),
                    mix(x.x, y.x, a),
                    mix(x.y, y.y, a),
                    mix(x.z, y.z, a)
            );
        } else {
            // Essential Mathematics, page 467
            T angle = acos(cosTheta);
            return (sin((T(1) - a) * angle) * x + sin(a * angle) * y) / sin(angle);
        }
    }

    template<typename T>
    tquat<T> lerp(tquat<T> const &x, tquat<T> const &y, T a) {
        assert(a >= T(0));
        assert(a <= T(1));
        return x * (T(1) - a) + (y * a);
    }

    template<typename T>
    tquat<T> slerp(tquat<T> const &x, tquat<T> const &y, T a) {
        tquat <T> z = y;

        T cosTheta = dot(x, y);

        if (cosTheta < T(0)) {
            z = -y;
            cosTheta = -cosTheta;
        }
        if (cosTheta > T(1) - epsilon<T>()) {
            return tquat<T>
                    (
                            mix(x.w, z.w, a),
                            mix(x.x, z.x, a),
                            mix(x.y, z.y, a),
                            mix(x.z, z.z, a)
                    );
        } else {
            // Essential Mathematics, page 467
            T angle = acos(cosTheta);
            return (sin((T(1) - a) * angle) * x + sin(a * angle) * z) / sin(angle);
        }
    }

    template<typename T>
    tquat<T> rotate
            (
                    typename tquat<T>::value_type angle,
                    tvec3<T> const &axis
            ) {
        tvec3 <T> Tmp = axis;

        typename tquat<T>::value_type len = length(Tmp);
        if (abs(len - T(1)) > T(0.001f)) {
            T oneOverLen = T(1) / len;
            Tmp.x *= oneOverLen;
            Tmp.y *= oneOverLen;
            Tmp.z *= oneOverLen;
        }
        typename tquat<T>::value_type const AngleRad = (T) DEG2RAD(angle);
        typename tquat<T>::value_type const Sin = (T) sin(AngleRad * T(0.5));
        return tquat<T>((T) cos(AngleRad * T(0.5)), Tmp.x * Sin, Tmp.y * Sin, Tmp.z * Sin);
    }

    template<typename valType>
    valType roll(tquat<valType> const &q) {
        return atan2(valType(2) * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z) *
               valType(RAD2DEG);
    }

    template<typename valType>
    valType pitch(tquat<valType> const &q) {
        return ::atan2(valType(2) * (q.y * q.z + q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z) *
               valType(RAD2DEG);
    }

    template<typename valType>
    valType yaw(tquat<valType> const &q) {
        return ::asin(valType(-2) * (q.x * q.z - q.w * q.y)) * valType(RAD2DEG);
    }

    template<typename T>
    tvec3<T> eulerAngles(tquat<T> const &x) {
        return tvec3<T>(pitch(x), yaw(x), roll(x));
    }


    template<typename T>
    tmat3x3<T> mat3_cast(const tquat<T> &q) {

        return tmat3x3<T>
                (
                        1 - 2 * q.y * q.y - 2 * q.z * q.z, 2 * q.x * q.y + 2 * q.w * q.z, 2 * q.x * q.z - 2 * q.w * q.y,
                        2 * q.x * q.y - 2 * q.w * q.z, 1 - 2 * q.x * q.x - 2 * q.z * q.z, 2 * q.y * q.z + 2 * q.w * q.x,
                        2 * q.x * q.z + 2 * q.w * q.y, 2 * q.y * q.z - 2 * q.w * q.x, 1 - 2 * q.x * q.x - 2 * q.y * q.y
                );
    }

    template<typename T>
    tmat4x4<T> mat4_cast(tquat<T> const &q) {
        return tmat4x4<T>(mat3_cast(q));
    }

    template<typename T>
    tquat<T> quat_cast(tmat3x3<T> const &m) {
        typename tquat<T>::value_type fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
        typename tquat<T>::value_type fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
        typename tquat<T>::value_type fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
        typename tquat<T>::value_type fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

        int biggestIndex = 0;
        typename tquat<T>::value_type fourBiggestSquaredMinus1 = fourWSquaredMinus1;
        if (fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
            fourBiggestSquaredMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if (fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
            fourBiggestSquaredMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if (fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
            fourBiggestSquaredMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }

        typename tquat<T>::value_type biggestVal = sqrt(fourBiggestSquaredMinus1 + T(1)) * T(0.5);
        typename tquat<T>::value_type mult = T(0.25) / biggestVal;

        tquat <T> res;
        switch (biggestIndex) {
            case 0:
                res.w = biggestVal;
                res.x = (m[1][2] - m[2][1]) * mult;
                res.y = (m[2][0] - m[0][2]) * mult;
                res.z = (m[0][1] - m[1][0]) * mult;
                break;
            case 1:
                res.w = (m[1][2] - m[2][1]) * mult;
                res.x = biggestVal;
                res.y = (m[0][1] + m[1][0]) * mult;
                res.z = (m[2][0] + m[0][2]) * mult;
                break;
            case 2:
                res.w = (m[2][0] - m[0][2]) * mult;
                res.x = (m[0][1] + m[1][0]) * mult;
                res.y = biggestVal;
                res.z = (m[1][2] + m[2][1]) * mult;
                break;
            case 3:
                res.w = (m[0][1] - m[1][0]) * mult;
                res.x = (m[2][0] + m[0][2]) * mult;
                res.y = (m[1][2] + m[2][1]) * mult;
                res.z = biggestVal;
                break;

            default:
                assert(false);
                break;
        }
        return res;
    }

    template<typename T>
    tquat<T> quat_cast(tmat4x4<T> const &m4) {
        return quat_cast(tmat3x3<T>(m4[0][0], m4[0][1], m4[0][2],
                                    m4[1][0], m4[1][1], m4[1][2],
                                    m4[2][0], m4[2][1], m4[2][2]));
    }

    template<typename T>
    T angle(tquat<T> const &x) {
        return acos(x.w) * T(2) * T(RAD2DEG);
    }

    template<typename T>
    tvec3<T> axis(tquat<T> const &x) {
        T tmp1 = T(1) - x.w * x.w;
        if (tmp1 <= T(0)) {
            return tvec3<T>(0, 0, 1);
        }
        T tmp2 = T(1) / sqrt(tmp1);

        return tvec3<T>(x.x * tmp2, x.y * tmp2, x.z * tmp2);
    }


    template<typename valType>
    tquat<valType> angleAxis(valType angle, tvec3<valType> const &axis) {
        tquat <valType> result;

        valType a = (valType) (valType(DEG2RAD(angle)));
        valType s = sin(a * valType(0.5));

        result.w = cos(a * valType(0.5));
        result.x = axis.x * s;
        result.y = axis.y * s;
        result.z = axis.z * s;
        return result;
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    tmat4x4<T> translate(tmat4x4<T> const &m, tvec3<T> const &v) {
        tmat4x4 <T> res(m);
        res[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
        return res;
    }

    template<typename T>
    tmat4x4<T> rotate
            (
                    tmat4x4<T> const &m,
                    T const &angle,
                    tvec3<T> const &v
            ) {

        T a = DEG2RAD(angle);
        T c = cos(a);
        T s = sin(a);

        tvec3 <T> axis = normalize(v);

        tvec3 <T> temp = (T(1) - c) * axis;

        tmat4x4 <T> res;
        res[0][0] = c + temp[0] * axis[0];
        res[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
        res[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

        res[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
        res[1][1] = c + temp[1] * axis[1];
        res[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

        res[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
        res[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
        res[2][2] = c + temp[2] * axis[2];

        tmat4x4 <T> rot;

        rot[0] = m[0] * res[0][0] + m[1] * res[0][1] + m[2] * res[0][2];
        rot[1] = m[0] * res[1][0] + m[1] * res[1][1] + m[2] * res[1][2];
        rot[2] = m[0] * res[2][0] + m[1] * res[2][1] + m[2] * res[2][2];
        rot[3] = m[3];
        return rot;
    }

    template<typename T>
    tmat4x4<T> scale(tmat4x4<T> const &m, tvec3<T> const &v) {
        tmat4x4 <T> res;
        res[0] = m[0] * v[0];
        res[1] = m[1] * v[1];
        res[2] = m[2] * v[2];
        res[3] = m[3];
        return res;
    }

    template<typename T>
    tmat4x4<T> rotate_slow
            (
                    tmat4x4<T> const &m,
                    T const &angle,
                    tvec3<T> const &v
            ) {

        T const a = DEG2RAD(angle);
        T c = cos(a);
        T s = sin(a);
        tmat4x4 <T> res;

        tvec3 <T> axis = normalize(v);

        res[0][0] = c + (1 - c) * axis.x * axis.x;
        res[0][1] = (1 - c) * axis.x * axis.y + s * axis.z;
        res[0][2] = (1 - c) * axis.x * axis.z - s * axis.y;
        res[0][3] = 0;

        res[1][0] = (1 - c) * axis.y * axis.x - s * axis.z;
        res[1][1] = c + (1 - c) * axis.y * axis.y;
        res[1][2] = (1 - c) * axis.y * axis.z + s * axis.x;
        res[1][3] = 0;

        res[2][0] = (1 - c) * axis.z * axis.x + s * axis.y;
        res[2][1] = (1 - c) * axis.z * axis.y - s * axis.x;
        res[2][2] = c + (1 - c) * axis.z * axis.z;
        res[2][3] = 0;

        res[3] = tvec4<T>(0, 0, 0, 1);
        return m * res;
    }

    template<typename T>
    tmat4x4<T> scale_slow(tmat4x4<T> const &m, tvec3<T> const &v) {
        tmat4x4 <T> res(T(1));
        res[0][0] = v.x;
        res[1][1] = v.y;
        res[2][2] = v.z;

        return m * res;
    }

    template<typename valType>
    tmat4x4<valType> ortho
            (
                    valType left,
                    valType right,
                    valType bottom,
                    valType top,
                    valType zNear,
                    valType zFar
            ) {
        tmat4x4 <valType> res(1);
        res[0][0] = valType(2) / (right - left);
        res[1][1] = valType(2) / (top - bottom);
        res[2][2] = -valType(2) / (zFar - zNear);
        res[3][0] = -(right + left) / (right - left);
        res[3][1] = -(top + bottom) / (top - bottom);
        res[3][2] = -(zFar + zNear) / (zFar - zNear);
        return res;
    }

    template<typename valType>
    tmat4x4<valType> frustum
            (
                    valType left,
                    valType right,
                    valType bottom,
                    valType top,
                    valType nearVal,
                    valType farVal
            ) {
        tmat4x4 <valType> res(0);
        res[0][0] = (valType(2) * nearVal) / (right - left);
        res[1][1] = (valType(2) * nearVal) / (top - bottom);
        res[2][0] = (right + left) / (right - left);
        res[2][1] = (top + bottom) / (top - bottom);
        res[2][2] = -(farVal + nearVal) / (farVal - nearVal);
        res[2][3] = valType(-1);
        res[3][2] = -(valType(2) * farVal * nearVal) / (farVal - nearVal);
        return res;
    }

    template<typename valType>
    tmat4x4<valType> perspective(valType fovy, valType aspect, valType zNear, valType zFar) {
        valType range = tan(fovy * valType(DEG2RAD(0.5))) * zNear;
        valType left = -range * aspect;
        valType right = range * aspect;
        valType bottom = -range;
        valType top = range;

        tmat4x4 <valType> res(valType(0));
        res[0][0] = (valType(2) * zNear) / (right - left);
        res[1][1] = (valType(2) * zNear) / (top - bottom);
        res[2][2] = -(zFar + zNear) / (zFar - zNear);
        res[2][3] = -valType(1);
        res[3][2] = -(valType(2) * zFar * zNear) / (zFar - zNear);
        return res;
    }

    template<typename T>
    tvec3<T> project
            (
                    tvec3<T> const &obj,
                    tmat4x4<T> const &model,
                    tmat4x4<T> const &proj,
                    tvec4<T> const &viewport
            ) {
        tvec4 <T> tmp = tvec4<T>(obj.x, obj.y, obj.z, T(1));
        tmp = model * tmp;
        tmp = proj * tmp;

        tmp /= tmp.w;
        tmp = tmp * T(0.5) + T(0.5);
        tmp[0] = tmp[0] * T(viewport[2]) + T(viewport[0]);
        tmp[1] = tmp[1] * T(viewport[3]) + T(viewport[1]);

        return tvec3<T>(tmp.x, tmp.y, tmp.z);
    }

    template<typename T>
    tvec3<T> unProject
            (
                    tvec3<T> const &win,
                    tmat4x4<T> const &model,
                    tmat4x4<T> const &proj,
                    tvec4<T> const &viewport
            ) {
        tmat4x4 <T> inverses = (proj * model).inverse();

        tvec4 <T> tmp = tvec4<T>(win.x, win.y, win.z, T(1));
        tmp.x = (tmp.x - T(viewport[0])) / T(viewport[2]);
        tmp.y = (tmp.y - T(viewport[1])) / T(viewport[3]);
        tmp = tmp * T(2) - T(1);

        tvec4 <T> obj = inverses * tmp;
        obj /= obj.w;

        return tvec3<T>(obj.x, obj.y, obj.z);
    }

    template<typename T>
    tmat4x4<T> pickMatrix
            (
                    tvec2<T> const &center,
                    tvec2<T> const &delta,
                    tvec4<T> const &viewport
            ) {
        assert(delta.x > T(0) && delta.y > T(0));
        tmat4x4 <T> res(1.0f);

        if (!(delta.x > T(0) && delta.y > T(0))) {
            return res;
        }

        tvec3 <T> Temp
                (
                        (T(viewport[2]) - T(2) * (center.x - T(viewport[0]))) / delta.x,
                        (T(viewport[3]) - T(2) * (center.y - T(viewport[1]))) / delta.y,
                        T(0)
                );
        res = translate(res, Temp);
        return scale(res, tvec3<T>(T(viewport[2]) / delta.x, T(viewport[3]) / delta.y, T(1)));
    }

    template<typename T>
    tmat4x4<T> lookAt
            (
                    tvec3<T> const &eye,
                    tvec3<T> const &center,
                    tvec3<T> const &up
            ) {
        tvec3 <T> f = normalize(center - eye);
        tvec3 <T> u = normalize(up);
        tvec3 <T> s = normalize(cross(f, u));
        u = cross(s, f);

        tmat4x4 <T> res(1);
        res[0][0] = s.x;
        res[1][0] = s.y;
        res[2][0] = s.z;
        res[0][1] = u.x;
        res[1][1] = u.y;
        res[2][1] = u.z;
        res[0][2] = -f.x;
        res[1][2] = -f.y;
        res[2][2] = -f.z;
        res[3][0] = -dot(s, eye);
        res[3][1] = -dot(u, eye);
        res[3][2] = dot(f, eye);
        return res;
    }

    template<typename T>
    class AxisAlignedBox2D {
    public:
        enum Extent {
            EXTENT_NULL,
            EXTENT_FINITE,
            EXTENT_INFINITE
        };
    public:
        tvec2<T> _minimum;
        tvec2<T> _maximum;
        Extent _extent;
    public:
        /*
        1-----2
        /|    /|
        / |   / |
        5-----4  |
        |  0--|--3
        | /   | /
        |/    |/
        6-----7
        */
        typedef enum {
            FAR_LEFT_BOTTOM = 0,
            FAR_LEFT_TOP = 1,
            FAR_RIGHT_TOP = 2,
            FAR_RIGHT_BOTTOM = 3,
            NEAR_RIGHT_BOTTOM = 7,
            NEAR_LEFT_BOTTOM = 6,
            NEAR_LEFT_TOP = 5,
            NEAR_RIGHT_TOP = 4
        } CornerEnum;

        AxisAlignedBox2D() {
            _minimum = tvec2<T>(T(-0.5), T(-0.5));
            _maximum = tvec2<T>(T(0.5), T(0.5));
            _extent = EXTENT_NULL;
        }

        AxisAlignedBox2D(const AxisAlignedBox2D &rkBox) {
            setExtents(rkBox._minimum, rkBox._maximum);
            _extent = rkBox._extent;
        }

        AxisAlignedBox2D(const tvec2<T> &min, const tvec2<T> &max) {
            setExtents(min, max);
        }

        AxisAlignedBox2D(
                T mx, T my,
                T Mx, T My
        ) {
            setExtents(tvec2<T>(mx, my), tvec2<T>(Mx, My));
        }

        AxisAlignedBox2D<T> &operator=(const AxisAlignedBox2D<T> &right) {
            setExtents(right._minimum, right._maximum);
            return *this;
        }

        ~AxisAlignedBox2D() {
        }

        /**
        *   Gets the minimum corner of the box.
        */
        const tvec2<T> &getMinimum(void) const {
            return _minimum;
        }

        /**
        *   Gets a modifiable version of the minimum
        *   corner of the box.
        */
        tvec2<T> &getMinimum(void) {
            return _minimum;
        }

        void setMinimum(const tvec2<T> &vec) {
            _minimum = vec;
        }

        void setMinimum(T x, T y) {
            _minimum = tvec2<T>(x, y);
        }

        /**
        *   Gets the maximum corner of the box.
        */
        const tvec2<T> &getMaximum(void) const {
            return _maximum;
        }

        /**
        *   Gets a modifiable version of the maximum
        *   corner of the box.
        */
        tvec2<T> &getMaximum(void) {
            return _maximum;
        }


        /**
        *   Sets the maximum corner of the box.
        */
        void setMaximum(const tvec2<T> &vec) {
            _maximum = vec;
        }

        void setMaximum(T x, T y) {
            _maximum.x = x;
            _maximum.y = y;
        }

        /**
        *   Sets both minimum and maximum extents at once.
        */
        void setExtents(const tvec2<T> &min, const tvec2<T> &max) {
            _minimum = min;
            _maximum = max;
            _extent = EXTENT_FINITE;
        }

        void setExtents(
                T mx, T my,
                T Mx, T My
        ) {
            _minimum.x = mx;
            _minimum.y = my;

            _maximum.x = Mx;
            _maximum.y = My;
            _extent = EXTENT_FINITE;
        }

        inline bool intersects(const AxisAlignedBox2D &b2) const {
            if (_maximum.x < b2._minimum.x)
                return false;
            if (_maximum.y < b2._minimum.y)
                return false;

            if (_minimum.x > b2._maximum.x)
                return false;
            if (_minimum.y > b2._maximum.y)
                return false;
            return true;
        }


        inline AxisAlignedBox2D<T> intersection(const AxisAlignedBox2D<T> &b2) const {
            tvec2 <T> intMin = _minimum;
            tvec2 <T> intMax = _maximum;

            intMin.makeCeil(b2.getMinimum());
            intMax.makeFloor(b2.getMaximum());

            if (intMin.x < intMax.x &&
                intMin.y < intMax.y) {
                return AxisAlignedBox2D<T>(intMin, intMax);
            }

            return AxisAlignedBox2D<T>();
        }

        inline void setNull() {
            _extent = EXTENT_NULL;
        }

        inline bool isNull(void) const {
            return (_extent == EXTENT_NULL);
        }

        bool isFinite(void) const {
            return (_extent == EXTENT_FINITE);
        }

        inline void setInfinite() {
            _extent = EXTENT_INFINITE;
        }

        inline bool isInfinite(void) const {
            return (_extent == EXTENT_INFINITE);
        }


        inline bool intersects(const tvec2<T> &v) const {
            return (v.x >= _minimum.x && v.x <= _maximum.x &&
                    v.y >= _minimum.y && v.y <= _maximum.y);
        }


        inline tvec2<T> getCenter(void) const {
            return tvec2<T>(
                    (_maximum.x + _minimum.x) * T(0.5f),
                    (_maximum.y + _minimum.y) * T(0.5f));
        }

        /**
        *   Gets the size of the box
        */
        inline tvec2<T> getSize(void) const {
            return _maximum - _minimum;
        }

        inline tvec2<T> getHalfSize(void) const {
            return (_maximum - _minimum) * T(0.5);
        }

        inline bool contains(const tvec2<T> &v) const {
            return _minimum.x <= v.x && v.x <= _maximum.x &&
                   _minimum.y <= v.y && v.y <= _maximum.y;
        }

        inline bool contains(const AxisAlignedBox2D &other) const {
            return this->_minimum.x <= other._minimum.x &&
                   this->_minimum.y <= other._minimum.y &&
                   other._maximum.x <= this->_maximum.x &&
                   other._maximum.y <= this->_maximum.y;
        }

        inline bool operator==(const AxisAlignedBox2D &right) const {
            return this->_minimum == right._minimum &&
                   this->_maximum == right._maximum;
        }

        inline bool operator!=(const AxisAlignedBox2D &right) const {
            return !(*this == right);
        }

        inline void merge(tvec2<T> point) {
            if (_minimum.x > point.x) {
                _minimum.x = point.x;
            }

            if (_minimum.y > point.y) {
                _minimum.y = point.y;
            }
            if (_maximum.x < point.x) {
                _maximum.x = point.x;
            }
            if (_maximum.y < point.y) {
                _maximum.y = point.y;
            }
        }

        inline void merge(AxisAlignedBox2D<T> other) {
            _maximum.makeCeil(other._maximum);
            _minimum.makeFloor(other._minimum);
        }

    };


    template<typename T>
    class AxisAlignedBox {
    public:
        enum Extent {
            EXTENT_NULL,
            EXTENT_FINITE,
            EXTENT_INFINITE
        };
    public:
        tvec3<T> _minimum;
        tvec3<T> _maximum;
        Extent _extent;
    public:
        /*
        1-----2
        /|    /|
        / |   / |
        5-----4  |
        |  0--|--3
        | /   | /
        |/    |/
        6-----7
        */
        typedef enum {
            FAR_LEFT_BOTTOM = 0,
            FAR_LEFT_TOP = 1,
            FAR_RIGHT_TOP = 2,
            FAR_RIGHT_BOTTOM = 3,
            NEAR_RIGHT_BOTTOM = 7,
            NEAR_LEFT_BOTTOM = 6,
            NEAR_LEFT_TOP = 5,
            NEAR_RIGHT_TOP = 4
        } CornerEnum;

        AxisAlignedBox() {
            _minimum = tvec3<T>(T(-0.5), T(-0.5), T(-0.5));
            _maximum = tvec3<T>(T(0.5), T(0.5), T(0.5));
            _extent = EXTENT_NULL;
        }

        AxisAlignedBox(const AxisAlignedBox &rkBox) {
            setExtents(rkBox._minimum, rkBox._maximum);
            _extent = rkBox._extent;
        }

        AxisAlignedBox(const tvec3<T> &min, const tvec3<T> &max) {
            setExtents(min, max);
        }

        AxisAlignedBox(
                T mx, T my, T mz,
                T Mx, T My, T Mz
        ) {
            setExtents(mx, my, mz, Mx, My, Mz);
        }

        AxisAlignedBox<T> &operator=(const AxisAlignedBox<T> &right) {
            setExtents(right._minimum, right._maximum);
            return *this;
        }

        ~AxisAlignedBox() {
        }

        /**
        *   Gets the minimum corner of the box.
        */
        const tvec3<T> &getMinimum(void) const {
            return _minimum;
        }

        /**
        *   Gets a modifiable version of the minimum
        *   corner of the box.
        */
        tvec3<T> &getMinimum(void) {
            return _minimum;
        }

        void setMinimum(const tvec3<T> &mins) {
            _minimum = mins;
        }

        void setMinimum(T x, T y, T z) {
            _minimum = tvec3<T>(x, y, z);
        }

        /**
        *   Gets the maximum corner of the box.
        */
        const tvec3<T> &getMaximum(void) const {
            return _maximum;
        }

        /**
        *   Gets a modifiable version of the maximum
        *   corner of the box.
        */
        tvec3<T> &getMaximum(void) {
            return _maximum;
        }

        /**
        *   Sets the maximum corner of the box.
        */
        void setMaximum(const tvec3<T> &vec) {
            _maximum = vec;
        }

        void setMaximum(T x, T y, T z) {
            _maximum.x = x;
            _maximum.y = y;
            _maximum.z = z;
        }

        /**
        *   Changes one of the components of the maximum corner of the box
        *   used to resize only one dimension of the box
        */
        void setMaximumX(T x) {
            _maximum.x = x;
        }

        void setMaximumY(T y) {
            _maximum.y = y;
        }

        void setMaximumZ(T z) {
            _maximum.z = z;
        }

        /**
        *   Sets both minimum and maximum extents at once.
        */
        void setExtents(const tvec3<T> &min, const tvec3<T> &max) {
            _minimum = min;
            _maximum = max;
            _extent = EXTENT_FINITE;
        }

        void setExtents(
                T mx, T my, T mz,
                T Mx, T My, T Mz) {
            _minimum.x = mx;
            _minimum.y = my;
            _minimum.z = mz;

            _maximum.x = Mx;
            _maximum.y = My;
            _maximum.z = Mz;
            _extent = EXTENT_FINITE;

        }

        /** Returns a pointer to an array of 8 corner points, useful for
        collision vs. non-aligned objects.
        @remarks
        If the order of these corners is important, they are as
        follows: The 4 points of the minimum Z face (note that
        because Ogre uses right-handed coordinates, the minimum Z is
        at the 'back' of the box) starting with the minimum point of
        all, then anticlockwise around this face (if you are looking
        onto the face from outside the box). Then the 4 points of the
        maximum Z face, starting with maximum point of all, then
        anticlockwise around this face (looking onto the face from
        outside the box). Like this:
        <pre>
        1-----2
        /|    /|
        / |   / |
        5-----4  |
        |  0--|--3
        | /   | /
        |/    |/
        6-----7
        </pre>
        @remarks as this implementation uses a static member, make sure to use your own copy !
        */
        void getAllCorners(tvec3<T> mpCorners[8]) const {
            mpCorners[0] = _minimum;
            mpCorners[1].x = _minimum.x;
            mpCorners[1].y = _maximum.y;
            mpCorners[1].z = _minimum.z;
            mpCorners[2].x = _maximum.x;
            mpCorners[2].y = _maximum.y;
            mpCorners[2].z = _minimum.z;
            mpCorners[3].x = _maximum.x;
            mpCorners[3].y = _minimum.y;
            mpCorners[3].z = _minimum.z;

            mpCorners[4] = _maximum;
            mpCorners[5].x = _minimum.x;
            mpCorners[5].y = _maximum.y;
            mpCorners[5].z = _maximum.z;
            mpCorners[6].x = _minimum.x;
            mpCorners[6].y = _minimum.y;
            mpCorners[6].z = _maximum.z;
            mpCorners[7].x = _maximum.x;
            mpCorners[7].y = _minimum.y;
            mpCorners[7].z = _maximum.z;
        }

        /**
        *   gets the position of one of the corners
        */
        tvec3<T> getCorner(CornerEnum cornerToGet) const {
            switch (cornerToGet) {
                case FAR_LEFT_BOTTOM:
                    return _minimum;
                case FAR_LEFT_TOP:
                    return tvec3<T>(_minimum.x, _maximum.y, _minimum.z);
                case FAR_RIGHT_TOP:
                    return tvec3<T>(_maximum.x, _maximum.y, _minimum.z);
                case FAR_RIGHT_BOTTOM:
                    return tvec3<T>(_maximum.x, _minimum.y, _minimum.z);
                case NEAR_RIGHT_BOTTOM:
                    return tvec3<T>(_maximum.x, _minimum.y, _maximum.z);
                case NEAR_LEFT_BOTTOM:
                    return tvec3<T>(_minimum.x, _minimum.y, _maximum.z);
                case NEAR_LEFT_TOP:
                    return tvec3<T>(_minimum.x, _maximum.y, _maximum.z);
                case NEAR_RIGHT_TOP:
                    return _maximum;
                default:
                    return tvec3<T>();
            }
        }

        /**
        *   Merges the passed in box into the current box. The result is the
        *   box which encompasses both.
        */
        void merge(const AxisAlignedBox<T> &right) {

            if ((right._extent == EXTENT_NULL) || (_extent == EXTENT_INFINITE)) {
                return;
            } else if (right._extent == EXTENT_INFINITE) {
                _extent = EXTENT_INFINITE;
            } else if (_extent == EXTENT_NULL) {
                setExtents(right._minimum, right._maximum);
            } else {
                //! merge
                tvec3 <T> min = _minimum;
                tvec3 <T> max = _maximum;
                max.makeCeil(right._maximum);
                min.makeFloor(right._minimum);
                setExtents(min, max);
            }
        }

        /**
        *   Extends the box to encompass the specified point (if needed).
        */
        void merge(const tvec3<T> &point) {
            switch (_extent) {
                case EXTENT_NULL: // if null, use this point
                    setExtents(point, point);
                    return;

                case EXTENT_FINITE:
                    _maximum.makeCeil(point);
                    _minimum.makeFloor(point);
                    return;

                case EXTENT_INFINITE:
                    return;
            }
        }

        void transform(const tmat4x4<T> &matrix) {
            tvec3 <T> oldMin;
            tvec3 <T> oldMax;
            tvec3 <T> currentCorner;

            oldMin = _minimum;
            oldMax = _maximum;


            // We sequentially compute the corners in the following order :
            // 0, 6, 5, 1, 2, 4 ,7 , 3
            // This sequence allows us to only change one member at a time to get at all corners.

            // For each one, we transform it using the matrix
            // Which gives the resulting point and merge the resulting point.

            currentCorner = oldMin;
            tvec3 <T> vVert = currentCorner * matrix;
            setExtents(vVert, vVert);

            // First corner
            // min min min
            currentCorner = oldMin;
            merge(currentCorner * matrix);

            // min,min,max
            currentCorner.z = oldMax.z;
            merge(currentCorner * matrix);

            // min max max
            currentCorner.y = oldMax.y;
            merge(currentCorner * matrix);

            // min max min
            currentCorner.z = oldMin.z;
            merge(currentCorner * matrix);

            // max max min
            currentCorner.x = oldMax.x;
            merge(currentCorner * matrix);

            // max max max
            currentCorner.z = oldMax.z;
            merge(currentCorner * matrix);

            // max min max
            currentCorner.y = oldMin.y;
            merge(currentCorner * matrix);

            // max min min
            currentCorner.z = oldMin.z;
            merge(currentCorner * matrix);
        }

        /**
        *   Returns whether or not this box intersects another.
        */
        bool intersects(const AxisAlignedBox &b2) const {
            if (_maximum.x < b2._minimum.x)
                return false;
            if (_maximum.y < b2._minimum.y)
                return false;
            if (_maximum.z < b2._minimum.z)
                return false;

            if (_minimum.x > b2._maximum.x)
                return false;
            if (_minimum.y > b2._maximum.y)
                return false;
            if (_minimum.z > b2._maximum.z)
                return false;
            return true;

        }

        /**
        *   Returns whether or not this box intersects another.
        */
        bool intersectsNoZ(const AxisAlignedBox &b2) const {
            if (_maximum.x < b2._minimum.x)
                return false;
            if (_maximum.y < b2._minimum.y)
                return false;

            if (_minimum.x > b2._maximum.x)
                return false;
            if (_minimum.y > b2._maximum.y)
                return false;
            return true;

        }


        AxisAlignedBox<T> intersection(const AxisAlignedBox<T> &b2) const {
            tvec3 <T> intMin = _minimum;
            tvec3 <T> intMax = _maximum;

            intMin.makeCeil(b2.getMinimum());
            intMax.makeFloor(b2.getMaximum());

            if (intMin.x < intMax.x &&
                intMin.y < intMax.y &&
                intMin.z < intMax.z) {
                return AxisAlignedBox<T>(intMin, intMax);
            }

            return AxisAlignedBox<T>();
        }

        void setNull() {
            _extent = EXTENT_NULL;
        }

        bool isNull(void) const {
            return (_extent == EXTENT_NULL);
        }

        bool isFinite(void) const {
            return (_extent == EXTENT_FINITE);
        }

        void setInfinite() {
            _extent = EXTENT_INFINITE;
        }

        bool isInfinite(void) const {
            return (_extent == EXTENT_INFINITE);
        }

        void scale(const tvec3<T> &s) {
            tvec3 <T> min = _minimum * s;
            tvec3 <T> max = _maximum * s;
            setExtents(min, max);
        }

        bool intersects(const tvec3<T> &v) const {
            return (v.x >= _minimum.x && v.x <= _maximum.x &&
                    v.y >= _minimum.y && v.y <= _maximum.y &&
                    v.z >= _minimum.z && v.z <= _maximum.z);
        }

        bool intersects(const tvec2<T> &v) const {
            return (v.x >= _minimum.x && v.x <= _maximum.x &&
                    v.y >= _minimum.y && v.y <= _maximum.y);
        }

        tvec3<T> getCenter(void) const {
            return tvec3<T>(
                    (_maximum.x + _minimum.x) * T(0.5f),
                    (_maximum.y + _minimum.y) * T(0.5f),
                    (_maximum.z + _minimum.z) * T(0.5f)
            );
        }

        /**
        *   Gets the size of the box
        */
        tvec3<T> getSize(void) const {
            return _maximum - _minimum;
        }

        tvec3<T> getHalfSize(void) const {
            return (_maximum - _minimum) * T(0.5);
        }

        bool contains(const tvec3<T> &v) const {
            return _minimum.x <= v.x && v.x <= _maximum.x &&
                   _minimum.y <= v.y && v.y <= _maximum.y &&
                   _minimum.z <= v.z && v.z <= _maximum.z;
        }

        bool contains(const AxisAlignedBox &other) const {
            return this->_minimum.x <= other._minimum.x &&
                   this->_minimum.y <= other._minimum.y &&
                   this->_minimum.z <= other._minimum.z &&
                   other._maximum.x <= this->_maximum.x &&
                   other._maximum.y <= this->_maximum.y &&
                   other._maximum.z <= this->_maximum.z;
        }

        bool operator==(const AxisAlignedBox &right) const {
            return this->_minimum == right._minimum &&
                   this->_maximum == right._maximum;
        }

        bool operator!=(const AxisAlignedBox &right) const {
            return !(*this == right);
        }
    };

    template<typename T>
    class tspline {
    public:
        tspline() {
            mCoeffs[0][0] = 2;
            mCoeffs[0][1] = -2;
            mCoeffs[0][2] = 1;
            mCoeffs[0][3] = 1;
            mCoeffs[1][0] = -3;
            mCoeffs[1][1] = 3;
            mCoeffs[1][2] = -2;
            mCoeffs[1][3] = -1;
            mCoeffs[2][0] = 0;
            mCoeffs[2][1] = 0;
            mCoeffs[2][2] = 1;
            mCoeffs[2][3] = 0;
            mCoeffs[3][0] = 1;
            mCoeffs[3][1] = 0;
            mCoeffs[3][2] = 0;
            mCoeffs[3][3] = 0;

            mCoeffs = mCoeffs.transpose();
            mAutoCalc = true;
        }

        ~tspline() {};

        void addPoint(const tvec3<T> &p) {
            mPoints.push_back(p);
            if (mAutoCalc) {
                recalcTangents();
            }
        }

        const tvec3<T> &getPoint(size_t index) const {
            assert (index < mPoints.size() && "Point index is out of bounds!!");

            return mPoints[index];
        }

        tvec3<T> &getPoint(size_t index) {
            assert (index < mPoints.size() && "Point index is out of bounds!!");

            return mPoints[index];
        }

        /**
        *   获取点的数量
        */
        size_t getNumPoints(void) const {
            return mPoints.size();
        }

        /**
        *   清除所有的点数据
        */
        void clear(void) {
            mPoints.clear();
            mTangents.clear();
        }

        /**
        *   更新点数据
        */
        void updatePoint(size_t index, const tvec3<T> &value) {
            assert (index < mPoints.size() && "Point index is out of bounds!!");

            mPoints[index] = value;
            if (mAutoCalc) {
                recalcTangents();
            }
        }

        /**
        *   差值获取点数据
        */
        tvec3<T> interpolate(T time) const {
            T fSeg = time * (mPoints.size() - 1);
            unsigned segIdx = (unsigned) fSeg;
            // Apportion t
            time = fSeg - segIdx;

            return interpolate(segIdx, time);
        }

        /**
        *   根据索引差值
        */
        tvec3<T> interpolate(size_t fromIndex, T t) const {
            // Bounds check
            assert (fromIndex < mPoints.size() && "fromIndex out of bounds");

            if ((fromIndex + 1) == mPoints.size()) {
                // Duff request, cannot blend to nothing
                // Just return source
                return mPoints[fromIndex];
            }
            // Fast special cases
            if (t == 0.0f) {
                return mPoints[fromIndex];
            } else if (t == 1.0f) {
                return mPoints[fromIndex + 1];
            }

            // float interpolation
            // Form a vector of powers of t
            T t2, t3;
            t2 = t * t;
            t3 = t2 * t;
            tvec4 <T> powers(t3, t2, t, 1);

            const tvec3 <T> &point1 = mPoints[fromIndex];
            const tvec3 <T> &point2 = mPoints[fromIndex + 1];
            const tvec3 <T> &tan1 = mTangents[fromIndex];
            const tvec3 <T> &tan2 = mTangents[fromIndex + 1];
            tmat4x4 <T> pt;

            pt[0][0] = point1.x;
            pt[0][1] = point1.y;
            pt[0][2] = point1.z;
            pt[0][3] = 1.0f;
            pt[1][0] = point2.x;
            pt[1][1] = point2.y;
            pt[1][2] = point2.z;
            pt[1][3] = 1.0f;
            pt[2][0] = tan1.x;
            pt[2][1] = tan1.y;
            pt[2][2] = tan1.z;
            pt[2][3] = 1.0f;
            pt[3][0] = tan2.x;
            pt[3][1] = tan2.y;
            pt[3][2] = tan2.z;
            pt[3][3] = 1.0f;

            pt = pt.transpose();

            tvec4 <T> ret = powers * mCoeffs * pt;

            return tvec3<T>(ret.x, ret.y, ret.z);
        }

        /**
        *   自动计算标记
        */
        void setAutoCalculate(bool autoCalc) {
            mAutoCalc = autoCalc;
        }

        /**
        *   计算切线
        */
        void recalcTangents(void) {
            size_t i, numPoints;
            bool isClosed;

            numPoints = mPoints.size();
            if (numPoints < 2) {
                return;
            }
            if (mPoints[0] == mPoints[numPoints - 1]) {
                isClosed = true;
            } else {
                isClosed = false;
            }

            mTangents.resize(numPoints);


            for (i = 0; i < numPoints; ++i) {
                if (i == 0) {
                    // Special case start
                    if (isClosed) {
                        // Use numPoints-2 since numPoints-1 is the last point and == [0]
                        mTangents[i] = 0.5f * (mPoints[1] - mPoints[numPoints - 2]);
                    } else {
                        mTangents[i] = 0.5f * (mPoints[1] - mPoints[0]);
                    }
                } else if (i == numPoints - 1) {
                    if (isClosed) {
                        mTangents[i] = mTangents[0];
                    } else {
                        mTangents[i] = 0.5f * (mPoints[i] - mPoints[i - 1]);
                    }
                } else {
                    mTangents[i] = 0.5f * (mPoints[i + 1] - mPoints[i - 1]);
                }
            }
        }

    public:
        bool mAutoCalc;
        std::vector<tvec3<T> > mPoints;
        std::vector<tvec3<T> > mTangents;
        tmat4x4<T> mCoeffs;
    };


    template<typename T>
    class tellipsoidModel {
    public:
        tellipsoidModel(T radiusEquator = T(WGS_84_RADIUS_EQUATOR), T radiusPolar = T(WGS_84_RADIUS_POLAR)) {
            _radiusEquator = radiusEquator;
            _radiusPolar = radiusPolar;
            T flattening = (_radiusEquator - _radiusPolar) / _radiusEquator;
            _eccentricitySquared = T(2) * flattening - flattening * flattening;
        }

        ~tellipsoidModel(void) {
        }


        void convertLatLongHeightToXYZ(
                T latitude,
                T longitude,
                T height,
                T &X,
                T &Y,
                T &Z
        ) const {
            // for details on maths see http://www.colorado.edu/geography/gcraft/notes/datum/gif/llhxyz.gif
            T sin_latitude = sin(latitude);
            T cos_latitude = cos(latitude);
            T N = _radiusEquator / sqrt(1.0 - _eccentricitySquared * sin_latitude * sin_latitude);
            X = (N + height) * cos_latitude * cos(longitude);
            Y = (N + height) * cos_latitude * sin(longitude);
            Z = (N * (1 - _eccentricitySquared) + height) * sin_latitude;
        }


        void convertXYZToLatLongHeight(
                T X,
                T Y,
                T Z,
                T &latitude,
                T &longitude,
                T &height
        ) const {
            // http://www.colorado.edu/geography/gcraft/notes/datum/gif/xyzllh.gif
            T p = (T) sqrt(X * X + Y * Y);
            T theta = (T) atan2(Z * _radiusEquator, (p * _radiusPolar));
            T eDashSquared =
                    (_radiusEquator * _radiusEquator - _radiusPolar * _radiusPolar) / (_radiusPolar * _radiusPolar);

            T sin_theta = (T) sin(theta);
            T cos_theta = (T) cos(theta);

            latitude = (T) atan((Z + eDashSquared * _radiusPolar * sin_theta * sin_theta * sin_theta) /
                                (p - _eccentricitySquared * _radiusEquator * cos_theta * cos_theta * cos_theta));
            longitude = (T) atan2(Y, X);

            T sin_latitude = (T) sin(latitude);
            T N = _radiusEquator / (T) sqrt(1.0 - _eccentricitySquared * sin_latitude * sin_latitude);

            height = p / (T) cos(latitude) - N;
        }

    protected:
        T _radiusEquator;
        T _radiusPolar;
        T _eccentricitySquared;
    };

    class Rgba4Byte {
    public:
        Rgba4Byte(
                unsigned char r = 255,
                unsigned char g = 255,
                unsigned char b = 255,
                unsigned char a = 255
        ) {
            _r = r;
            _g = g;
            _b = b;
            _a = a;
        }

        friend bool operator==(const Rgba4Byte &left, const Rgba4Byte &right) {
            return left._r == right._r &&
                   left._g == right._g &&
                   left._b == right._b &&
                   left._a == right._a;
        }

        friend bool operator!=(const Rgba4Byte &left, const Rgba4Byte &right) {
            return left._r != right._r ||
                   left._g != right._g ||
                   left._b != right._b ||
                   left._a != right._a;
        }

        operator unsigned() {
            unsigned color;
            char *pColor = (char *) &color;
            pColor[0] = _r;
            pColor[1] = _g;
            pColor[2] = _b;
            pColor[3] = _a;
            return color;
        }

        operator int() {
            int color;
            char *pColor = (char *) &color;
            pColor[0] = _r;
            pColor[1] = _g;
            pColor[2] = _b;
            pColor[3] = _a;
            return color;
        }

        operator long() {
            long color;
            char *pColor = (char *) &color;
            pColor[0] = _r;
            pColor[1] = _g;
            pColor[2] = _b;
            pColor[3] = _a;
            return color;
        }

    public:
        unsigned char _r;
        unsigned char _g;
        unsigned char _b;
        unsigned char _a;
    };

    typedef Rgba4Byte Rgba;

    inline Rgba4Byte colorLerp(const Rgba4Byte &c1, const Rgba4Byte &c2, float s) {
        Rgba4Byte color;

        color._r = (unsigned char) (c1._r + s * (c2._r - c1._r));
        color._g = (unsigned char) (c1._g + s * (c2._g - c1._g));
        color._b = (unsigned char) (c1._b + s * (c2._b - c1._b));
        color._a = (unsigned char) (c1._a + s * (c2._a - c1._a));
        return color;
    }

    template<typename T>
    class tAxisAlignedBox2 {
    public:
        enum Extent {
            EXTENT_NULL,
            EXTENT_FINITE,
            EXTENT_INFINITE
        };
    public:
        tvec2<T> _vMin;
        tvec2<T> _vMax;
        Extent mExtent;
    public:
        tvec2<T> center() const {
            return (_vMin + _vMax) * T(0.5);
        }

        tvec2<T> size() const {
            return _vMax - _vMin;
        }

        tvec2<T> halfSize() const {
            return (_vMax - _vMin) * T(0.5);
        }

        bool intersects(tvec2<T> v) const {
            return (v.x >= _vMin.x && v.x <= _vMax.x &&
                    v.y >= _vMin.y && v.y <= _vMax.y);
        }

        void merge(tvec2<T> point) {
            if (_vMin.x > point.x) {
                _vMin.x = point.x;
            }

            if (_vMin.y > point.y) {
                _vMin.y = point.y;
            }
            if (_vMax.x < point.x) {
                _vMax.x = point.x;
            }
            if (_vMax.y < point.y) {
                _vMax.y = point.y;
            }
        }


        void merge(tAxisAlignedBox2<T> other) {
            _vMax.makeCeil(other._vMax);
            _vMin.makeFloor(other._vMin);
        }

        bool contains(tvec2<T> point) const {
            return _vMin.x <= point.x && point.x <= _vMax.x &&
                   _vMin.y <= point.y && point.y <= _vMax.y;
        }

        bool contains(tAxisAlignedBox2<T> other) const {
            return this->_vMin.x <= other._vMin.x &&
                   this->_vMin.y <= other._vMin.y &&
                   other._vMax.x <= this->_vMax.x &&
                   other._vMax.y <= this->_vMax.y;
        }
    };

    template<typename T>
    class tray {
        typedef T value_type;
        typedef tray<T> type;
    protected:
        tvec3<T> _origin;
        tvec3<T> _direction;
    public:
        tray() :
                _origin(value_type(0), value_type(0), value_type(0)),
                _direction(value_type(0), value_type(0), value_type(1)) {}

        tray(const tvec3<T> &origin, const tvec3<T> &direction) :
                _origin(origin),
                _direction(direction) {}

        /**
        *   设置射线的起点
        */
        void setOrigin(const tvec3<T> &origin) {
            _origin = origin;
        }

        /**
        *   返回射线的起点
        */
        const tvec3<T> &getOrigin(void) const {
            return _origin;
        }

        /**
        *   设置射线的方向
        */
        void setDirection(const tvec3<T> &dir) {
            _direction = dir;
        }

        /**
        *   返回射线的方向
        */
        const tvec3<T> &getDirection(void) const {
            return _direction;
        }

        /**
    *   Gets the position of a point t units along the ray.
    */
        tvec3<T> getPoint(T time) const {
            return tvec3<T>(_origin + (_direction * time));
        }

        /**
        *   测试射线box相交
        *   如果相交,返回值中的first == true.否则false
        *   second为射线到点的距离
        *   调用getPoint方法，则返回交点
        */
        std::pair<bool, T> intersects(const AxisAlignedBox<T> &box) const {
            T lowt = 0.0f;
            T t;
            bool hit = false;
            tvec3 <T> hitpoint;
            tvec3 <T> min = box.getMinimum();
            tvec3 <T> max = box.getMaximum();

            /**
            *   点在包围盒里面
            */
            if (_origin > min && _origin < max) {
                return std::pair<bool, T>(true, 0.0f);
            }

            // Check each face in turn, only check closest 3
            // Min x
            if (_origin.x <= min.x && _direction.x > 0) {
                t = (min.x - _origin.x) / _direction.x;
                if (t >= 0) {
                    // Substitute t back into ray and check bounds and dist
                    hitpoint = _origin + _direction * t;
                    if (hitpoint.y >= min.y &&
                        hitpoint.y <= max.y &&
                        hitpoint.z >= min.z &&
                        hitpoint.z <= max.z &&
                        (!hit || t < lowt)) {
                        hit = true;
                        lowt = t;
                    }
                }
            }
            // Max x
            if (_origin.x >= max.x && _direction.x < 0) {
                t = (max.x - _origin.x) / _direction.x;
                if (t >= 0) {
                    // Substitute t back into ray and check bounds and dist
                    hitpoint = _origin + _direction * t;
                    if (hitpoint.y >= min.y &&
                        hitpoint.y <= max.y &&
                        hitpoint.z >= min.z &&
                        hitpoint.z <= max.z &&
                        (!hit || t < lowt)) {
                        hit = true;
                        lowt = t;
                    }
                }
            }
            // Min y
            if (_origin.y <= min.y && _direction.y > 0) {
                t = (min.y - _origin.y) / _direction.y;
                if (t >= 0) {
                    // Substitute t back into ray and check bounds and dist
                    hitpoint = _origin + _direction * t;
                    if (hitpoint.x >= min.x &&
                        hitpoint.x <= max.x &&
                        hitpoint.z >= min.z &&
                        hitpoint.z <= max.z &&
                        (!hit || t < lowt)) {
                        hit = true;
                        lowt = t;
                    }
                }
            }
            // Max y
            if (_origin.y >= max.y && _direction.y < 0) {
                t = (max.y - _origin.y) / _direction.y;
                if (t >= 0) {
                    // Substitute t back into ray and check bounds and dist
                    hitpoint = _origin + _direction * t;
                    if (hitpoint.x >= min.x &&
                        hitpoint.x <= max.x &&
                        hitpoint.z >= min.z &&
                        hitpoint.z <= max.z &&
                        (!hit || t < lowt)) {
                        hit = true;
                        lowt = t;
                    }
                }
            }
            // Min z
            if (_origin.z <= min.z && _direction.z > 0) {
                t = (min.z - _origin.z) / _direction.z;
                if (t >= 0) {
                    // Substitute t back into ray and check bounds and dist
                    hitpoint = _origin + _direction * t;
                    if (hitpoint.x >= min.x &&
                        hitpoint.x <= max.x &&
                        hitpoint.y >= min.y &&
                        hitpoint.y <= max.y &&
                        (!hit || t < lowt)) {
                        hit = true;
                        lowt = t;
                    }
                }
            }
            // Max z
            if (_origin.z >= max.z && _direction.z < 0) {
                t = (max.z - _origin.z) / _direction.z;
                if (t >= 0) {
                    // Substitute t back into ray and check bounds and dist
                    hitpoint = _origin + _direction * t;
                    if (hitpoint.x >= min.x &&
                        hitpoint.x <= max.x &&
                        hitpoint.y >= min.y &&
                        hitpoint.y <= max.y &&
                        (!hit || t < lowt)) {
                        hit = true;
                        lowt = t;
                    }
                }
            }
            return std::pair<bool, T>(hit, lowt);
        }
    };


    template<class T>
    class Plane {
    public:
        tvec3<T> _normal;
        T _distance;
    public:
        Plane() {
            _normal = tvec3<T>(0, 0, 0);
            _distance = 0.0f;
        }

        Plane(const Plane &right) {
            _normal = right._normal;
            _distance = right._distance;
        }

        /** Construct a plane through a normal, and a distance to move the plane along the normal.*/
        Plane(const tvec3<T> &rkNormal, T fConstant) {
            _normal = rkNormal;
            _distance = -fConstant;
        }

        /** Construct a plane using the 4 constants directly **/
        Plane(T x, T y, T z, T o) {
            _normal = tvec3<T>(x, y, z);
            T invLen = 1.0f / (_normal).length();
            _normal *= invLen;
            _distance = o * invLen;
        }

        Plane(const tvec3<T> &rkNormal, const tvec3<T> &rkPoint) {
            redefine(rkNormal, rkPoint);
        }

        Plane(const tvec3<T> &rkPoint0, const tvec3<T> &rkPoint1, const tvec3<T> &rkPoint2) {
            redefine(rkPoint0, rkPoint1, rkPoint2);
        }

        /**
        *   到点的距离
        */
        float distance(const tvec3<T> &pos) const {
            return dot(_normal, pos) + _distance;
        }

        /** The "positive side" of the plane is the half space to which the
            plane normal points. The "negative side" is the other half
            space. The flag "no side" indicates the plane itself.
        */
        enum Side {
            NO_SIDE,
            POSITIVE_SIDE,
            NEGATIVE_SIDE,
            BOTH_SIDE
        };

        Side getSide(const tvec3<T> &rkPoint) const {
            float fDistance = getDistance(rkPoint);

            if (fDistance < 0.0)
                return Plane::NEGATIVE_SIDE;

            if (fDistance > 0.0)
                return Plane::POSITIVE_SIDE;

            return Plane::NO_SIDE;
        }


        Side getSide(const tvec3<T> &centre, const tvec3<T> &halfSize) const {
            // Calculate the distance between box centre and the plane
            float dist = getDistance(centre);

            // Calculate the maximise allows absolute distance for
            // the distance between box centre and plane
            float maxAbsDist = _normal.absDot(halfSize);

            if (dist < -maxAbsDist)
                return Plane::NEGATIVE_SIDE;

            if (dist > +maxAbsDist)
                return Plane::POSITIVE_SIDE;

            return Plane::BOTH_SIDE;
        }

        float getDistance(const tvec3<T> &rkPoint) const {
            return _normal.dot(rkPoint) + _distance;
        }

        void redefine(const tvec3<T> &rkPoint0, const tvec3<T> &rkPoint1,
                      const tvec3<T> &rkPoint2) {
            tvec3 <T> kEdge1 = rkPoint1 - rkPoint0;
            tvec3 <T> kEdge2 = rkPoint2 - rkPoint0;
            _normal = cross(kEdge1, kEdge2);
            _normal.normalise();
            _distance = -dot(_normal, rkPoint0);
        }

        /** Redefine this plane based on a normal and a point. */
        void redefine(const tvec3<T> &rkNormal, const tvec3<T> &rkPoint) {
            _normal = rkNormal;
            _distance = -dot(rkNormal, rkPoint);
        }


// 	    tvec3<T> projectVector(const tvec3<T>& p) const
//         {
//             matrix3 xform;
//             xform[0][0] = 1.0f - _normal.x * _normal.x;
//             xform[0][1] = -_normal.x * _normal.y;
//             xform[0][2] = -_normal.x * _normal.z;
//             xform[1][0] = -_normal.y * _normal.x;
//             xform[1][1] = 1.0f - _normal.y * _normal.y;
//             xform[1][2] = -_normal.y * _normal.z;
//             xform[2][0] = -_normal.z * _normal.x;
//             xform[2][1] = -_normal.z * _normal.y;
//             xform[2][2] = 1.0f - _normal.z * _normal.z;
//             return xform * p;
//         }

        /** Normalises the plane.
            @remarks
                This method normalises the plane's normal and the length scale of d
                is as well.
            @note
                This function will not crash for zero-sized vectors, but there
                will be no changes made to their components.
            @returns The previous length of the plane's normal.
        */
        float normalise(void) {
            float fLength = _normal.length();

            // Will also work for zero-sized vectors, but will change nothing
            if (fLength > 1e-08f) {
                float fInvLength = 1.0f / fLength;
                _normal *= fInvLength;
                _distance *= fInvLength;
            }

            return fLength;
        }


        /// Comparison operator
        bool operator==(const Plane &right) const {
            return (right._distance == _distance && right._normal == _normal);
        }

        bool operator!=(const Plane &right) const {
            return (right._distance != _distance && right._normal != _normal);
        }
    };

    template<class T>
    class tfrustum {
    public:
        enum {
            FRUSTUM_LEFT = 0,
            FRUSTUM_RIGHT = 1,
            FRUSTUM_TOP = 2,
            FRUSTUM_BOTTOM = 3,
            FRUSTUM_FAR = 4,
            FRUSTUM_NEAR = 5,
        };
    public:
        /**
        *   project * modleview
        */
        void loadFrustum(const tmat4x4<T> &mvp) {
            const T *dataPtr = mvp.data();
            _planes[FRUSTUM_LEFT] = Plane<T>(dataPtr[12] - dataPtr[0], dataPtr[13] - dataPtr[1],
                                             dataPtr[14] - dataPtr[2], dataPtr[15] - dataPtr[3]);
            _planes[FRUSTUM_RIGHT] = Plane<T>(dataPtr[12] + dataPtr[0], dataPtr[13] + dataPtr[1],
                                              dataPtr[14] + dataPtr[2], dataPtr[15] + dataPtr[3]);

            _planes[FRUSTUM_TOP] = Plane<T>(dataPtr[12] - dataPtr[4], dataPtr[13] - dataPtr[5],
                                            dataPtr[14] - dataPtr[6], dataPtr[15] - dataPtr[7]);
            _planes[FRUSTUM_BOTTOM] = Plane<T>(dataPtr[12] + dataPtr[4], dataPtr[13] + dataPtr[5],
                                               dataPtr[14] + dataPtr[6], dataPtr[15] + dataPtr[7]);

            _planes[FRUSTUM_FAR] = Plane<T>(dataPtr[12] - dataPtr[8], dataPtr[13] - dataPtr[9],
                                            dataPtr[14] - dataPtr[10], dataPtr[15] - dataPtr[11]);
            _planes[FRUSTUM_NEAR] = Plane<T>(dataPtr[12] + dataPtr[8], dataPtr[13] + dataPtr[9],
                                             dataPtr[14] + dataPtr[10], dataPtr[15] + dataPtr[11]);
        }

        bool pointInFrustum(const tvec3<T> &pos) const {
            for (int i = 0; i < 6; i++) {
                if (_planes[i].distance(pos) <= 0)
                    return false;
            }
            return true;
        }

        bool sphereInFrustum(const tvec3<T> &pos, const float radius) const {
            for (int i = 0; i < 6; i++) {
                if (_planes[i].distance(pos) <= -radius)
                    return false;
            }
            return true;
        }

        bool cubeInFrustum(T minX, T maxX, T minY, T maxY, T minZ, T maxZ) const {
            for (int i = 0; i < 6; i++) {
                if (_planes[i].distance(tvec3<T>(minX, minY, minZ)) > 0) continue;
                if (_planes[i].distance(tvec3<T>(minX, minY, maxZ)) > 0) continue;
                if (_planes[i].distance(tvec3<T>(minX, maxY, minZ)) > 0) continue;
                if (_planes[i].distance(tvec3<T>(minX, maxY, maxZ)) > 0) continue;
                if (_planes[i].distance(tvec3<T>(maxX, minY, minZ)) > 0) continue;
                if (_planes[i].distance(tvec3<T>(maxX, minY, maxZ)) > 0) continue;
                if (_planes[i].distance(tvec3<T>(maxX, maxY, minZ)) > 0) continue;
                if (_planes[i].distance(tvec3<T>(maxX, maxY, maxZ)) > 0) continue;
                return false;
            }
            return true;
        }

        const Plane<T> &getPlane(const int plane) const {
            return _planes[plane];
        }

    protected:
        Plane<T> _planes[6];
    };


    typedef double real;
    typedef tvec2<int> int2;
    typedef tvec2<float> float2;
    typedef tvec2<double> double2;

    typedef tvec2<real> real2;


    typedef tvec3<int> int3;
    typedef tvec3<unsigned> uint3;
    typedef tvec3<float> float3;
    typedef tvec3<double> double3;

    typedef tvec3<real> real3;


    typedef tvec4<int> int4;
    typedef tvec4<float> float4;
    typedef tvec4<double> double4;
    typedef tvec4<real> real4;
    typedef trect<real> rect4;
    typedef trect<int> rect4i;

    typedef AxisAlignedBox<float> aabb3d;
    typedef AxisAlignedBox<real> aabbr;

    typedef AxisAlignedBox2D<float> AABB2D;
    typedef AxisAlignedBox2D<real> aabb2dr;
    typedef AxisAlignedBox2D<int> aabb2di;


    typedef tmat2x2<float> matrix2;
    typedef tmat3x3<float> matrix3;
    typedef tmat4x4<float> matrix4;
    typedef tmat4x4<real>  matrix4r;

    typedef tquat<float> quaternion;
    typedef tquat<real>  quatr;
    typedef tray<float>  Ray;
    typedef tfrustum<float> Frustum;

    typedef tellipsoidModel<float> ellipsoid;

}

#endif //NATIVECPPAPP_CELLMATH_HPP
