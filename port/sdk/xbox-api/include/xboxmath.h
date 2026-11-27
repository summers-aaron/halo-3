// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xboxmath.h: the Xbox math library types (XMVECTOR, XMMATRIX, packed formats), its global constant vectors and the
// inline routines the game uses, in the library's VMX32 evaluation (the one this build selects).
#pragma once
#ifndef __XBOXMATH_H__
#define __XBOXMATH_H__

// Evaluation path: none of the x86 macros is defined and VMX128 is not advertised, so the VMX32 path is taken.
#if defined(_X86_) || defined(_AMD64_)
#define _NO_INTRINSICS_
#elif defined(__VMX128_SUPPORTED)
#define _VMX128_INTRINSICS_
#else
#define _VMX32_INTRINSICS_
#endif

#ifndef _NO_INTRINSICS_
#include <VectorIntrinsics.h>
#include <PPCIntrinsics.h>
#endif
#include <math.h>

#if !defined(_NO_INTRINSICS_) && defined(_VMX1_INTRINSICS_) && !defined(_VMX32_INTRINSICS_)
#define _VMX32_INTRINSICS_
#endif
#if !defined(_NO_INTRINSICS_) && !defined(_VMX32_INTRINSICS_) && !defined(_VMX128_INTRINSICS_)
#define _VMX128_INTRINSICS_
#endif

#ifndef XMINLINE
#ifdef XM_NO_MISALIGNED_VECTOR_ACCESS
#define XMINLINE __forceinline
#else
#define XMINLINE __inline
#endif
#endif

#ifndef XMFINLINE
#define XMFINLINE __forceinline
#endif

#if !defined(XMDEBUG) && defined(_DEBUG)
#define XMDEBUG
#endif

#ifndef XMASSERT
#ifdef XMDEBUG
// Debug builds only (never this port's): the assertion reporter is left to the program.
void XMAssert(const char *pExpression, const char *pFileName, unsigned int LineNumber);
#define XMASSERT(Expression) ((void)((Expression) || (XMAssert(#Expression, __FILE__, __LINE__), 0)))
#else
#define XMASSERT(Expression) ((void)0)
#endif
#endif

#ifndef _NOALIGNMENT_
#define _DECLSPEC_ALIGN_16_ __declspec(align(16))
#else
#define _DECLSPEC_ALIGN_16_
#endif

// Constants.
#define XM_PI 3.141592654f
#define XM_2PI 6.283185307f
#define XM_1DIVPI 0.318309886f
#define XM_1DIV2PI 0.159154943f
#define XM_PIDIV2 1.570796327f
#define XM_PIDIV4 0.785398163f

// Select-mask words and vperm byte indices of each element of the first (0) and second (1) operand.
#define XM_SELECT_0 0x00000000
#define XM_SELECT_1 0xFFFFFFFF
#define XM_PERMUTE_0X 0x00010203
#define XM_PERMUTE_0Y 0x04050607
#define XM_PERMUTE_0Z 0x08090A0B
#define XM_PERMUTE_0W 0x0C0D0E0F
#define XM_PERMUTE_1X 0x10111213
#define XM_PERMUTE_1Y 0x14151617
#define XM_PERMUTE_1Z 0x18191A1B
#define XM_PERMUTE_1W 0x1C1D1E1F

#define XM_CACHE_LINE_SIZE 128

// Types. The bit-field members below are allocated from the least significant bit up.
#if defined(_X86_) || defined(_AMD64_)
typedef unsigned int __vector4i[4];
#else
typedef __declspec(align(16)) unsigned int __vector4i[4];
#endif

typedef __vector4 XMVECTOR;
typedef __vector4i XMVECTORI;

// 4x4 matrix held as four row vectors.
#if defined(_X86_) || defined(_AMD64_)
typedef struct _XMMATRIX
#else
typedef _DECLSPEC_ALIGN_16_ struct _XMMATRIX
#endif
{
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
#ifdef __cplusplus
    _XMMATRIX() {}
    _XMMATRIX &operator=(const _XMMATRIX &M);
#endif
} XMMATRIX;

// IEEE-style half: sign, 5-bit biased exponent, 10-bit mantissa.
typedef unsigned short HALF;

// Normalized 3D vector packed into 32 bits as Z11 Y11 X10 (x in the low bits).
typedef struct _XMDHENN3 {
    union {
        struct {
            int x : 10;
            int y : 11;
            int z : 11;
        };
        unsigned int v;
    };
#ifdef __cplusplus
    _XMDHENN3() {}
    _XMDHENN3 &operator=(const _XMDHENN3 &DHenN3);
#endif
} XMDHENN3;

// Four floats, and the 16-byte aligned form.
typedef struct _XMFLOAT4 {
    float x;
    float y;
    float z;
    float w;
#ifdef __cplusplus
    _XMFLOAT4() {}
    _XMFLOAT4(float _x, float _y, float _z, float _w);
    _XMFLOAT4 &operator=(const _XMFLOAT4 &Float4);
#endif
} XMFLOAT4;

