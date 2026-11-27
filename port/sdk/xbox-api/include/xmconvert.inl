// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xmconvert.inl: xboxmath conversion and store routines (half -> float, aligned vector stores), VMX32 evaluation.
#pragma once
#ifndef __XMCONVERT_INL__
#define __XMCONVERT_INL__

// Half to float, done on the integer bits: a normal half re-biases its exponent (15 -> 127), a denormal is shifted
// up until its leading one reaches the implicit-bit position (lowering the exponent once per shift), zero stays zero.
XMFINLINE float XMConvertHalfToFloat(HALF Value)
{
    unsigned int fraction = (unsigned int)(Value & 0x03FF);
    unsigned int exponent;
    unsigned int bits;

    if ((Value & 0x7C00) != 0) {
        exponent = (unsigned int)((Value >> 10) & 0x1F);
    } else if (fraction != 0) {
        exponent = 1;
        do {
            exponent--;
            fraction <<= 1;
        } while ((fraction & 0x0400) == 0);
        fraction &= 0x03FF;
    } else {
        exponent = (unsigned int)-112;
    }

    bits = ((Value & 0x8000) << 16) | ((exponent + 112) << 23) | (fraction << 13);
    return *(float *)&bits;
}

// 16-byte aligned store of a whole vector.
XMFINLINE void XMStoreVector4A(void *pDestination, XMVECTOR V)
{
    XMASSERT(pDestination);
    XMASSERT(((__SIZE_TYPE__)pDestination & 0xF) == 0);
    __stvx(V, pDestination, 0);
}

XMFINLINE void XMStoreFloat4A(XMFLOAT4A *pDestination, XMVECTOR V)
{
    XMStoreVector4A(pDestination, V);
}

#endif
