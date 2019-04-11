//
// Created by zzr on 2019/3/18.
//
#pragma once
#ifndef NATIVECPPAPP_CELLMATH_HPP
#define NATIVECPPAPP_CELLMATH_HPP

#include <cmath>
#include <assert.h>

namespace CELL
{
    #define PI                 3.14159265358979323
    #define TWO_PI             6.28318530717958647
    #define HALF_PI            1.57079632679489661
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







    // tvec3
    template <typename T>
    struct tvec3
    {
        typedef T               value_type;
        typedef std::size_t     size_type;
        typedef tvec3<T>        type;

        value_type  x;
        value_type  y;
        value_type  z;

        size_type   length() const
        {
            return 3;
        }

        inline tvec3() : x(value_type(0)), y(value_type(0)), z(value_type(0)) {}

        inline tvec3(tvec3<T> const & v) : x(v.x), y(v.y), z(v.z) {}

        inline tvec3(value_type s) : x(s), y(s), z(s) {}

        inline tvec3(value_type s0, value_type s1, value_type s2) :
                x(s0), y(s1), z(s2) {}

        value_type & operator[](size_type i)
        {
            assert(i < this->length());
            return (&x)[i];
        }

        value_type const & operator[](size_type i) const
        {
            assert(i < this->length());
            return (&x)[i];
        }

        tvec3<T> & operator= (tvec3<T> const & v)
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            return *this;
        }
        tvec3<T> & operator+=(tvec3 const & v)
        {
            this->x += v.x;
            this->y += v.y;
            this->z += v.z;
            return *this;
        }
        tvec3<T> & operator-=(tvec3 const & s)
        {
            this->x -= s.x;
            this->y -= s.y;
            this->z -= s.z;
            return *this;
        }
        tvec3<T> & operator*=(tvec3 const & v)
        {
            this->x *= v.x;
            this->y *= v.y;
            this->z *= v.z;
            return *this;
        }
        tvec3<T> & operator/=(tvec3 const & v)
        {
            this->x /= v.x;
            this->y /= v.y;
            this->z /= v.z;
            return *this;
        }
        tvec3<T> & operator++()
        {
            ++this->x;
            ++this->y;
            ++this->z;
            return *this;
        }
        tvec3<T> & operator--()
        {
            --this->x;
            --this->y;
            --this->z;
            return *this;
        }

        void    makeFloor( const tvec3<T>& cmp )
        {
            if( cmp.x < x ) x = cmp.x;
            if( cmp.y < y ) y = cmp.y;
            if( cmp.z < z ) z = cmp.z;
        }
        void    makeCeil( const tvec3<T>& cmp )
        {
            if( cmp.x > x ) x = cmp.x;
            if( cmp.y > y ) y = cmp.y;
            if( cmp.z > z ) z = cmp.z;
        }
        T       lengthf() const
        {
            return (T)sqrtf( x * x + y * y + z * z );
        }
    };

    template <typename T>
    tvec3<T> operator- (tvec3<T> const & v1, tvec3<T> const & v2)
    {
        return tvec3<T>(
                v1.x - T(v2.x),
                v1.y - T(v2.y),
                v1.z - T(v2.z));
    }
    template <typename T>
    tvec3<T> operator- (T const & s, tvec3<T> const & v)
    {
        return tvec3<T>(
                T(s) - v.x,
                T(s) - v.y,
                T(s) - v.z);
    }
    template <typename T>
    tvec3<T> operator- (tvec3<T> const & v, T const & s)
    {
        return tvec3<T>(
                v.x - T(s),
                v.y - T(s),
                v.z - T(s));
    }
    template <typename T>
    tvec3<T> operator- (tvec3<T> const & v)
    {
        return tvec3<T>(
                -v.x,
                -v.y,
                -v.z);
    }
    template <typename T>
    tvec3<T> operator+(tvec3<T> const & v, T const & s)
    {
        return tvec3<T>(
                v.x + T(s),
                v.y + T(s),
                v.z + T(s));
    }
    template <typename T>
    tvec3<T> operator+ ( T const & s, tvec3<T> const & v)
    {
        return tvec3<T>(
                T(s) + v.x,
                T(s) + v.y,
                T(s) + v.z);
    }
    template <typename T>
    tvec3<T> operator+ (tvec3<T> const & v1, tvec3<T> const & v2)
    {
        return tvec3<T>(
                v1.x + T(v2.x),
                v1.y + T(v2.y),
                v1.z + T(v2.z));
    }
    // tvec3 end.


















    template <typename T>
    T inversesqrt(T x)
    {
        return T(1) / sqrt(x);
    }
    template <typename T>
    tvec3<T> normalize(tvec3<T> const & x)
    {
        typename tvec3<T>::value_type sqr = x.x * x.x + x.y * x.y + x.z * x.z;
        return x * inversesqrt(sqr);
    }














    typedef double            real;
    typedef tvec3<real>       real3;
    typedef tvec3<int>        int3;
    typedef tvec3<unsigned>   uint3;
    typedef tvec3<float>      float3;
    typedef tvec3<double>     double3;
}
#endif //NATIVECPPAPP_CELLMATH_HPP
