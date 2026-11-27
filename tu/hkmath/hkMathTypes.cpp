// hkmath/hkMathTypes.obj: from the records 0x612870 0x6128b0 0x612940 0x6129f8 0x612a88 0x70ae08 0x70b018
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x70ae08 ??__E?s_maskFromBits@phys_vec4_cmp@@2QBU__vector4@@B@@YAXXZ
// @address 0x70b018 ??__E?s_invMaskFromBits@phys_vec4_cmp@@2QBU__vector4@@B@@YAXXZ
// @address 0x612870 ?rand_unit@phys_math@@YAMXZ
// @address 0x6128b0 ?phys_floor@phys_math@@YAMM@Z
// @address 0x612940 ?phys_floor_to_int@phys_math@@YAHM@Z
// @address 0x6129f8 ?phys_float_to_int@phys_math@@YAHM@Z
// @address 0x612a88 phys_math_atan2f_approx
#include <math.h>
#include <ppcintrinsics.h>
#include <xtl.h>
#include "phys/phys_math_types.h"
#include "phys/phys_math.h"
// @address 0x724660 g_phys_rand01_seed



union phys_float_int_union {
    float f;
    int i;
    unsigned u;
};

float phys_math_atan2f_approx(float x, float y)
{
    float fx = phys_math::fabs(x);
    float fy = phys_math::fabs(y);

    float result;
    const float c2 = -0.121079f;
    const float c3 = 3.1415927f * 0.25f - 1.0f - c2;

    if (fx <= fy) {
        fy += 1.1920929e-7f;
        float a = fx / fy;
        result = a;
        result += c2 * a * a;
        result += c3 * a * a * a;
    } else {
        fx += 1.1920929e-7f;
        float a = fy / fx;
        result = a;
        result += c2 * a * a;
        result += c3 * a * a * a;
        result = 3.1415927f * 0.5f - result;
    }

    if (y < 0.0f) {
        result = 3.1415927f - result;
    }
    if (x < 0.0f) {
        result = -result;
    }
    return result;
}

float phys_math::rand_unit()
{
    g_phys_rand01_seed = (0x3039 - (g_phys_rand01_seed * 0x3e39b193)) & 0x7fffffff;
    return (float)(g_phys_rand01_seed & 0x7fffffff) * 4.656613e-10f;
}

float phys_math::phys_floor(float r)
{
    phys_float_int_union float_bits;
    float_bits.f = r;
    int frac_bit_mask = (int)0xff800000;
    unsigned exp = float_bits.u & 0x7f800000;
    exp >>= 0x17;
    exp -= 0x7f;
    int right_shift = 0x17 - int(exp);
    int sign = ~(right_shift >> 0x1f);
    right_shift &= sign;
    int abs_bits = float_bits.i & 0x7fffffff;
    abs_bits = (abs_bits - 1) >> 0x1f;
    float_bits.i &= ~abs_bits;
    frac_bit_mask >>= (0x17 & sign) - right_shift;
    frac_bit_mask |= ~sign;
    sign = int(float_bits.u & 0x80000000);
    sign >>= 0x1f;
    int add_bit_mask = 0x00800000;
    add_bit_mask >>= int(exp);
    add_bit_mask &= sign;
    add_bit_mask &= ~((float_bits.i & (~frac_bit_mask)) - 1);
    float_bits.i += add_bit_mask;
    float_bits.i &= frac_bit_mask;
    abs_bits = int(exp);
    abs_bits = abs_bits >> 0x1f;
    exp = (0xbf800000 & abs_bits) & sign;
    float_bits.i &= ~abs_bits;
    float_bits.u |= exp;
    return float_bits.f;
}

