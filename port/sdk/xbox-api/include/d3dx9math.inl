// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3dx9math.inl: member functions of the D3DX math types (element-wise copy assignment).
#ifndef __D3DX9MATH_INL__
#define __D3DX9MATH_INL__

#ifdef __cplusplus

D3DXINLINE D3DXVECTOR2 &D3DXVECTOR2::operator=(CONST D3DXVECTOR2 &v)
{
    x = v.x;
    y = v.y;
    return *this;
}

D3DXINLINE D3DXVECTOR3 &D3DXVECTOR3::operator=(CONST D3DXVECTOR3 &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

D3DXINLINE D3DXVECTOR4 &D3DXVECTOR4::operator=(CONST D3DXVECTOR4 &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return *this;
}

// Copies the sixteen elements in row order.
D3DXINLINE D3DXMATRIX &D3DXMATRIX::operator=(CONST D3DXMATRIX &mat)
{
    _11 = mat._11;
    _12 = mat._12;
    _13 = mat._13;
    _14 = mat._14;
    _21 = mat._21;
    _22 = mat._22;
    _23 = mat._23;
    _24 = mat._24;
    _31 = mat._31;
    _32 = mat._32;
    _33 = mat._33;
    _34 = mat._34;
    _41 = mat._41;
    _42 = mat._42;
    _43 = mat._43;
    _44 = mat._44;
    return *this;
}

D3DXINLINE D3DXQUATERNION &D3DXQUATERNION::operator=(CONST D3DXQUATERNION &q)
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
    return *this;
}

D3DXINLINE D3DXPLANE &D3DXPLANE::operator=(CONST D3DXPLANE &p)
{
    a = p.a;
    b = p.b;
    c = p.c;
    d = p.d;
    return *this;
}

D3DXINLINE D3DXCOLOR &D3DXCOLOR::operator=(CONST D3DXCOLOR &color)
{
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
    return *this;
}

#endif

#endif
