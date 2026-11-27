// hkmath/hkVector4.obj: from the records 0x6117f0 0x611848 0x611898 0x6118e8 0x611958 0x6119c8 0x611a10 0x611a60 0x611a98 0x611ae0 0x611b18 0x611ba8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6117f0 ?phys_vec4_is_ok@@YA?AVphys_bool@@PBMH@Z
// @address 0x611848 ?is_ok3@phys_vec4@@QBA?AVphys_bool@@XZ
// @address 0x611898 ?is_ok4@phys_vec4@@QBA?AVphys_bool@@XZ
// @address 0x6118e8 ?is_normalized3@phys_vec4@@QBA?AVphys_bool@@M@Z
// @address 0x611958 ?is_normalized4@phys_vec4@@QBA?AVphys_bool@@M@Z
// @address 0x6119c8 ?vec_set_transformed_pos@phys_vec4@@QAAXABVphys_transform@@ABV1@@Z
// @address 0x611a10 ?vec_set_transformed_inverse_pos@phys_vec4@@QAAXABVphys_transform@@ABV1@@Z
// @address 0x611a60 ?vec_set_rotated_dir@phys_vec4@@QAAXABVphys_rotation@@ABV1@@Z
// @address 0x611a98 ?vec_set_rotated_inverse_dir@phys_vec4@@QAAXABVphys_rotation@@ABV1@@Z
// @address 0x611ae0 ?set_mul3@phys_vec4@@QAAXABVphys_mat3@@ABV1@@Z
// @address 0x611b18 ?vec_set_transformed_pos@phys_vec4@@QAAXABVphys_qs_transform@@ABV1@@Z
// @address 0x611ba8 ?vec_set_transformed_inverse_pos@phys_vec4@@QAAXABVphys_qs_transform@@ABV1@@Z
#define PHYS_CONFIG_SIMD 1


#include "phys/phys_base.h"


 void phys_vec4::vec_set_transformed_pos(const phys_transform& t, const phys_vec4& v)
 {
    set_transformed_pos(t,v);
 }

 void phys_vec4::vec_set_transformed_inverse_pos(const phys_transform& t, const phys_vec4& v)
 {
    set_transformed_inverse_pos(t,v);
 }

 void phys_vec4::vec_set_transformed_pos(const phys_qs_transform& t, const phys_vec4& v)
 {
    set_transformed_pos(t,v);
 }

 void phys_vec4::vec_set_transformed_inverse_pos(const phys_qs_transform& t, const phys_vec4& v)
 {
    set_transformed_inverse_pos(t,v);
 }

 void phys_vec4::vec_set_rotated_dir(const phys_rotation& t, const phys_vec4& v)
 {
    set_rotated_dir(t,v);
 }

 void phys_vec4::vec_set_rotated_inverse_dir(const phys_rotation& t, const phys_vec4& v)
 {
    set_rotated_inverse_dir(t,v);
 }

 phys_bool phys_vec4_is_ok(const phys_real* f, int n)
 {
    for(int i=0; i<n; ++i)
    {
        if( phys_math::is_finite(f[i]) == false )
        {
            return false;
        }
    }
    return true;
 }

 phys_bool phys_vec4::is_ok3() const
 {
    const phys_real* f = reinterpret_cast<const phys_real*>(this);
    return phys_vec4_is_ok(f, 3);
 }

 phys_bool phys_vec4::is_ok4() const
 {
    const phys_real* f = reinterpret_cast<const phys_real*>(this);
    return phys_vec4_is_ok(f, 4);
 }


 phys_bool phys_vec4::is_normalized3(phys_real float_epsilon) const
 {
    if( is_ok3() )
    {
        phys_real len = this->length_sq_3d();
        return phys_math::fabs( len - 1.0f ) < float_epsilon;
    }
    return false;
 }

 phys_bool phys_vec4::is_normalized4(phys_real float_epsilon) const
 {
    if( is_ok4() )
    {
        phys_real len = this->length_sq_4d();
        return phys_math::fabs( len - 1.0f ) < float_epsilon;
    }
    return false;
 }

 void phys_vec4::set_mul3( const phys_mat3& a, const phys_vec4& b)
 {
    this->set_mul_mat3_vec3( a, b );
 }

