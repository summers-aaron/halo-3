#ifndef PHYSICS_CHARACTER_PHYSICS_MODE_DEAD_H
#define PHYSICS_CHARACTER_PHYSICS_MODE_DEAD_H

#include "math/real_math_types.h"
#include "game/game_time_types.h"

struct s_character_physics_move_input_datum;
struct s_character_physics_move_output_datum;
struct s_character_physics_update_input_datum;
struct s_character_physics_update_output_datum;

// March methods belong to physics/character_physics_mode_dead.cpp.
// Ground-contact counters saturate at 254.
class c_character_physics_mode_dead_datum
{
public:
    // March 82c23438: no locals; the safe position uses the origin, the velocities the zero vector.
    __forceinline void initialize()
    {
        extern const real_vector3d *global_up3d;
        extern const real_point3d *global_origin3d;
        extern const real_vector3d *global_zero_vector3d;
        m_ground_plane = *global_up3d;
        m_object_relative_safe_position = *global_origin3d;
        m_contact_ticks = 0;
        m_ground_ticks = 0;
        m_slow_ticks = 0;
        m_fast_ticks = 0;
        m_resting_ticks = 0;
        m_contact_on_relaxable_surface = false;
        m_dead_size = 0;
        m_support_velocity = *global_zero_vector3d;
        m_death_time = -1;
        m_death_node_index = -1;
        m_death_node_acceleration = *global_zero_vector3d;
        m_ragdoll_start_time = -1;
    }
    // March 82c238a0.
    __forceinline void dispose() {}
    void notify_entered_ragdoll(bool entered);
    float get_ragdoll_transition_fraction();
    float get_ragdoll_z_offset_fraction();
    void update(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input);
    bool can_relax() const;
    bool on_relaxable_surface() const;
    void snap_facing(real_vector3d *forward, real_vector3d *up, const s_character_physics_update_input_datum *input);
    // March 82c24670.
    __forceinline void update_animation(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input) {}
    __forceinline void set_death_node_acceleration(long node_index, const real_vector3d *acceleration)
    {
        m_death_node_index = node_index;
        m_death_time = game_time_globals->time;
        m_death_node_acceleration = *acceleration;
    }
    bool get_death_node_acceleration(long *node_index, real_vector3d *acceleration);
    // March 82c247d0 / 82c24820.
    __forceinline void set_object_relative_safe_position(real_point3d *position) { m_object_relative_safe_position = *position; }
    __forceinline const real_point3d *get_object_relative_safe_position() const { return &m_object_relative_safe_position; }
    void move(s_character_physics_move_output_datum *output, const s_character_physics_move_input_datum *input);
    // March 82c24cd8: clear the two contact counters, not boolean aliases of them.
    __forceinline void notify_off_ground() { m_contact_ticks = 0; m_ground_ticks = 0; }
    __forceinline long get_dead_size() const { return m_dead_size; }
    // March 82c24df0.
    __forceinline const real_vector3d *get_resting_surface_normal() const
    {
        extern const real_vector3d *global_up3d;
        return m_contact_ticks ? &m_ground_plane : global_up3d;
    }

public: // Shared datum fields used by the inlined dead-motor operations.
    real_vector3d m_ground_plane;
    unsigned char m_ground_ticks;
    unsigned char m_contact_ticks;
    unsigned char m_slow_ticks;
    unsigned char m_fast_ticks;
    unsigned char m_resting_ticks;
    bool m_contact_on_relaxable_surface;
    unsigned char m_dead_size;
    real_vector3d m_support_velocity;
    real_point3d m_object_relative_safe_position;
    long m_ragdoll_start_time;
    long m_death_time;
    long m_death_node_index;
    real_vector3d m_death_node_acceleration;
};

typedef char character_physics_dead_size[(sizeof(c_character_physics_mode_dead_datum) == 0x44) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_MODE_DEAD_H
