#ifndef INCLUDE_PHYS_MAT6_TYPES_H
#define INCLUDE_PHYS_MAT6_TYPES_H

#include "phys/phys_vec8.h"

class phys_mat6
{
	public:

		PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_MATH, phys_mat6);

		__forceinline phys_mat6() { }
		phys_mat6(const phys_mat6 &o);

		__forceinline void operator= ( const phys_mat6& a )
		{
			m_blocks[0][0] = a.m_blocks[0][0];
			m_blocks[0][1] = a.m_blocks[0][1];
			m_blocks[1][0] = a.m_blocks[1][0];
			m_blocks[1][1] = a.m_blocks[1][1];
		}

		__forceinline void set_zero()
		{
			m_blocks[0][0].set_zero();
			m_blocks[0][1].set_zero();
			m_blocks[1][0].set_zero();
			m_blocks[1][1].set_zero();
		}

		__forceinline void set_to_identity()
		{
			m_blocks[0][0].set_to_identity();
			m_blocks[0][1].set_zero();
			m_blocks[1][0].set_zero();
			m_blocks[1][1].set_to_identity();
		}

		__forceinline void set_transpose( const phys_mat6& s )
		{
			m_blocks[0][0].set_transpose( s.m_blocks[0][0] );
			m_blocks[0][1].set_transpose( s.m_blocks[1][0] );
			m_blocks[1][0].set_transpose( s.m_blocks[0][1] );
			m_blocks[1][1].set_transpose( s.m_blocks[1][1] );
		}
		
		__forceinline void set_mul( const phys_mat6& a, const phys_mat6& b )
		{
			phys_mat3 tmp;
			
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					tmp      .set_mat3_product( a.m_blocks[i][0], b.m_blocks[0][j] );
					m_blocks[i][j].set_mat3_product( a.m_blocks[i][1], b.m_blocks[1][j] );
					m_blocks[i][j].accumulate_mat3( tmp );
				}
			}
		}

		__forceinline void add( const phys_mat6& a )
		{
			m_blocks[0][0].accumulate_mat3( a.m_blocks[0][0] );
			m_blocks[0][1].accumulate_mat3( a.m_blocks[0][1] );
			m_blocks[1][0].accumulate_mat3( a.m_blocks[1][0] );
			m_blocks[1][1].accumulate_mat3( a.m_blocks[1][1] );
		}

		__forceinline void sub( const phys_mat6& a )
		{
			m_blocks[0][0].subtract_mat3( a.m_blocks[0][0] );
			m_blocks[0][1].subtract_mat3( a.m_blocks[0][1] );
			m_blocks[1][0].subtract_mat3( a.m_blocks[1][0] );
			m_blocks[1][1].subtract_mat3( a.m_blocks[1][1] );
		}

		__forceinline void mul( const phys_mat6& a)
		{
			phys_mat6 temp;
			temp.set_mul( *this, a );
			*this = temp;
		}

		__forceinline void mul( phys_simd_scalar_param scale )
		{
			m_blocks[0][0].set_mat3_product(scale, m_blocks[0][0]);
			m_blocks[0][1].set_mat3_product(scale, m_blocks[0][1]);
			m_blocks[1][0].set_mat3_product(scale, m_blocks[1][0]);
			m_blocks[1][1].set_mat3_product(scale, m_blocks[1][1]);
		}

		void set_inverted( const phys_mat6& in );

	public:

		__declspec(align(16)) phys_mat3 m_blocks[2][2];

};

void phys_vec8::set_mul_mat6(const phys_mat6& a, const phys_vec8& b )
{
	phys_vec4 scratch0;
	phys_vec4 scratch1;
	scratch0.set_mul_mat3_vec3( a.m_blocks[0][0], b.m_lin_block );
	m_lin_block.set_mul_mat3_vec3( a.m_blocks[0][1], b.m_ang_half );
	scratch1.set_mul_mat3_vec3( a.m_blocks[1][0], b.m_lin_block );
	m_ang_half.set_mul_mat3_vec3( a.m_blocks[1][1], b.m_ang_half );
	m_lin_block.add4(scratch0);
	m_ang_half.add4(scratch1);
}



typedef char phys_mat6_size[(sizeof(phys_mat6) == 192) ? 1 : -1];

#endif
