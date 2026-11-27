// port/sdk/vmx (both hosts): the out-of-line XNA math (xboxmath.h) functions the game links to rather than inlines.
// Built like the game (the MS ABI and __vector4 of the XDK's vectorintrinsics.h), so the C++ names are the game's.
//
// XMVectorSinCos (?XMVectorSinCos@@YAXPAU__vector4@@0U1@@Z, the retail copy at 0x11a738 that
// matrix3x3_rotation_from_angles calls): the XDK's _VMX128_INTRINSICS_ evaluation (xmvector.inl), which is what the
// Xbox ran, transcribed operation for operation over the port's VMX128 intrinsics (vmx128.cpp), so it computes what
// the game's inlined copies of the XDK code compute on this port. The angle is reduced to [-pi, pi] as
// V - 2pi * round(V / 2pi) (vmulfp, vrfin, vnmsubfp); sin is then the odd Taylor series to V^23 and cos the even one
// to V^22 with the XDK's coefficients, the powers by vmulfp and each term added by vmaddfp. Lanes are independent.
// Saved films replay this arithmetic, so it must not be replaced by the host's sinf / cosf.
#include <vectorintrinsics.h>

namespace {

__vector4 splat(float value)
{
    __vector4 v;
    v.v[0] = v.v[1] = v.v[2] = v.v[3] = value;
    return v;
}

// g_XMSinCoefficients0..2 and g_XMCosCoefficients0..2 (xboxmath.h), term 0 (the 1) first.
const float k_sin[12] = {1.0f, -0.166666667f, 8.333333333e-3f, -1.984126984e-4f, 2.755731922e-6f, -2.505210839e-8f,
    1.605904384e-10f, -7.647163732e-13f, 2.811457254e-15f, -8.220635247e-18f, 1.957294106e-20f, -3.868170171e-23f};
const float k_cos[12] = {1.0f, -0.5f, 4.166666667e-2f, -1.388888889e-3f, 2.480158730e-5f, -2.755731922e-7f,
    2.087675699e-9f, -1.147074560e-11f, 4.779477332e-14f, -1.561920697e-16f, 4.110317623e-19f, -8.896791392e-22f};
const float k_two_pi = 6.283185307f;            // XM_2PI (g_XMPiConstants0.y)
const float k_reciprocal_two_pi = 0.159154943f; // XM_1DIV2PI (g_XMPiConstants0.w)

} // namespace

void XMVectorSinCos(__vector4 *pSin, __vector4 *pCos, __vector4 V)
{
    __vector4 VN = __vmulfp(V, splat(k_reciprocal_two_pi));
    VN = __vrfin(VN);
    __vector4 V1 = __vnmsubfp(splat(k_two_pi), VN, V);
    // powers: V2 = V1 V1, V3 = V2 V1, V4 = V2 V2, then V(2k) = Vk Vk and V(2k+1) = V(k+1) Vk, as xmvector.inl
    __vector4 power[24];
    power[1] = V1;
    power[2] = __vmulfp(V1, V1);
    power[3] = __vmulfp(power[2], V1);
    power[4] = __vmulfp(power[2], power[2]);
    for (int i = 5; i <= 23; i++) {
        int half = i / 2;
        power[i] = (i & 1) ? __vmulfp(power[half + 1], power[half]) : __vmulfp(power[half], power[half]);
    }
    __vector4 Cos = __vmaddfp(splat(k_cos[1]), power[2], splat(1.0f));
    __vector4 Sin = __vmaddfp(splat(k_sin[1]), power[3], V1);
    for (int term = 2; term <= 11; term++) {
        Cos = __vmaddfp(splat(k_cos[term]), power[2 * term], Cos);
        Sin = __vmaddfp(splat(k_sin[term]), power[2 * term + 1], Sin);
    }
    *pCos = Cos;
    *pSin = Sin;
}
