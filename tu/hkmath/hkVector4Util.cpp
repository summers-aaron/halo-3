// hkmath/hkVector4Util.obj: from the records 0x702b78 0x702c30 0x702ce8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x702b78 ?float_to_i16_floor_bias@phys_vec4_util@@YAMXZ
// @address 0x702c30 ?float_to_i32_floor_bias@phys_vec4_util@@YAMXZ
// @address 0x702ce8 ?pack_quat_to_u32@phys_vec4_util@@YAIABVphys_vec4@@@Z
#define PHYS_CONFIG_SIMD 1

#include "phys/phys_base.h"
#include <float.h>

#include "phys/phys_vec4_util.h"
phys_real phys_vec4_util::float_to_i16_floor_bias()
{
	phys_real l = 10.0f;
	phys_real h = 11.0f;
	phys_vec4 one; one.flags_set_all( 1.0f );

	for (int i = 0; i < 23; i++ )
	{
		phys_real m = (l+h) * 0.5f;
		phys_vec4 p; p.flags_set_all( m );

		phys_int_union64 out;
		convert_to_u16( p, phys_vec4::get_zero(), one, out );

		if ( out.as_u16[0] < 11 )
		{
			l = m;
		}
		else
		{
			h = m;
		}
	}
	return (l+h) * 0.5f - 11;
}

phys_real phys_vec4_util::float_to_i32_floor_bias()
{
	phys_real l = 10.0f;
	phys_real h = 11.0f;
	phys_vec4 one; one.flags_set_all( 1.0f );

	for (int i = 0; i < 23; i++ )
	{
		phys_real m = (l+h) * 0.5f;
		phys_vec4 p; p.flags_set_all( m );

		unsigned int out[4];
		convert_to_u32( p, phys_vec4::get_zero(), one, out );

		if ( out[0] < 11 )
		{
			l = m;
		}
		else
		{
			h = m;
		}
	}
	return (l+h) * 0.5f - 11;
}

#define pack_range_limit 1.1f

// @address 0x21180 ?scale@?1??pack_quat_to_u32@phys_vec4_util@@YAIABVphys_vec4@@@Z@4U__vector4@@B
// @address 0x21190 ?bias@?1??pack_quat_to_u32@phys_vec4_util@@YAIABVphys_vec4@@@Z@4U__vector4@@B
// @address 0x1790 ?magic@?1??convert_to_u16@phys_vec4_util@@YAXABVphys_vec4@@00AATphys_int_union64@@@Z@4QBMB
unsigned int phys_vec4_util::pack_quat_to_u32( phys_vec4_param quat_in )
{
	static const phys_simd_vec scale = { 116.363632f, 116.363632f, 116.363632f, 116.363632f };
	static const phys_simd_vec bias = { 196736.5f, 196736.5f, 196736.5f, 196736.5f };
	phys_vec4 x; x.set_mul4(quat_in, reinterpret_cast<const phys_vec4&>(scale));
	phys_vec4 y; y.set_add4( x,  reinterpret_cast<const phys_vec4&>(bias) );

	union 
	{
		phys_simd_vec q;
		unsigned int i[4];
		float f[4];
	} u;

	u.q = y.get_quad();

	int a = u.i[0] >> 6;
	int b = u.i[1] >> 6;
	int c = u.i[2] >> 6;
	int d = u.i[3] >> 6;
	a &= 0xff;
	b &= 0xff;
	c &= 0xff;
	d &= 0xff;
	a |= b <<8;
	c |= d <<8;
	a |= c << 16;
	return a;
}

