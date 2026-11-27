// hkmath/hkSweptTransform.obj: from the records 0x7029a0 0x7029f0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x7029a0 ?init_swept_transform@phys_swept_transform@@QAAXABVphys_vec4@@ABVphys_quat@@@Z
// @address 0x7029f0 ?approx_body_transform_at@phys_swept_transform@@QBAXMAAVphys_transform@@@Z
#define PHYS_CONFIG_SIMD 1


#include "phys/phys_base.h"

void phys_swept_transform::approx_body_transform_at( float time, phys_transform& xform_out ) const
{
	approx_transform_at(time, xform_out );
}

void phys_swept_transform::init_swept_transform( const phys_vec4& position, const phys_quat& rotation )
{
	m_center_of_mass0.set_xyz0( position ); 
	m_center_of_mass1.set_xyz0( position ); 

	m_rotation_0 = rotation;
	m_rotation_1 = rotation;
	m_center_of_mass_local.set_zero4();
}

