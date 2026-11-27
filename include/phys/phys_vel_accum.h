#ifndef INCLUDE_PHYS_VEL_ACCUM_H
#define INCLUDE_PHYS_VEL_ACCUM_H

#include "phys/phys_math.h"

class phys_vel_accum {
public:
    enum phys_accum_type {
        PHYS_RIGID_BODY,
        PHYS_VEL_ACCUM_KEYFRAMED_BODY,
        PHYS_NO_GRAVITY_RIGID_BODY,
        PHYS_VEL_ACCUM_END
    };

    phys_enum<phys_accum_type, unsigned char> m_type;
    phys_bool m_mat_is_identity;
    unsigned int m_deactivation_counter;
    unsigned int m_deactivation_class;
    phys_vec4 m_lin_vel;
    phys_vec4 m_ang_vel;
    phys_vec4 m_inv_masses;
    phys_vec4 m_scratch_0;
    phys_vec4 m_scratch_1;
    phys_vec4 m_scratch_2;
    phys_vec4 m_scratch_3;

    __forceinline phys_vel_accum() {}
    phys_vel_accum &operator=(const phys_vel_accum &other);
    __forceinline phys_vec4 &get_center_of_mass_in_world() { return m_scratch_0; }
    __forceinline const phys_vec4 &get_center_of_mass_in_world() const { return m_scratch_0; }
    __forceinline phys_rotation &get_core_from_world() { return reinterpret_cast<phys_rotation &>(m_scratch_1); }
    __forceinline const phys_rotation &get_core_from_world() const { return reinterpret_cast<const phys_rotation &>(m_scratch_1); }
    __forceinline phys_vec4 &get_sum_lin_vel() { return m_scratch_0; }
    __forceinline const phys_vec4 &get_sum_lin_vel() const { return m_scratch_0; }
    __forceinline phys_vec4 &get_sum_ang_vel() { return m_scratch_1; }
    __forceinline const phys_vec4 &get_sum_ang_vel() const { return m_scratch_1; }
    __forceinline void make_fixed_accumulator() {
        m_type = PHYS_VEL_ACCUM_KEYFRAMED_BODY;
        m_mat_is_identity = true;
        m_inv_masses.set_zero4();
        m_lin_vel.set_zero4();
        m_ang_vel.set_zero4();
        get_center_of_mass_in_world().set_zero4();
        get_core_from_world().set_zero();
    }
};

typedef char phys_vel_accum_struct_size[(sizeof(phys_vel_accum) == 128) ? 1 : -1];
typedef char phys_vel_accum_counter_offset[(offsetof(phys_vel_accum, m_deactivation_counter) == 4) ? 1 : -1];
typedef char phys_vel_accum_class_offset[(offsetof(phys_vel_accum, m_deactivation_class) == 8) ? 1 : -1];
typedef char phys_vel_accum_lin_vel_offset[(offsetof(phys_vel_accum, m_lin_vel) == 16) ? 1 : -1];
typedef char phys_vel_accum_scratch_offset[(offsetof(phys_vel_accum, m_scratch_0) == 64) ? 1 : -1];

#endif
