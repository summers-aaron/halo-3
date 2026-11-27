#ifndef INCLUDE_PHYS_COLLISION_QUALITY_INFO_H
#define INCLUDE_PHYS_COLLISION_QUALITY_INFO_H

#include "phys/phys_base_types.h"

struct phys_collision_quality_info {
    float m_keep_contact;
    float m_create_4d_contact;
    float m_create_contact;
    float m_manifold_sweep_dist;
    phys_bool m_use_ccd;
    float m_min_separation;
    float m_min_extra_separation;
    float m_min_safe_dt;
    float m_min_abs_safe_dt;
    float m_toi_separation;
    float m_toi_extra_separation;
    float m_toi_accuracy;
    float m_max_constraint_violation;
    float m_min_toi_dt;
    unsigned short m_constraint_priority;
    phys_bool m_toi_weld_reject;
    __forceinline float compute_min_separation(float cur_distance) const { return m_min_separation; }
};
typedef char phys_collide_size_quality[(sizeof(phys_collision_quality_info) == 0x3c) ? 1 : -1];


#endif
