// hkmath/hkQsTransform.obj: from the records 0x703480 0x703628 0x703690 0x7036e0 0x703740 0x7038a8 0x703988 0x703a10 0x703ae0 0x703b28
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x703480 ?fast_renormalize_batch@phys_qs_transform@@SAXPAV1@PAMI@Z
// @address 0x703628 ?set_from_transform_unscaled@phys_qs_transform@@QAAXABVphys_transform@@@Z
// @address 0x703690 ?to_transform_unscaled@phys_qs_transform@@QBAXAAVphys_transform@@@Z
// @address 0x7036e0 ?is_stream_ok@phys_qs_transform@@QBA?AVphys_bool@@XZ
// @address 0x703740 ?is_approx_equal@phys_qs_transform@@QBA?AVphys_bool@@ABV1@M@Z
// @address 0x7038a8 ?get_4x4_column_major@phys_qs_transform@@QBAXPAM@Z
// @address 0x703988 ?set_4x4_column_major@phys_qs_transform@@QAA?AVphys_bool@@PBM@Z
// @address 0x703a10 ?set_to_inverse@phys_qs_transform@@QAAXABV1@@Z
// @address 0x703ae0 ?set_from_transform@phys_qs_transform@@QAAXABVphys_transform@@@Z
// @address 0x703b28 ?to_transform@phys_qs_transform@@QBAXAAVphys_transform@@@Z
#define PHYS_CONFIG_SIMD 1
#include "physics/havok_memory.h"

#include "phys/phys_base.h"
#include <float.h>

#include "phys/phys_math_util.h"
#include "phys/phys_vec4_util.h"
void phys_qs_transform::fast_renormalize_batch( phys_qs_transform* pose_out, phys_real* weight, unsigned int transform_count)
{
	for (unsigned i=0; i < transform_count; i++)
	{
		const phys_simd_scalar inv_weight = phys_simd_scalar::create(1.0f) / (phys_simd_scalar) weight[i];
		pose_out[i].m_translation.mul4(inv_weight);
		pose_out[i].m_scale.mul4(inv_weight);
	}

	phys_qs_transform* block_begin = pose_out;
	unsigned xform_quad_cnt = transform_count/4;
	for (unsigned i=0; i< xform_quad_cnt; i++)
	{
		phys_vec4 dot_products;
		phys_vec4_util::dot4_4x4(block_begin[0].m_rotation.m_quat_vec, block_begin[0].m_rotation.m_quat_vec,
			block_begin[1].m_rotation.m_quat_vec, block_begin[1].m_rotation.m_quat_vec,
			block_begin[2].m_rotation.m_quat_vec, block_begin[2].m_rotation.m_quat_vec,
			block_begin[3].m_rotation.m_quat_vec, block_begin[3].m_rotation.m_quat_vec,
			dot_products);
		phys_vec4 inv_sqrt_dots;
		inv_sqrt_dots.set_sqrt_inverse4(dot_products);

		block_begin[0].m_rotation.m_quat_vec.mul4(inv_sqrt_dots.get_simd_at(0));
		block_begin[1].m_rotation.m_quat_vec.mul4(inv_sqrt_dots.get_simd_at(1));
		block_begin[2].m_rotation.m_quat_vec.mul4(inv_sqrt_dots.get_simd_at(2));
		block_begin[3].m_rotation.m_quat_vec.mul4(inv_sqrt_dots.get_simd_at(3));

		block_begin += 4;
	}

	unsigned leftover_count = transform_count%4;
	for (unsigned j=0; j<leftover_count; j++)
	{
		block_begin[j].m_rotation.normalize();
	}

}

void phys_qs_transform::get_4x4_column_major(phys_real* d) const
{

	phys_rotation rot_matrix; rot_matrix.set (m_rotation);
	phys_rotation diag_scale_mat;
	diag_scale_mat.set_zero();
	diag_scale_mat.set_diagonal(m_scale(0), m_scale(1), m_scale(2));

	phys_rotation rot_scale;
	rot_scale.set_mul(rot_matrix, diag_scale_mat);

	phys_transform temp(rot_scale, m_translation);

	temp.get_4x4_column_major(d);

}

phys_bool phys_qs_transform::set_4x4_column_major(const phys_real* p)
{
	phys_math_util::mat_decomposition decomp_out;
	phys_math_util::decompose_4x4_col_xform(p, decomp_out);

	set(decomp_out.m_translation, decomp_out.m_rotation, decomp_out.m_scale);

	return !decomp_out.m_has_skew;

}

void phys_qs_transform::set_from_transform_unscaled (const phys_transform& transform)
{
	m_rotation.set(transform.get_rotation());
	m_translation = transform.get_translation();
	m_scale.flags_set_all(1.0f);
}

void phys_qs_transform::to_transform_unscaled (phys_transform& xform_out) const
{
	xform_out.set(m_rotation, m_translation);
}

void phys_qs_transform::set_from_transform (const phys_transform& transform)
{
	phys_real buffer[16];
	transform.get_4x4_column_major(buffer);
	set_4x4_column_major(buffer);
}

void phys_qs_transform::to_transform (phys_transform& xform_out) const
{
	phys_real buffer[16];
	get_4x4_column_major(buffer);
	xform_out.set_4x4_column_major(buffer);
}

phys_bool phys_qs_transform::is_stream_ok() const
{
	const phys_real* r = reinterpret_cast<const phys_real*>(this);
	for(int i=0; i<12; ++i)
	{

		if ((i==3) || (i==11)) continue;

		if( phys_math::is_finite(r[i]) == false )
		{
			return false;
		}
	}
	return true;
}

phys_bool phys_qs_transform::is_approx_equal( const phys_qs_transform& other, phys_real epsilon ) const
{

	if ( ! m_translation.approx_equal3( other.m_translation, epsilon ) )
	{
		return false;
	}

	if ( ! m_scale.approx_equal3(other.m_scale, epsilon) )
	{
		return false;
	}

	phys_vec4 this_quat_vec = m_rotation.m_quat_vec;
	phys_vec4 other_quat_vec = other.m_rotation.m_quat_vec;

	if (this_quat_vec(3) * other_quat_vec(3) < 0)
	{
		this_quat_vec.mul4(-1.0f);
	}

	return this_quat_vec.approx_equal4(other_quat_vec, epsilon) != false;
}



static __forceinline void phys_qs_transform_set_reciprocal4(phys_vec4* out, const phys_vec4& vector)
{
	__vector4 input = __lvx(&vector.get_quad(), 0);
	__vector4 estimate = __vrefp(input);
	__vector4 correction = __vnmsubfp(estimate, input, phys_simd_vec_1111);
	out->get_quad() = __vmaddfp(correction, estimate, estimate);
}

void phys_qs_transform::set_to_inverse(const phys_qs_transform& transform)
{
	m_translation.vec_set_rotated_inverse_dir(transform.m_rotation, transform.m_translation);
	m_translation.mul4(phys_simd_scalar(-1.0f));
	m_rotation.set_to_inverse(transform.m_rotation);
	phys_vec4 recip_scale;
	phys_qs_transform_set_reciprocal4(&recip_scale, transform.m_scale);
	m_scale.set_xyz0(recip_scale);
}
