// hkmath/hkMatrix6.obj: from the records 0x702d38 0x702d98 0x702ea8 0x702fb8 0x703070 0x703158 0x703478
// @compiler xdk6021
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x702d38 phys_mat6_set_transpose
// @address 0x702d98 phys_mat6_add
// @address 0x702ea8 phys_mat6_sub
// @address 0x702fb8 phys_mat6_set_mul
// @address 0x703158 ?set_inverted@phys_mat6@@QAAXABV1@@Z
// @address 0x703478 phys_mat6_set_invert
#define PHYS_CONFIG_SIMD 1
#include "phys/phys_math_types.h"
#include "phys/phys_mat3.h"
#include "phys/phys_mat6.h"


#include "phys/phys_base.h"


#define PHYS_MATH_MAT6_H



class phys_mat6;

#include "phys/phys_vec8.h"

#include "phys/phys_mat6_types.h"
#include "phys/phys_mat6_copy_inl.h"
void phys_mat6_add(phys_mat6& mat_a_out, const phys_mat6& b)
{
	mat_a_out.add(b);
}

void phys_mat6_sub(phys_mat6& mat_a_out, const phys_mat6& b)
{
	mat_a_out.sub(b);
}

void phys_mat6_set_mul(phys_mat6& out, const phys_mat6&a, const phys_mat6&b)
{
	out.set_mul(a,b);
}

void phys_mat6_set_transpose(phys_mat6& out, const phys_mat6&in)
{
	out.set_transpose(in);
}

void phys_mat6_set_invert(phys_mat6& out, const phys_mat6& in )
{
	out.set_inverted(in);
}

void phys_mat6::set_inverted( const phys_mat6& src_matrix )
{
	phys_mat6& out = *this;
	phys_mat6 in = src_matrix;
	out.set_to_identity();

	for (int row2 = 0; row2 < 2; row2++)
	{
		for (int block_row = 0; block_row < 3; block_row++)
		{
			{


				phys_real multiplier = 1.0f / in.m_blocks[row2][row2](block_row,block_row);

				in.m_blocks[0][row2].get_column(block_row).mul4( multiplier );
				in.m_blocks[1][row2].get_column(block_row).mul4( multiplier );

				out.m_blocks[0][row2].get_column(block_row).mul4( multiplier );
				out.m_blocks[1][row2].get_column(block_row).mul4( multiplier );
			}

			for (int col2 = 0; col2 < 2; col2++)
			{
				for (int block_col = 0; block_col < 3; block_col++)
				{
					if (col2 != row2 || block_col != block_row)
					{

						phys_real multiplier = - in.m_blocks[row2][col2](block_row,block_col);

						in.m_blocks[0][col2].get_column(block_col).add_scaled4( multiplier, in.m_blocks[0][row2].get_column(block_row) );
						in.m_blocks[1][col2].get_column(block_col).add_scaled4( multiplier, in.m_blocks[1][row2].get_column(block_row) );

						out.m_blocks[0][col2].get_column(block_col).add_scaled4( multiplier, out.m_blocks[0][row2].get_column(block_row) );
						out.m_blocks[1][col2].get_column(block_col).add_scaled4( multiplier, out.m_blocks[1][row2].get_column(block_row) );

					}
				}
			}
		}
	}
}


// @address 0x703070 phys_mat6_set_mul_vec
void phys_mat6_set_mul_vec(phys_vec8& out, const phys_mat6& a, const phys_vec8& b)
{
	out.set_mul_mat6(a, b);
}
