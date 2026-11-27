#ifndef MATH_D3DX_MATRIX_H
#define MATH_D3DX_MATRIX_H

struct D3DXMATRIX
{
    float m[4][4];
};
struct D3DXPLANE
{
    float a, b, c, d;
};
extern "C" D3DXPLANE *D3DXPlaneTransform(D3DXPLANE *out, const D3DXPLANE *plane, const D3DXMATRIX *matrix);
extern "C" D3DXMATRIX *D3DXMatrixTranspose(D3DXMATRIX *out, const D3DXMATRIX *in);
extern "C" D3DXMATRIX *D3DXMatrixInverse(D3DXMATRIX *out, float *determinant, const D3DXMATRIX *in);

#endif
