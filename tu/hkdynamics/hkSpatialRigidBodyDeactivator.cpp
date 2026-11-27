// hkdynamics/hkSpatialRigidBodyDeactivator.obj: from the records 0x64c2c0 0x64c2d8 0x64c2e0 0x64c388 0x64c4e8 0x64c508 0x64c3a8 0x64c448
// @flags /TP /O2 /Oc
#include "phys/phys_base_types.h"
// @address 0x64c388 ?rotation_length_squared@@YAMABVphys_quat@@0@Z
// @address 0x64c4e8 ?fixup_loaded_spatial_deactivator@@YAXPAX@Z
// @address 0x64c2c0 ?destroy_loaded_spatial_body_deactivator@@YAXPAX@Z
// @address 0x64c2d8 ?get_body_deactivator_type@phys_spatial_body_deactivator@@UBA?AW4sleep_detector_kind@phys_rigid_body_deactivator@@XZ
// @address 0x64c508 ?get_spatial_deactivator_vtable@@YAPBXXZ
// @address 0x64c2e0 ??0phys_spatial_body_deactivator@@QAA@XZ
// @address 0x64c3a8 ?should_sleep_high_freq@phys_spatial_body_deactivator@@UAA?AVphys_bool@@PBVphys_entity@@@Z
// @address 0x64c448 ?should_sleep_low_freq@phys_spatial_body_deactivator@@UAA?AVphys_bool@@PBVphys_entity@@@Z
#include <vectorintrinsics.h>
#include "phys/phys_spatial_body_deactivator.h"
#include "phys/phys_math.h"
#include "phys/phys_dynamics.h"



// 0x64c2e0
phys_spatial_body_deactivator::phys_spatial_body_deactivator()
{
    m_radius_sq = -1.0f;
    m_min_high_freq_translation = 0.01f;
    m_min_high_freq_rotation = 0.005f;
    m_min_low_freq_pos = 0.1f;
    m_min_low_freq_rot = 0.2f;
    m_hf_sample.m_ref_pos.flags_set_all(3.40282e+38f);
    m_low_freq_sample.m_ref_pos.flags_set_all(3.40282e+38f);
    m_hf_sample.m_ref_rot.set_to_identity();
    m_low_freq_sample.m_ref_rot.set_to_identity();
}

// 0x64c2c0
void destroy_loaded_spatial_body_deactivator(void *memory)
{
    static_cast<phys_spatial_body_deactivator *>(memory)->~phys_spatial_body_deactivator();
}

// 0x64c2d8
phys_rigid_body_deactivator::sleep_detector_kind phys_spatial_body_deactivator::get_body_deactivator_type() const
{
    return sleep_policy_spatial;
}

float rotation_length_squared(const phys_quat &first, const phys_quat &second)
{
    phys_vec4 difference;
    difference.set_sub4(first.m_quat_vec, second.m_quat_vec);
    float length_squared;
    __stvewx(__vmsum4fp(difference.m_simd_quad, difference.m_simd_quad), &length_squared, 0);
    return length_squared;
}

void fixup_loaded_spatial_deactivator(void *storage)
{
    phys_fixup_tag flag;
    new (storage) phys_spatial_body_deactivator(flag);
}

// 0x64c508
const void *get_spatial_deactivator_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_spatial_body_deactivator)];
    phys_fixup_tag flag;
    phys_spatial_body_deactivator *object = new (storage) phys_spatial_body_deactivator(flag);
    return *reinterpret_cast<const void *const *>(object);
}

phys_bool phys_spatial_body_deactivator::should_sleep_high_freq(const phys_entity *entity)
{
    const phys_motion_state *motion_state = entity->get_motion()->get_motion_state();
    const phys_swept_transform &swept_transform = motion_state->m_swept_transform;
    float radius = motion_state->m_object_radius;
    const phys_vec4 &position = swept_transform.m_center_of_mass1;
    const phys_quat &rotation = swept_transform.m_rotation_1;
    phys_vec4 difference;
    difference.set_sub4(m_hf_sample.m_ref_pos, position);
    float distance_sq = difference.length_sq_3d();
    float rotation_sq = rotation_length_squared(m_hf_sample.m_ref_rot, rotation) * radius * radius;
    if (distance_sq < m_min_high_freq_translation * m_min_high_freq_translation &&
        rotation_sq < m_min_high_freq_rotation * m_min_high_freq_rotation)
    {
        return true;
    }
    m_hf_sample.m_ref_pos = position;
    m_hf_sample.m_ref_rot = rotation;
    return false;
}

phys_bool phys_spatial_body_deactivator::should_sleep_low_freq(const phys_entity *entity)
{
    const phys_motion_state *motion_state = entity->get_motion()->get_motion_state();
    const phys_swept_transform &swept_transform = motion_state->m_swept_transform;
    float radius = motion_state->m_object_radius;
    const phys_vec4 &position = swept_transform.m_center_of_mass1;
    const phys_quat &rotation = swept_transform.m_rotation_1;
    phys_vec4 difference;
    difference.set_sub4(m_low_freq_sample.m_ref_pos, position);
    float distance_sq = difference.length_sq_3d();
    float rotation_sq = rotation_length_squared(m_low_freq_sample.m_ref_rot, rotation) * radius * radius;
    if (distance_sq < m_min_low_freq_pos * m_min_low_freq_pos &&
        rotation_sq < m_min_low_freq_rot * m_min_low_freq_rot)
    {
        return true;
    }
    m_low_freq_sample.m_ref_pos = position;
    m_low_freq_sample.m_ref_rot = rotation;
    return false;
}

