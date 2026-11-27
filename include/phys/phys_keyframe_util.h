#ifndef INCLUDE_PHYS_KEYFRAME_UTIL_H
#define INCLUDE_PHYS_KEYFRAME_UTIL_H

#include "phys/phys_base.h"

class phys_rigid_body;

class phys_keyframe_util
{
	public:

		struct accel_info
		{
			PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_UTILITIES, phys_keyframe_util::accel_info );

			accel_info();

			phys_vec4 m_lin_pos_factor;

			phys_vec4 m_ang_pos_factor;

			phys_vec4 m_lin_vel_factor;

			phys_vec4 m_ang_vel_factor;

			phys_real m_max_lin_accel;
				
			phys_real m_max_ang_accel; 

			phys_real m_max_allowed_dist;

		};

		struct KeyFrameInfo
		{
			PHYS_DECLARE_NONVIRTUAL_ALLOCATOR( PHYS_MEM_UTILITIES, phys_keyframe_util::KeyFrameInfo );

			phys_vec4 m_position;
				
			phys_quat m_key_orientation;

			phys_vec4 m_linear_velocity;

			phys_vec4 m_ang_velocity;

			void set_from_position_orientation(  const phys_vec4& cur_position, const phys_quat& cur_orientation,
				const phys_vec4& keyframe_next_pos, const phys_quat& keyframe_next_orient, phys_real inv_delta_time );

			void fast_set_from_pos_orient(  const phys_vec4& cur_position, const phys_quat& cur_orientation,
				const phys_vec4& keyframe_next_pos, const phys_quat& keyframe_next_orient, phys_real inv_delta_time );
		};

		static void PHYS_CALL apply_soft_keyframe( const KeyFrameInfo& keyframe_info, accel_info& keyframe_accel_info,
			phys_real body_delta_time, phys_real inv_delta_time, phys_rigid_body* body );

		static void PHYS_CALL apply_hard_keyframe( const phys_vec4& keyframe_next_pos, const phys_quat& keyframe_next_orient,
			phys_real inv_delta_time, phys_rigid_body* body);

		static void PHYS_CALL apply_hard_keyframe_async( const phys_vec4& keyframe_next_pos, const phys_quat& keyframe_next_orient,
			phys_real inv_delta_time, phys_rigid_body* body);

};

typedef char havok_size_hkKeyFrameUtility_KeyFrameInfo[(sizeof(phys_keyframe_util::KeyFrameInfo) == 64) ? 1 : -1];
typedef char havok_size_hkKeyFrameUtility_AccelerationInfo[(sizeof(phys_keyframe_util::accel_info) == 80) ? 1 : -1];

#endif // HAVOK_HK_KEY_FRAME_UTILITY_H
