// hkmath/hkRotation.obj: from the records 0x7018c0 0x7019a8 0x7019e8 0x701ae8
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x7018c0 ?set@phys_rotation@@QAAXABVphys_quat@@@Z
// @address 0x7019a8 ?set_axis_angle@phys_rotation@@QAAXABVphys_vec4@@M@Z
// @address 0x7019e8 ?is_orthonormal@phys_rotation@@QBA?AVphys_bool@@M@Z
// @address 0x701ae8 ?renormalize_axes@phys_rotation@@QAAXXZ
#define PHYS_CONFIG_SIMD 1


#include "phys/phys_base.h"

void phys_rotation::set(const phys_quat& qi)
{

	const phys_vec4& q = qi.m_quat_vec;

	phys_real x2 = q(0) + q(0);
	phys_real y2 = q(1) + q(1);
	phys_real z2 = q(2) + q(2);

	phys_real xx = q(0)*x2;
	phys_real xy = q(0)*y2;
	phys_real xz = q(0)*z2;
	phys_real yy = q(1)*y2;
	phys_real yz = q(1)*z2;
	phys_real zz = q(2)*z2;

	phys_real w_x2 = q(3)*x2;
	phys_real w_y2 = q(3)*y2;
	phys_real w_z2 = q(3)*z2;

	get_column(0).set(1.0f-(yy+zz),	xy+w_z2,			xz-w_y2);
	get_column(1).set(xy-w_z2,			1.0f-(xx+zz),	yz+w_x2);
	get_column(2).set(xz+w_y2,			yz-w_x2,			1.0f-(xx+yy));
}

void phys_rotation::set_axis_angle(const phys_vec4& axis, phys_real angle)
{
	phys_quat q;
	q.set_axis_angle( axis, angle );
	this->set(q);
}

phys_bool phys_rotation::is_orthonormal( phys_real epsilon ) const
{

	const phys_real snap_dot = m_column0.dot3(m_column0);
	if( phys_math::fabs( snap_dot - 1.0f) > epsilon)
	{
		return false;
	}

	const phys_real edge_dot = m_column1.dot3(m_column1);
	if( phys_math::fabs( edge_dot - 1.0f) > epsilon)
	{
		return false;
	}

	const phys_real col2_len_sq = m_column2.dot3(m_column2);
	if( phys_math::fabs( col2_len_sq - 1.0f) > epsilon)
	{
		return false;
	}

	phys_vec4 col0_cross_col1;
	col0_cross_col1.set_cross(m_column0, m_column1);

	return col0_cross_col1.approx_equal3(m_column2, epsilon) != false;
}

void phys_rotation::renormalize_axes()
{
	phys_quat q; q.set_and_normalize(*this);
	this->set(q);
}

