//
// Created by zzr on 2019/3/18.
//
#pragma once
#ifndef NATIVECPPAPP_CELLMATH_HPP
#define NATIVECPPAPP_CELLMATH_HPP

#include <cmath>

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
        /**
         * Multiplies two 4x4 matrices together and stores the result in a third 4x4 matrix.
         * In matrix notation: result = lhs x rhs.
         */
        static void multiplyMM(float* re, const float* lhs, const float* rhs)
        {
            int re_length = sizeof(re) / sizeof(re[0]);
            int lhs_length = sizeof(lhs) / sizeof(lhs[0]);
            int rhs_length = sizeof(rhs) / sizeof(rhs[0]);

            if(re_length<16 || lhs_length<16 || rhs_length<16)
                return;

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
            int lhs_lenght = sizeof(lhs) / sizeof(lhs[0]);
            int rhs_lenght = sizeof(rhs) / sizeof(rhs[0]);
            if(lhs_lenght<4||rhs_lenght<16)
                return;
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
            int sm_length = sizeof(sm) / sizeof(sm[0]);
            if(sm_length-smOffset < 16)
                return;

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
            m[0] = a / aspect;  m[4] = 0f;  m[8] = 0f;              m[12] = 0f;
            m[1] = 0f;          m[5] = a;   m[9] = 0f;              m[13] = 0f;
            m[2] = 0f;          m[6] = 0f;  m[10] = -((f+n)/(f-n)); m[14] = -((2f*f*n)/(f-n));
            m[3] = 0f;          m[7] = 0f;  m[11] = -1f;            m[15] = 0f;
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
        static
        void translateM(float* m, int mOffset, float x, float y, float z)
        {
            for (int i=0 ; i<4 ; i++) {
                int mi = mOffset + i;
                m[12 + mi] += m[mi] * x + m[4 + mi] * y + m[8 + mi] * z;
            }
        }
    };
}
#endif //NATIVECPPAPP_CELLMATH_HPP
