#ifndef MATH_XENON_MATRIX_TYPES_H
#define MATH_XENON_MATRIX_TYPES_H

#include <vectorintrinsics.h>

typedef __vector4 XMVECTOR;
struct __declspec(align(16)) _XMMATRIX {
    union {
        XMVECTOR r[4];
        struct {
            float _11, _12, _13, _14;
            float _21, _22, _23, _24;
            float _31, _32, _33, _34;
            float _41, _42, _43, _44;
        };
        float m[4][4];
    };
    _XMMATRIX() {}
    __forceinline _XMMATRIX &operator=(const _XMMATRIX &M)
    {
        r[0] = M.r[0];
        r[1] = M.r[1];
        r[2] = M.r[2];
        r[3] = M.r[3];
        return *this;
    }
};
typedef _XMMATRIX XMMATRIX;

inline void XMScalarSinCos(float *sine, float *cosine, float angle);
XMMATRIX XMMatrixLookToLH(XMVECTOR EyePosition, XMVECTOR EyeDirection, XMVECTOR UpDirection);

#endif
