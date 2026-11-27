#ifndef PHYSICS_CHARACTER_PHYSICS_MODE_MELEE_H
#define PHYSICS_CHARACTER_PHYSICS_MODE_MELEE_H

// Functions defined in tu/blamlibxbox/character_physics_mode_melee.cpp that other units call.

#include "cseries/cseries.h"
#include "cseries/cseries_wrapped.h"
#include "math/real_math_types.h"

struct s_character_physics_update_output_datum;
struct s_character_physics_update_input_datum;

struct s_melee_clamp_data
{
    unsigned long flags;
    float maximum;
    long target_unit_index;
    long count;
    real_vector3d initial_aiming_vector;
    real_vector3d initial_velocity;                         // +28
};

class c_character_physics_mode_melee_datum
{
public:
    __forceinline void initialize();
    __forceinline bool is_initialized() const { return ((m_flags.get_unsafe() >> 2) & 1) != 0; }
    __forceinline long get_ticks_remaining() const { return m_ticks_remaining; }
    __forceinline bool is_fast() const { return (m_flags.get_unsafe() & 1) != 0; }
    __forceinline bool has_suppressed_camera() const { return ((m_flags.get_unsafe() >> 4) & 1) != 0; }
    __forceinline long get_exit_invisible_crouched_uber_melee_offset() const { return m_exit_invisible_crouched_uber_melee_offset; }
    __forceinline bool is_running_invisible_crouched_uber_melee() const { return (m_flags.get_unsafe() & (1 << 4)) != 0; }
    __forceinline bool is_invisible_crouched_uber_melee_physics_initialized() const { return (m_flags.get_unsafe() & (1 << 5)) != 0; }
    __forceinline void start_invisible_crouched_uber_melee(long offset)
    {
        m_exit_invisible_crouched_uber_melee_offset = offset;
        m_flags.set_unsafe(m_flags.get_unsafe() | (1 << 5));
    }
    __forceinline void decrement_invisible_crouched_uber_melee_offset()
    {
        if (m_exit_invisible_crouched_uber_melee_offset != 0)
            m_exit_invisible_crouched_uber_melee_offset += m_exit_invisible_crouched_uber_melee_offset > 0 ? -1 : 1;
    }
    __forceinline long get_target_unit_index() const { return m_target_unit_index; }
    __forceinline const real_vector3d *get_initial_aiming_vector() const { return &m_initial_aiming_vector; }
    __forceinline void clear_target_unit_index() { m_target_unit_index = NONE; }
    __forceinline void set_flag(long bit, bool value)
    {
        if (value)
            m_flags.set_unsafe(m_flags.get_unsafe() | (1 << bit));
        else
            m_flags.set_unsafe(m_flags.get_unsafe() & ~(1 << bit));
    }
    __forceinline void setup_dash(bool forced, bool has_target, bool invisible_crouched_uber_melee, long target_unit_index, long count,
        const real_vector3d *position, const real_vector3d *velocity)
    {
        m_target_unit_index = target_unit_index;
        m_count = count;
        m_initial_aiming_vector = *position;
        m_initial_velocity = *velocity;
        set_flag(0, forced);
        set_flag(1, has_target);
        set_flag(4, invisible_crouched_uber_melee);
        set_flag(8, true);
    }
    __forceinline void exit_invisible_crouched_uber_melee(long offset)
    {
        m_exit_invisible_crouched_uber_melee_offset = offset;
        m_flags.set_unsafe(m_flags.get_unsafe() & ~((1 << 4) | (1 << 5)));
    }
    void update(s_character_physics_update_output_datum *physics_output, const s_character_physics_update_input_datum *physics_input);

private:
    void update_internal(s_character_physics_update_output_datum *physics_output, float time_step, float duration, bool enabled,
        const real_vector3d *velocity, const real_point3d *position, const real_vector3d *forward, const real_vector3d *target_velocity,
        const real_point3d *target);

    enum e_flags
    {
        _fast_bit,
        _lead_bit,
        _initialized_bit,
        _clamped_bit,
        _invisible_crouched_uber_melee_bit,
        _invisible_crouched_uber_melee_physics_initialized_bit,
        _decelerating_bit,
        _finished_bit
    };
    c_flags_no_init<e_flags, unsigned long, 8> m_flags;       // +0
    float m_maximum;                                        // +4
    long m_target_unit_index;                               // +8
    long m_count;                                           // +12
    real_vector3d m_initial_aiming_vector;                   // +16
    real_vector3d m_initial_velocity;                        // +28
    float m_initial_speed;                                  // +40
    real_point3d m_target;                                   // +44
    real_vector3d m_direction;                               // +56
    real_vector3d m_velocity;                                // +68
    float m_distance;                                       // +80
    float m_rate;                                           // +84
    long m_ticks_remaining;                                 // +88
    long m_elapsed_ticks;                                   // +92
    long m_duration_ticks;                                  // +96
    float m_minimum_speed;                                  // +100
    float m_deceleration;                                   // +104
    long m_exit_invisible_crouched_uber_melee_offset;         // +108
};

struct s_rate_input
{
    unsigned long flags;                                    // +0
    float maximum;                                          // +4
    long target_unit_index;                                  // +8
    long count;                                             // +12
};

extern "C" bool character_physics_mode_melee_clamp_velocity(const s_melee_clamp_data *data, real_vector3d *velocity);



#endif // PHYSICS_CHARACTER_PHYSICS_MODE_MELEE_H
