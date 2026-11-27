#ifndef INCLUDE_PHYS_SWEPT_TRANSFORM_UTIL_SWEEP_H
#define INCLUDE_PHYS_SWEPT_TRANSFORM_UTIL_SWEEP_H

#include "phys/phys_base.h"

namespace phys_swept_transform_util
{
__forceinline void PHYS_CALL compute_sweep_info( const phys_motion_state& motion_state0, const phys_motion_state& motion_state1,
	phys_real body_delta_time, phys_vec4& sweep_out)
{
	const phys_swept_transform& swept_a = motion_state0.get_swept_transform();
	const phys_swept_transform& swept_b = motion_state1.get_swept_transform();

	phys_vec4 com_diff0; com_diff0.set_sub4( swept_a.m_center_of_mass0, swept_a.m_center_of_mass1 );
	phys_vec4 com_diff1; com_diff1.set_sub4( swept_b.m_center_of_mass1, swept_b.m_center_of_mass0 );

	phys_real f0 = body_delta_time * swept_a.get_inv_delta_time();
	phys_real f1 = body_delta_time * swept_b.get_inv_delta_time();


	sweep_out.set_mul4( f0, com_diff0 );
	sweep_out.add_scaled4( f1, com_diff1 );
	sweep_out(3) = f0 * motion_state0.m_delta_angle(3) * motion_state0.m_object_radius  + f1 * motion_state1.m_delta_angle(3) * motion_state1.m_object_radius;

}
}

#endif
