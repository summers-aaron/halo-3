// hkmath/hkMatrix3.obj: from the records 0x611c50 0x611ca0 0x611cb0 0x611ce8 0x611d48 0x611d88 0x611dd8 0x611ec8 0x611f58 0x612080 0x612148 0x612190 0x6121d8 0x612218 0x612270 0x612278 0x6122d0 0x612388 0x6123e8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x611c50 ?is_stream_ok@phys_mat3@@QBA?AVphys_bool@@XZ
// @address 0x611ca0 ?get_identity@phys_mat3@@SAABV1@XZ
// @address 0x611cb0 ?transpose_in_place@phys_mat3@@QAAXXZ
// @address 0x611ce8 ?set_transpose@phys_mat3@@QAAXABV1@@Z
// @address 0x611d48 ?set_mul@phys_mat3@@QAAXABVphys_simd_scalar@@ABV1@@Z
// @address 0x611d88 ?add_scaled@phys_mat3@@QAAXABVphys_simd_scalar@@ABV1@@Z
// @address 0x611dd8 ?is_approx_equal@phys_mat3@@QBA?AVphys_bool@@ABV1@M@Z
// @address 0x611ec8 ?set_cross_skew_symmetric@phys_mat3@@QAAXABVphys_vec4@@@Z
// @address 0x611f58 ?try_invert@phys_mat3@@QAA?AW4phys_result@@M@Z
// @address 0x612080 ?invert_symmetric@phys_mat3@@QAAXXZ
// @address 0x612148 ?add@phys_mat3@@QAAXABV1@@Z
// @address 0x612190 ?sub@phys_mat3@@QAAXABV1@@Z
// @address 0x6121d8 ?mul@phys_mat3@@QAAXABVphys_simd_scalar@@@Z
// @address 0x612218 phys_mat3_set_mul_mat3
// @address 0x612270 phys_mat3_invert_symmetric
// @address 0x612278 ?set_mul@phys_mat3@@QAAXABV1@0@Z
// @address 0x6122d0 ?set_mul_inverse@phys_mat3@@QAAXABV1@ABVphys_rotation@@@Z
// @address 0x612388 ?mul@phys_mat3@@QAAXABV1@@Z
// @address 0x6123e8 ?change_basis@phys_mat3@@QAAXABVphys_rotation@@@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_base.h"
#include <float.h>

