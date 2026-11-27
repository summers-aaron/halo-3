// blamlibxbox/visibility_test.obj: from the records 0x156de0
// @flags /TP /O2 /Oc
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
// match: 0x156de0: keep the prior declaration before the helper definitions; check 7 lost provenance without it.
#include "visibility/visibility_test.h"



static __forceinline unsigned int visibility_test_vector4_equal_int_r(__vector4 a, __vector4 b)
{
    unsigned int cr = 0;
    __vcmpequwR(a, b, &cr);
    return cr;
}

#define visibility_test_comparison_all_true(cr) (((cr) & 0x80) != 0)

static __forceinline __vector4 visibility_test_negate(__vector4 v)
{
    __vector4 sign_mask = __vspltisw(-1);
    sign_mask = __vslw(sign_mask, sign_mask);
    return __vxor(v, sign_mask);
}

// @address 0x156de0 ?visibility_volume_test_sphere_vmx@@YAJU__vector4@@0000000@Z
long visibility_volume_test_sphere_vmx(__vector4 center, __vector4 radius, __vector4 plane0, __vector4 plane1,
    __vector4 plane2, __vector4 plane3, __vector4 plane4, __vector4 plane5)
{
    __vector4 distance0 = __vmsum4fp(center, plane0);
    __vector4 distance1 = __vmsum4fp(center, plane1);
    __vector4 distance2 = __vmsum4fp(center, plane2);
    __vector4 distance3 = __vmsum4fp(center, plane3);
    __vector4 distance4 = __vmsum4fp(center, plane4);
    __vector4 distance5 = __vmsum4fp(center, plane5);
    __vector4 outside = __vcmpgtfp(distance0, radius);
    outside = __vor(outside, __vcmpgtfp(distance1, radius));
    outside = __vor(outside, __vcmpgtfp(distance2, radius));
    outside = __vor(outside, __vcmpgtfp(distance3, radius));
    outside = __vor(outside, __vcmpgtfp(distance4, radius));
    outside = __vor(outside, __vcmpgtfp(distance5, radius));
    __vector4 inside = __vcmpgtfp(visibility_test_negate(radius), distance0);
    inside = __vand(inside, __vcmpgtfp(visibility_test_negate(radius), distance1));
    inside = __vand(inside, __vcmpgtfp(visibility_test_negate(radius), distance2));
    inside = __vand(inside, __vcmpgtfp(visibility_test_negate(radius), distance3));
    inside = __vand(inside, __vcmpgtfp(visibility_test_negate(radius), distance4));
    inside = __vand(inside, __vcmpgtfp(visibility_test_negate(radius), distance5));
    if (visibility_test_comparison_all_true(visibility_test_vector4_equal_int_r(outside, __vspltisw(-1))))
        return 0;
    return visibility_test_comparison_all_true(visibility_test_vector4_equal_int_r(inside, __vspltisw(-1))) ? 2 : 1;
}
