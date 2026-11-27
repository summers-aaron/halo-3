// hkmath/hkMotionState.obj: from the records 0x702b18
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x702b18 ?init_motion_state@phys_motion_state@@QAAXABVphys_vec4@@ABVphys_quat@@@Z
#define PHYS_CONFIG_SIMD 1


#include "phys/phys_base.h"





void phys_motion_state::init_motion_state( const phys_vec4& position, const phys_quat& rotation )
{
	m_swept_transform.init_swept_transform( position, rotation );

	get_transform().set( rotation, position );
	
	m_delta_angle.set_zero4();
	m_object_radius = 1.0f;
}