#include "phys/phys_vec4_util.h"
#include "phys/phys_algorithm.h"
#include "phys/phys_mat3.h"
 phys_bool phys_mat3::is_stream_ok() const
 {
    const phys_real* f = reinterpret_cast<const phys_real*>(this);
    for(int i=0; i<12; ++i)
    {
        if( phys_math::is_finite(f[i]) == false )
        {
            return false;
        }
    }
    return true;
 }

 const phys_mat3& phys_mat3::get_identity()
 {
    return phys_transform::get_identity().get_rotation();
 }

 void phys_mat3::transpose_in_place()
 {
    phys_mat3& m = *this;
    phys_algorithm::swap( m(0,1), m(1,0) );
    phys_algorithm::swap( m(0,2), m(2,0) );
    phys_algorithm::swap( m(1,2), m(2,1) );
 }

 void phys_mat3_set_mul_mat3( phys_mat3* THIS, const phys_mat3& a_from_b, const phys_mat3& b_from_c )
 {
    phys_vec4_util::rotate_points( a_from_b, &b_from_c.get_column(0), 3, &THIS->get_column(0) );
 }

 void phys_mat3_invert_symmetric( phys_mat3& this_matrix )
 {
    this_matrix.invert_symmetric();
 }

 void phys_mat3::set_transpose( const phys_mat3& s )
 {
    phys_mat3& d = *this;
    d(0,0) = s(0,0);
    d(1,1) = s(1,1);
    d(2,2) = s(2,2);
    d(3,0) = 0.0f;
    d(3,1) = 0.0f;
    d(3,2) = 0.0f;
    d(1,0) = s(0,1);	
    d(0,1) = s(1,0);	
    d(2,0) = s(0,2);	
    d(0,2) = s(2,0);	
    d(2,1) = s(1,2);	
    d(1,2) = s(2,1);	
 }

 void phys_mat3::set_mul( const phys_mat3& a_from_b, const phys_mat3& b_from_c )
 {
    phys_vec4_util::rotate_points( a_from_b, &b_from_c.get_column(0), 3, &this->get_column(0) );
 }

 void phys_mat3::change_basis(const phys_rotation& r)
 {
    phys_rotation temp;
    temp.set_mul_inverse(*this, r);
    this->set_mul(r, temp);
 }

 void phys_mat3::set_mul_inverse( const phys_mat3& a_from_b, const phys_rotation& c_from_b )
 {
    phys_rotation b_from_c; b_from_c.set_transpose(c_from_b);
    phys_vec4_util::rotate_points( a_from_b, &b_from_c.get_column(0), 3, &this->get_column(0) );
 }

 void phys_mat3::set_mul( phys_simd_scalar_param scale, const phys_mat3& a)
 {
    get_column(0).set_mul4(scale, a.get_column(0));
    get_column(1).set_mul4(scale, a.get_column(1));
    get_column(2).set_mul4(scale, a.get_column(2));
 }

 void phys_mat3::add_scaled( phys_simd_scalar_param scale, const phys_mat3& a)
 {
    get_column(0).add_scaled4(scale, a.get_column(0));
    get_column(1).add_scaled4(scale, a.get_column(1));
    get_column(2).add_scaled4(scale, a.get_column(2));
 }

 phys_bool phys_mat3::is_approx_equal( const phys_mat3& m, phys_real zero) const
 {
    return	   m_column0.approx_equal3( m.get_column(0), zero )
            && m_column1.approx_equal3( m.get_column(1), zero )
            && m_column2.approx_equal3( m.get_column(2), zero );
 }

 void phys_mat3::set_cross_skew_symmetric( const phys_vec4& r )
 {
    get_column(0).set(  0   ,  r(2), -r(1) );
    get_column(1).set( -r(2),     0, +r(0) );
    get_column(2).set(  r(1), -r(0),     0 );
 }

 phys_result phys_mat3::try_invert(phys_real epsilon)
 {
    phys_vec4 r0; r0.set_cross( get_column(1), get_column(2) );
    phys_vec4 r1; r1.set_cross( get_column(2), get_column(0) );
    phys_vec4 r2; r2.set_cross( get_column(0), get_column(1) );

    phys_simd_scalar determinant = get_column(0).dot3(r0);
    
    if( phys_math::fabs(determinant) > epsilon * epsilon * epsilon)
    {
        phys_simd_scalar inv_det = phys_simd_scalar(1.0f) / determinant;
        get_column(0).set_mul4(inv_det, r0);
        get_column(1).set_mul4(inv_det, r1);
        get_column(2).set_mul4(inv_det, r2);
        phys_mat3& m = *this;
        phys_algorithm::swap( m(0,1), m(1,0) );
        phys_algorithm::swap( m(0,2), m(2,0) );
        phys_algorithm::swap( m(1,2), m(2,1) );
        return PHYS_SUCCESS;	
    }
    else
    {
        return PHYS_FAILURE;
    }    
 }

 void phys_mat3::invert_symmetric()
 {
    phys_vec4 r0; r0.set_cross( get_column(1), get_column(2) );
    phys_vec4 r1; r1.set_cross( get_column(2), get_column(0) );
    phys_vec4 r2; r2.set_cross( get_column(0), get_column(1) );

    phys_real determinant = get_column(0).dot3(r0);
    const phys_real float_epsilon = FLT_EPSILON*FLT_EPSILON*FLT_EPSILON;
    determinant = phys_math::max_of_two( float_epsilon, determinant );

    phys_simd_scalar inv_det = 1.0f / determinant;

    get_column(0).set_mul4(inv_det, r0);
    get_column(1).set_mul4(inv_det, r1);
    get_column(2).set_mul4(inv_det, r2);
 }

 void phys_mat3::add( const phys_mat3& a )
 {
    get_column(0).add4( a.get_column(0) );
    get_column(1).add4( a.get_column(1) );
    get_column(2).add4( a.get_column(2) );
 }

 void phys_mat3::sub( const phys_mat3& a )
 {
    get_column(0).sub4( a.get_column(0) );
    get_column(1).sub4( a.get_column(1) );
    get_column(2).sub4( a.get_column(2) );
 }

 void phys_mat3::mul( const phys_mat3& a )
 {

    phys_mat3 temp;
    temp.set_mul( *this, a );
    *this = temp;
 }

 void phys_mat3::mul( phys_simd_scalar_param scale)
 {
    get_column(0).mul4(scale);
    get_column(1).mul4(scale);
    get_column(2).mul4(scale);
 }

