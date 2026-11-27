#ifndef INCLUDE_PHYS_SPATIAL_BODY_DEACTIVATOR_H
#define INCLUDE_PHYS_SPATIAL_BODY_DEACTIVATOR_H

#include "phys/phys_dynamics.h"

class phys_spatial_body_deactivator : public phys_rigid_body_deactivator {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_DEACTIVATOR);
    struct sleep_sample { phys_vec4 m_ref_pos; phys_quat m_ref_rot; };
    virtual ~phys_spatial_body_deactivator() {}
    virtual sleep_detector_kind get_body_deactivator_type() const;
    phys_spatial_body_deactivator();
    // March 0x834de8a8 uses the default base and sample constructors.
    __forceinline phys_spatial_body_deactivator(phys_fixup_tag) {}
    virtual phys_bool should_sleep_high_freq(const phys_entity *entity);
    virtual phys_bool should_sleep_low_freq(const phys_entity *entity);
    sleep_sample m_hf_sample;
    sleep_sample m_low_freq_sample;
    float m_radius_sq;
    float m_min_high_freq_translation;
    float m_min_high_freq_rotation;
    float m_min_low_freq_pos;
    float m_min_low_freq_rot;
};

typedef char phys_spatial_deactivator_size[(sizeof(phys_spatial_body_deactivator) == 0x70) ? 1 : -1];
typedef char phys_spatial_deactivator_high_offset[(offsetof(phys_spatial_body_deactivator, m_hf_sample) == 0x10) ? 1 : -1];
typedef char phys_spatial_deactivator_low_offset[(offsetof(phys_spatial_body_deactivator, m_low_freq_sample) == 0x30) ? 1 : -1];
typedef char phys_spatial_deactivator_radius_offset[(offsetof(phys_spatial_body_deactivator, m_radius_sq) == 0x50) ? 1 : -1];

float rotation_length_squared(const phys_quat &first, const phys_quat &second);
void fixup_loaded_spatial_deactivator(void *memory);

#endif
