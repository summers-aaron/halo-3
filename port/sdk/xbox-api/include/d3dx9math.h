// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3dx9math.h: D3DX math types (vectors, matrix, quaternion, plane, color) and the D3DX matrix / plane routines the
// game calls (implemented by port/sdk/d3d/d3dx_math.cpp).
#include "d3dx9.h"

#ifndef __D3DX9MATH_H__
#define __D3DX9MATH_H__

#include <math.h>
#ifndef _NO_XBOXMATH
#include <xboxmath.h>
#endif

#ifndef D3DXINLINE
#define D3DXINLINE __forceinline
#endif

#define D3DX_PI ((FLOAT)3.141592654f)
#define D3DX_1BYPI ((FLOAT)0.318309886f)

// Each type below has a user-provided default constructor and copy assignment, which the ABI sees: they are
// returned through a hidden pointer like the other non-trivial classes.
#ifdef __cplusplus

typedef struct D3DXVECTOR2 {
public:
    D3DXVECTOR2() {}
    D3DXVECTOR2 &operator=(CONST D3DXVECTOR2 &);
public:
    FLOAT x, y;
} D3DXVECTOR2, *LPD3DXVECTOR2;

typedef struct D3DXVECTOR3 : public D3DVECTOR {
public:
    D3DXVECTOR3() {}
    D3DXVECTOR3 &operator=(CONST D3DXVECTOR3 &);
} D3DXVECTOR3, *LPD3DXVECTOR3;

typedef struct D3DXVECTOR4 {
public:
    D3DXVECTOR4() {}
    D3DXVECTOR4 &operator=(CONST D3DXVECTOR4 &);
public:
    FLOAT x, y, z, w;
} D3DXVECTOR4, *LPD3DXVECTOR4;

typedef struct D3DXMATRIX : public D3DMATRIX {
public:
    D3DXMATRIX() {}
    D3DXMATRIX &operator=(CONST D3DXMATRIX &);
} D3DXMATRIX, *LPD3DXMATRIX;

typedef struct D3DXQUATERNION {
public:
    D3DXQUATERNION() {}
    D3DXQUATERNION &operator=(CONST D3DXQUATERNION &);
    FLOAT x, y, z, w;
} D3DXQUATERNION, *LPD3DXQUATERNION;

typedef struct D3DXPLANE {
public:
    D3DXPLANE() {}
    D3DXPLANE &operator=(CONST D3DXPLANE &);
    FLOAT a, b, c, d;
} D3DXPLANE, *LPD3DXPLANE;

typedef struct D3DXCOLOR {
public:
    D3DXCOLOR() {}
    D3DXCOLOR &operator=(CONST D3DXCOLOR &);
    FLOAT r, g, b, a;
} D3DXCOLOR, *LPD3DXCOLOR;

#else

typedef struct D3DXVECTOR2 {
    FLOAT x, y;
} D3DXVECTOR2, *LPD3DXVECTOR2;
typedef struct _D3DVECTOR D3DXVECTOR3, *LPD3DXVECTOR3;
typedef struct D3DXVECTOR4 {
    FLOAT x, y, z, w;
} D3DXVECTOR4, *LPD3DXVECTOR4;
typedef struct _D3DMATRIX D3DXMATRIX, *LPD3DXMATRIX;
typedef struct D3DXQUATERNION {
    FLOAT x, y, z, w;
} D3DXQUATERNION, *LPD3DXQUATERNION;
typedef struct D3DXPLANE {
    FLOAT a, b, c, d;
} D3DXPLANE, *LPD3DXPLANE;
typedef struct D3DXCOLOR {
    FLOAT r, g, b, a;
} D3DXCOLOR, *LPD3DXCOLOR;

#endif

#ifdef __cplusplus
extern "C" {
#endif

// pOut = transpose of pM.
D3DXMATRIX *WINAPI D3DXMatrixTranspose(D3DXMATRIX *pOut, CONST D3DXMATRIX *pM);

// pOut = inverse of pM; the determinant is stored through pDeterminant when it is not NULL. Returns NULL when pM
// is singular.
D3DXMATRIX *WINAPI D3DXMatrixInverse(D3DXMATRIX *pOut, FLOAT *pDeterminant, CONST D3DXMATRIX *pM);

// pOut = plane pP transformed by pM (pass the inverse transpose of the point transform).
D3DXPLANE *WINAPI D3DXPlaneTransform(D3DXPLANE *pOut, CONST D3DXPLANE *pP, CONST D3DXMATRIX *pM);

#ifdef __cplusplus
}
#endif

#include "d3dx9math.inl"

#endif
