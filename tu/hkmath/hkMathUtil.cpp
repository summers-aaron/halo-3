// hkmath/hkMathUtil.obj: from the records 0x703b68 0x704038 0x704040
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x703b68 ?decompose_4x4_col_xform@phys_math_util@@YAXPBMAAUmat_decomposition@1@@Z
// @address 0x704038 ?decompose_matrix@phys_math_util@@YAXABVphys_mat4@@AAUmat_decomposition@1@@Z
// @address 0x704040 ?decompose_matrix@phys_math_util@@YAXABVphys_mat3@@AAUmat_decomposition@1@@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_base.h"
#include <float.h>


#include "phys/phys_math_util.h"
void PHYS_CALL phys_math_util::decompose_matrix (const class phys_mat4& matrix_in, mat_decomposition& matrix_decomp_out)
{
	decompose_4x4_col_xform((const phys_real*) &matrix_in, matrix_decomp_out);
}

void PHYS_CALL phys_math_util::decompose_matrix (const phys_mat3& matrix_in, mat_decomposition& matrix_decomp_out)
{
	
	phys_mat4 full_matrix;
	full_matrix.set_cols(matrix_in.get_column(0), matrix_in.get_column(1), matrix_in.get_column(2), phys_vec4::get_zero());

	decompose_matrix (full_matrix, matrix_decomp_out);
}

void PHYS_CALL phys_math_util::decompose_4x4_col_xform (const phys_real* matrix_in, phys_math_util::mat_decomposition& matrix_decomp_out)
{
	
	const int polar_max_iters = 30; 
	const phys_real tolerance = FLT_EPSILON;

	phys_transform t;
	t.set_4x4_column_major(matrix_in);

	{
		matrix_decomp_out.m_translation = t.get_translation();
	}

	{
		phys_rotation q_cur, q_next, q_inv_t;

		int simplex_iter_count = 0;

		static_cast<phys_mat3&>(q_next) = t.get_rotation();
		do
		{
			static_cast<phys_mat3&>(q_cur) = q_next;

			static_cast<phys_mat3&>(q_inv_t) = q_cur;
			q_inv_t.try_invert(tolerance);
			q_inv_t.transpose_in_place();

			phys_mat3 temp;
			temp = q_cur;
			temp.add(q_inv_t);
			q_next.set_mul( 0.5f, temp );	

			simplex_iter_count++;
		} while((simplex_iter_count<polar_max_iters) && (!q_next.is_approx_equal( q_cur, tolerance )));

		static_cast<phys_mat3&>(matrix_decomp_out.m_basis_rotation) = q_next;
	}

	{
		phys_vec4 c1 = matrix_decomp_out.m_basis_rotation.get_column(0);
		c1.normalize_xyz();
		phys_vec4 c2 = matrix_decomp_out.m_basis_rotation.get_column(1);
		c2.normalize_xyz();
		phys_vec4 c3 = matrix_decomp_out.m_basis_rotation.get_column(2);
		c3.normalize_xyz();

		phys_vec4 r0; r0.set_cross( c2, c3 );
		const phys_simd_scalar determinant = c1.dot3(r0);

		if (determinant < 0.0f )
		{
			
			c1.mul4(-1.0f);

		}

		matrix_decomp_out.m_has_flip = (determinant<0.0f);

		{
			phys_rotation rot;
			rot.set_cols( c1, c2, c3 );
			matrix_decomp_out.m_rotation = phys_quat(rot);
			matrix_decomp_out.m_rotation.normalize();
		}

	}

	{
		phys_quat inv_rot_quat; inv_rot_quat.set_to_inverse(matrix_decomp_out.m_rotation);
		phys_rotation inv_rot_mat; inv_rot_mat.set (inv_rot_quat);

		matrix_decomp_out.m_scale_and_skew.set_mul(inv_rot_mat, t.get_rotation());
	}
	
	{

		matrix_decomp_out.m_scale.set( matrix_decomp_out.m_scale_and_skew(0,0), matrix_decomp_out.m_scale_and_skew(1,1),
			matrix_decomp_out.m_scale_and_skew(2,2) );

		phys_vec4 one; one.flags_set_all(1.0f);
		matrix_decomp_out.m_has_scale = ! matrix_decomp_out.m_scale.approx_equal3( one );
	}

	{
		
		phys_mat3 inv_scale_mat; inv_scale_mat.set_diagonal(1.0f/matrix_decomp_out.m_scale(0), 1.0f/matrix_decomp_out.m_scale(1),
			1.0f/matrix_decomp_out.m_scale(2));
		matrix_decomp_out.m_skew_mat.set_mul(inv_scale_mat, matrix_decomp_out.m_scale_and_skew);
		
		matrix_decomp_out.m_has_skew = !matrix_decomp_out.m_skew_mat.is_approx_equal(phys_transform::get_identity().get_rotation());
	}
}