typedef __declspec(align(16)) XMFLOAT4 XMFLOAT4A, XMFLOAT4A16;

// Routines.
float XMConvertHalfToFloat(HALF Value);
void XMStoreVector4A(void *pDestination, XMVECTOR V);
void XMStoreFloat4A(XMFLOAT4A *pDestination, XMVECTOR V);
XMVECTOR XMVectorSet(float x, float y, float z, float w);
XMVECTOR XMVectorSelectControl(unsigned int VectorIndex0, unsigned int VectorIndex1, unsigned int VectorIndex2,
    unsigned int VectorIndex3);
void XMVectorSinCos(XMVECTOR *pSin, XMVECTOR *pCos, XMVECTOR V);
XMMATRIX XMMatrixMultiply(XMMATRIX M1, XMMATRIX M2);
XMMATRIX XMMatrixTranspose(XMMATRIX M);
XMMATRIX XMMatrixInverse(XMVECTOR *pDeterminant, XMMATRIX M);
XMMATRIX XMMatrixIdentity();
XMMATRIX XMMatrixLookToLH(XMVECTOR EyePosition, XMVECTOR EyeDirection, XMVECTOR UpDirection);
void XMScalarSinCos(float *pSin, float *pCos, float Value);

// Global constants, shared between translation units (one copy survives the link).
#define XMGLOBALCONST extern const __declspec(selectany)

// Taylor-series coefficients (terms 0..11) of sin and cos, then tan, arcsin, arctan and the estimate variants.
XMGLOBALCONST XMVECTOR g_XMSinCoefficients0 = {1.0f, -0.166666667f, 8.333333333e-3f, -1.984126984e-4f};
XMGLOBALCONST XMVECTOR g_XMSinCoefficients1 = {2.755731922e-6f, -2.505210839e-8f, 1.605904384e-10f, -7.647163732e-13f};
XMGLOBALCONST XMVECTOR g_XMSinCoefficients2 = {2.811457254e-15f, -8.220635247e-18f, 1.957294106e-20f, -3.868170171e-23f};
XMGLOBALCONST XMVECTOR g_XMCosCoefficients0 = {1.0f, -0.5f, 4.166666667e-2f, -1.388888889e-3f};
XMGLOBALCONST XMVECTOR g_XMCosCoefficients1 = {2.480158730e-5f, -2.755731922e-7f, 2.087675699e-9f, -1.147074560e-11f};
XMGLOBALCONST XMVECTOR g_XMCosCoefficients2 = {4.779477332e-14f, -1.561920697e-16f, 4.110317623e-19f, -8.896791392e-22f};
XMGLOBALCONST XMVECTOR g_XMTanCoefficients0 = {1.0f, 0.333333333f, 0.133333333f, 5.396825397e-2f};
XMGLOBALCONST XMVECTOR g_XMTanCoefficients1 = {2.186948854e-2f, 8.863235530e-3f, 3.592128167e-3f, 1.455834485e-3f};
XMGLOBALCONST XMVECTOR g_XMTanCoefficients2 = {5.900274264e-4f, 2.391290764e-4f, 9.691537707e-5f, 3.927832950e-5f};
XMGLOBALCONST XMVECTOR g_XMASinCoefficients0 = {-0.05806367563904f, -0.41861972469416f, 0.22480114791621f, 2.17337241360606f};
XMGLOBALCONST XMVECTOR g_XMASinCoefficients1 = {0.61657275907170f, 4.29696498283455f, -1.18942822255452f, -6.53784832094831f};
XMGLOBALCONST XMVECTOR g_XMASinCoefficients2 = {-1.36926553863413f, -4.48179294237210f, 1.41810672941833f, 5.48179257935713f};
XMGLOBALCONST XMVECTOR g_XMATanCoefficients0 = {1.0f, 0.333333334f, 0.2f, 0.142857143f};
XMGLOBALCONST XMVECTOR g_XMATanCoefficients1 = {1.111111111e-1f, 9.090909091e-2f, 7.692307692e-2f, 6.666666667e-2f};
XMGLOBALCONST XMVECTOR g_XMATanCoefficients2 = {5.882352941e-2f, 5.263157895e-2f, 4.761904762e-2f, 4.347826087e-2f};
XMGLOBALCONST XMVECTOR g_XMSinEstCoefficients = {1.0f, -1.66521856991541e-1f, 8.199913018755e-3f, -1.61475937228e-4f};
XMGLOBALCONST XMVECTOR g_XMCosEstCoefficients = {1.0f, -4.95348008918096e-1f, 3.878259962881e-2f, -9.24587976263e-4f};
XMGLOBALCONST XMVECTOR g_XMTanEstCoefficients = {2.484f, -1.954923183e-1f, 2.467401101f, XM_1DIVPI};
XMGLOBALCONST XMVECTOR g_XMATanEstCoefficients = {7.689891418951e-1f, 1.104742493348f, 8.661844266006e-1f, XM_PIDIV2};
XMGLOBALCONST XMVECTOR g_XMASinEstCoefficients = {-1.36178272886711f, 2.37949493464538f, -8.08228565650486e-1f, 2.78440142746736e-1f};
XMGLOBALCONST XMVECTOR g_XMASinEstConstants = {1.00000011921f, XM_PIDIV2, 0.0f, 0.0f};
XMGLOBALCONST XMVECTOR g_XMPiConstants0 = {XM_PI, XM_2PI, XM_1DIVPI, XM_1DIV2PI};