int phys_math::phys_floor_to_int(float r)
{
    phys_float_int_union float_bits;
    float_bits.f = r;
    int frac_bit_mask = (int)0xff800000;
    unsigned exp = float_bits.u & 0x7f800000;
    exp >>= 0x17;
    exp -= 0x7f;
    int left_shift = int(exp) - 0x17;
    int right_shift = 0x17 - int(exp);
    int sign = (left_shift - 1) >> 0x1f;
    left_shift &= ~sign;
    right_shift &= sign;
    int abs_bits = float_bits.i & 0x7fffffff;
    abs_bits = (abs_bits - 1) >> 0x1f;
    float_bits.i &= ~abs_bits;
    frac_bit_mask >>= (0x17 & sign) - right_shift;
    frac_bit_mask |= ~sign;
    sign = int(float_bits.u & 0x80000000);
    sign >>= 0x1f;
    int add_bit_mask = 0x00800000;
    add_bit_mask >>= int(exp);
    add_bit_mask &= sign;
    add_bit_mask &= ~((float_bits.i & (~frac_bit_mask)) - 1);
    abs_bits = int(exp);
    abs_bits = abs_bits >> 0x1f;
    exp = unsigned((0x00800000 & abs_bits) & sign);
    float_bits.i &= ~abs_bits;
    float_bits.i += add_bit_mask;
    float_bits.i &= 0x007fffff;
    float_bits.i |= (0x00800000 & ~abs_bits);
    float_bits.u |= exp;
    float_bits.i &= frac_bit_mask;
    int temp = (float_bits.i << 1) - 1;
    temp |= sign;
    float_bits.i = (temp - float_bits.i) + 1;
    float_bits.i >>= right_shift;
    float_bits.i <<= left_shift;
    return float_bits.i;
}

int phys_math::phys_float_to_int(float r)
{
    phys_float_int_union float_bits;
    float_bits.f = r;
    int frac_bit_mask = (int)0xff800000;
    unsigned int exp = float_bits.u & 0x7f800000;
    exp >>= 0x17;
    exp -= 0x7f;
    int left_shift = int(exp) - 0x17;
    int right_shift = 0x17 - int(exp);
    int sign = (left_shift - 1) >> 0x1f;
    left_shift &= ~sign;
    right_shift &= sign;
    int abs_bits = float_bits.i & 0x7fffffff;
    abs_bits = (abs_bits - 1) >> 0x1f;
    float_bits.i &= ~abs_bits;
    frac_bit_mask >>= (0x17 & sign) - right_shift;
    frac_bit_mask |= ~sign;
    sign = int(float_bits.u & 0x80000000);
    sign >>= 0x1f;
    float_bits.i &= 0x007fffff;
    float_bits.i |= 0x00800000;
    float_bits.i &= frac_bit_mask;
    int temp = (float_bits.i << 1) - 1;
    temp |= sign;
    float_bits.i = (temp - float_bits.i) + 1;
    abs_bits = int(exp);
    abs_bits = ~(abs_bits >> 0x1f);
    float_bits.i &= abs_bits;
    float_bits.i >>= right_shift;
    float_bits.i <<= left_shift;
    return float_bits.i;
}

const __vector4 phys_vec4_cmp::s_maskFromBits[CMP_MASK_XYZW + 1] = {
    XMVectorSelectControl(0, 0, 0, 0),
    XMVectorSelectControl(0, 0, 0, 1),
    XMVectorSelectControl(0, 0, 1, 0),
    XMVectorSelectControl(0, 0, 1, 1),
    XMVectorSelectControl(0, 1, 0, 0),
    XMVectorSelectControl(0, 1, 0, 1),
    XMVectorSelectControl(0, 1, 1, 0),
    XMVectorSelectControl(0, 1, 1, 1),
    XMVectorSelectControl(1, 0, 0, 0),
    XMVectorSelectControl(1, 0, 0, 1),
    XMVectorSelectControl(1, 0, 1, 0),
    XMVectorSelectControl(1, 0, 1, 1),
    XMVectorSelectControl(1, 1, 0, 0),
    XMVectorSelectControl(1, 1, 0, 1),
    XMVectorSelectControl(1, 1, 1, 0),
    XMVectorSelectControl(1, 1, 1, 1),
};

const __vector4 phys_vec4_cmp::s_invMaskFromBits[CMP_MASK_XYZW + 1] = {
    XMVectorSelectControl(1, 1, 1, 1),
    XMVectorSelectControl(1, 1, 1, 0),
    XMVectorSelectControl(1, 1, 0, 1),
    XMVectorSelectControl(1, 1, 0, 0),
    XMVectorSelectControl(1, 0, 1, 1),
    XMVectorSelectControl(1, 0, 1, 0),
    XMVectorSelectControl(1, 0, 0, 1),
    XMVectorSelectControl(1, 0, 0, 0),
    XMVectorSelectControl(0, 1, 1, 1),
    XMVectorSelectControl(0, 1, 1, 0),
    XMVectorSelectControl(0, 1, 0, 1),
    XMVectorSelectControl(0, 1, 0, 0),
    XMVectorSelectControl(0, 0, 1, 1),
    XMVectorSelectControl(0, 0, 1, 0),
    XMVectorSelectControl(0, 0, 0, 1),
    XMVectorSelectControl(0, 0, 0, 0),
};


