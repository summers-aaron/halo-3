#ifndef RASTERIZER_XENON_XENON_MATRIX_H
#define RASTERIZER_XENON_XENON_MATRIX_H

typedef __vector4 XMVECTOR;
struct _XMMATRIX;
typedef _XMMATRIX XMMATRIX;
XMMATRIX XMMatrixInverse(XMVECTOR *pDeterminant, XMMATRIX M);
extern "C" XMVECTOR xmmatrix_determinant(XMMATRIX matrix);

#endif
