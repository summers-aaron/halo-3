// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xmvector.inl: xboxmath vector routines (construction, select masks) and the member functions of the vector
// storage types, VMX32 evaluation.
#pragma once
#ifndef __XMVECTOR_INL__
#define __XMVECTOR_INL__

// Build a vector from four floats (VMX32: element stores).
XMFINLINE XMVECTOR XMVectorSet(float x, float y, float z, float w)
{
    XMVECTOR V;
    V.v[0] = x;
    V.v[1] = y;
    V.v[2] = z;
    V.v[3] = w;
    return V;
}

// Select mask for __vsel: element i is all ones (take the second operand) when VectorIndexi is 1, else zero.
XMFINLINE XMVECTOR XMVectorSelectControl(unsigned int VectorIndex0, unsigned int VectorIndex1,
    unsigned int VectorIndex2, unsigned int VectorIndex3)
{
    XMVECTOR mask;
    const unsigned int ControlElement[] = {XM_SELECT_0, XM_SELECT_1};

    XMASSERT(VectorIndex0 < 2);
    XMASSERT(VectorIndex1 < 2);
    XMASSERT(VectorIndex2 < 2);
    XMASSERT(VectorIndex3 < 2);

    mask.u[0] = ControlElement[VectorIndex0];
    mask.u[1] = ControlElement[VectorIndex1];
    mask.u[2] = ControlElement[VectorIndex2];
    mask.u[3] = ControlElement[VectorIndex3];
    return mask;
}

#ifdef __cplusplus

XMFINLINE _XMDHENN3 &_XMDHENN3::operator=(const _XMDHENN3 &DHenN3)
{
    v = DHenN3.v;
    return *this;
}

XMFINLINE _XMFLOAT4::_XMFLOAT4(float _x, float _y, float _z, float _w)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

XMFINLINE _XMFLOAT4 &_XMFLOAT4::operator=(const _XMFLOAT4 &Float4)
{
    x = Float4.x;
    y = Float4.y;
    z = Float4.z;
    w = Float4.w;
    return *this;
}

#endif

#endif