#if defined(_NO_INTRINSICS_) || defined(_VMX32_INTRINSICS_)
// Splatted scalars, IEEE bit patterns, identity rows, select masks and permute controls.
XMGLOBALCONST XMVECTORI g_XMNegativeZero = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
XMGLOBALCONST XMVECTOR g_XMOne = {1.0f, 1.0f, 1.0f, 1.0f};
XMGLOBALCONST XMVECTOR g_XMNegativeOne = {-1.0f, -1.0f, -1.0f, -1.0f};
XMGLOBALCONST XMVECTOR g_XMOneHalf = {0.5f, 0.5f, 0.5f, 0.5f};
XMGLOBALCONST XMVECTOR g_XMNegativeTwoPi = {-XM_2PI, -XM_2PI, -XM_2PI, -XM_2PI};
XMGLOBALCONST XMVECTOR g_XMNegativePi = {-XM_PI, -XM_PI, -XM_PI, -XM_PI};
XMGLOBALCONST XMVECTOR g_XMHalfPi = {XM_PIDIV2, XM_PIDIV2, XM_PIDIV2, XM_PIDIV2};
XMGLOBALCONST XMVECTOR g_XMPi = {XM_PI, XM_PI, XM_PI, XM_PI};
XMGLOBALCONST XMVECTOR g_XMReciprocalPi = {XM_1DIVPI, XM_1DIVPI, XM_1DIVPI, XM_1DIVPI};
XMGLOBALCONST XMVECTOR g_XMTwoPi = {XM_2PI, XM_2PI, XM_2PI, XM_2PI};
XMGLOBALCONST XMVECTOR g_XMReciprocalTwoPi = {XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI};
XMGLOBALCONST XMVECTOR g_XMEpsilon = {1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f};
XMGLOBALCONST XMVECTORI g_XMInfinity = {0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000};
XMGLOBALCONST XMVECTORI g_XMQNaN = {0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000};
XMGLOBALCONST XMVECTORI g_XMAbsMask = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
XMGLOBALCONST XMVECTORI g_XMFltMin = {0x00800000, 0x00800000, 0x00800000, 0x00800000};
XMGLOBALCONST XMVECTORI g_XMFltMax = {0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF};
XMGLOBALCONST XMVECTOR g_XMIdentityR0 = {1.0f, 0.0f, 0.0f, 0.0f};
XMGLOBALCONST XMVECTOR g_XMIdentityR1 = {0.0f, 1.0f, 0.0f, 0.0f};
XMGLOBALCONST XMVECTOR g_XMIdentityR2 = {0.0f, 0.0f, 1.0f, 0.0f};
XMGLOBALCONST XMVECTOR g_XMIdentityR3 = {0.0f, 0.0f, 0.0f, 1.0f};
XMGLOBALCONST XMVECTORI g_XMSelect1000 = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_0, XM_SELECT_0};
XMGLOBALCONST XMVECTORI g_XMSelect1100 = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
XMGLOBALCONST XMVECTORI g_XMSelect1110 = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_1, XM_SELECT_0};
XMGLOBALCONST XMVECTORI g_XMSelect0101 = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1};
XMGLOBALCONST XMVECTORI g_XMSwizzleXYXY = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0Y};
XMGLOBALCONST XMVECTORI g_XMSwizzleXYZX = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X};
XMGLOBALCONST XMVECTORI g_XMSwizzleYXZW = {XM_PERMUTE_0Y, XM_PERMUTE_0X, XM_PERMUTE_0Z, XM_PERMUTE_0W};
XMGLOBALCONST XMVECTORI g_XMSwizzleYZXW = {XM_PERMUTE_0Y, XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0W};
XMGLOBALCONST XMVECTORI g_XMSwizzleZXYW = {XM_PERMUTE_0Z, XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_0W};
XMGLOBALCONST XMVECTORI g_XMPermute0X0Y1X1Y = {XM_PERMUTE_0X, XM_PERMUTE_0Y, XM_PERMUTE_1X, XM_PERMUTE_1Y};
XMGLOBALCONST XMVECTORI g_XMPermute0Z0W1Z1W = {XM_PERMUTE_0Z, XM_PERMUTE_0W, XM_PERMUTE_1Z, XM_PERMUTE_1W};
#endif

#define XMMin(a, b) (((a) < (b)) ? (a) : (b))
#define XMMax(a, b) (((a) > (b)) ? (a) : (b))

#include <xmconvert.inl>
#include <xmvector.inl>
#include <xmmatrix.inl>
#include <xmmisc.inl>

#endif
