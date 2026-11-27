// hkmath/hkTransform.obj: from the records 0x612430 0x612480 0x6124d0 0x612520 0x6125d0 0x612650 0x6126c0 0x612760 0x612820
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x612430 ?get_4x4_column_major@phys_transform@@QBAXPAM@Z
// @address 0x612480 ?set_4x4_column_major@phys_transform@@QAAXPBM@Z
// @address 0x6124d0 ?is_stream_ok@phys_transform@@QBA?AVphys_bool@@XZ
// @address 0x612520 ?is_approx_equal@phys_transform@@QBA?AVphys_bool@@ABV1@M@Z
// @address 0x6125d0 ?set_to_inverse@phys_transform@@QAAXABV1@@Z
// @address 0x612650 ?set_mul@phys_transform@@QAAXABV1@0@Z
// @address 0x6126c0 ?set_mul_inverse_mul@phys_transform@@QAAXABV1@0@Z
// @address 0x612760 ?set_mul_eq@phys_transform@@QAAXABV1@@Z
// @address 0x612820 ?set_mul_mul_inverse@phys_transform@@QAAXABV1@0@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_base.h"
#include <float.h>


#include "phys/phys_vec4_util.h"
#include "phys/phys_math_types.h"

// Port link (P5): globals owned by hkmath:hkTransform.obj (data-owners.tsv).
// Contents match the reference image exactly (default.pe, file offset = rva).
// @address 0x724620 ?phys_transform_identity@@3PAMA
float phys_transform_identity[] = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};
// @address 0x724660 g_phys_rand01_seed
int g_phys_rand01_seed = 0x219;

void phys_transform::get_4x4_column_major(phys_real* d) const
{
	const phys_real* p = &m_rotation(0,0);
	for (int i = 0; i<4; i++)
	{
		d[0] = p[0];
		d[1] = p[1];
		d[2] = p[2];
		d[3] = 0.0f;
		d+= 4;
		p+= 4;
	}
	d[-1] = 1.0f;
}

void phys_transform::set_4x4_column_major(const phys_real* p)
{
	phys_real* d = &m_rotation(0,0);
	for (int i = 0; i<4; i++)
	{
		d[0] = p[0];
		d[1] = p[1];
		d[2] = p[2];
		d[3] = 0.0f;
		d+= 4;
		p+= 4;
	}
	d[-1] = 1.0f;
}

void phys_transform::set_to_inverse( const phys_transform& t )
{
	m_rotation.set_transpose( t.get_rotation() );

	phys_vec4 tr;	tr.set_neg4(t.get_translation());
	m_translation.set_rotated_dir( m_rotation, tr);
}

void phys_transform::set_mul( const phys_transform& a_from_b, const phys_transform& b_from_c )
{

	phys_vec4_util::rotate_points( a_from_b.get_rotation(), &b_from_c.get_rotation().get_column(0), 4, &this->get_rotation().get_column(0) );
	m_translation.add4( a_from_b.m_translation);
}

void phys_transform::set_mul_inverse_mul( const phys_transform& b_from_a, const phys_transform &b_from_c )
{
	phys_vec4_util::rotate_inverse_points( b_from_a.get_rotation(), &b_from_c.get_rotation().get_column(0), 3, &this->get_rotation().get_column(0) );
	phys_vec4 h; h.set_sub4(b_from_c.m_translation, b_from_a.m_translation );
	m_translation.set_rotated_inverse_dir( b_from_a.m_rotation, h);
}

void phys_transform::set_mul_eq( const phys_transform& b )
{
	phys_transform at = *this;
	set_mul( at, b );
}

phys_bool phys_transform::is_stream_ok() const
{
	const phys_real* r = reinterpret_cast<const phys_real*>(this);
	for(int i=0; i<16; ++i)
	{
		if( phys_math::is_finite(r[i]) == false )
		{
			return false;
		}
	}
	return true;
}

void phys_transform::set_mul_mul_inverse( const phys_transform &world_from_a, const phys_transform &world_from_b )
{

	phys_transform h;
	h.set_to_inverse( world_from_b );
	this->set_mul( world_from_a, h );
}


phys_bool phys_transform::is_approx_equal( const phys_transform& t, phys_real zero ) const
{
	return		m_rotation.is_approx_equal( t.get_rotation(), zero )
		&&	m_translation.approx_equal3( t.get_translation(), zero );
}

