#ifndef INCLUDE_PHYS_MOTOR_H
#define INCLUDE_PHYS_MOTOR_H

#include "phys/phys_base.h"

// Constraint motors. Methods follow the March 2007 debug build (marchbody class hkConstraintMotor /
// hkLimitedForceConstraintMotor); data member and enumerator names are ours.
class phys_motor : public phys_ref_object {
public:
    enum motor_kind {
        motor_kind_invalid = 0,
        motor_kind_position = 1,
        motor_kind_velocity = 2,
        motor_kind_spring_damper = 3,
        motor_kind_callback = 4,
        k_motor_kind_count = 5
    };

    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_CONSTRAINT);

    __forceinline phys_motor() : m_motor_kind(motor_kind_invalid) {}
    __forceinline phys_motor(phys_fixup_tag) {}
    virtual phys_motor *clone() const = 0;
    __forceinline motor_kind get_type_id() const { return m_motor_kind; }

    phys_enum<motor_kind, signed char> m_motor_kind;
};

class phys_limited_force_motor : public phys_motor {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_CONSTRAINT);

    __forceinline phys_limited_force_motor() {}
    __forceinline phys_limited_force_motor(phys_fixup_tag flag) : phys_motor(flag) {}
    __forceinline void set_max_force(float force) {
        m_motor_min_force = -force;
        m_motor_max_force = force;
    }

    float m_motor_min_force;
    float m_motor_max_force;
};

class phys_position_motor : public phys_limited_force_motor {
public:
    phys_position_motor(float current_position);
    __forceinline phys_position_motor(phys_fixup_tag flag) : phys_limited_force_motor(flag) {}
    virtual phys_motor *clone() const;

    float m_tau_scale;
    float m_motor_damping;
    float m_proportional_recovery_vel;
    float m_const_recovery_vel;
};

class phys_spring_damper_motor : public phys_limited_force_motor {
public:
    phys_spring_damper_motor();
    phys_spring_damper_motor(float spring_constant, float spring_damping, float maximum_force);
    __forceinline phys_spring_damper_motor(phys_fixup_tag flag) : phys_limited_force_motor(flag) {}
    virtual phys_motor *clone() const;

    float m_spring_stiffness;
    float m_spring_damp;
};

#endif
