#ifndef PHYSICS_CHARACTER_PHYSICS_COMPONENT_H
#define PHYSICS_CHARACTER_PHYSICS_COMPONENT_H

#include "physics/character_physics_mode_flying.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/character_physics_mode_melee.h"

struct s_character_physics_move_output_datum;
struct s_character_physics_move_input_datum;
struct s_character_physics_update_output_datum;
struct s_character_physics_update_input_datum;
union real_vector3d;

class c_character_physics_mode_ground_datum;
class c_character_physics_mode_sentinel_datum;

// Character physics state shared by the active movement modes.
class c_character_physics_component {
public:
    // March: get_mode asserts IN_RANGE_INCLUSIVE(m_mode, _mode_first, _mode_last); set_mode skips the dispose
    // switch when m_mode is 0. Mode 5 is the second sentinel mode (set_mode passes mode==5 to the sentinel
    // datum's initialize); its name is not attested.
    enum e_mode {
        _mode_none = 0,
        _mode_ground = 1,
        _mode_flying = 2,
        _mode_dead = 3,
        _mode_sentinel = 4,
        _mode_sentinel_b = 5,
        _mode_melee = 6,

        _mode_first = _mode_ground,
        _mode_last = _mode_melee
    };

    void set_mode(e_mode new_mode);
    void update(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input);
    void move(s_character_physics_move_output_datum *output, const s_character_physics_move_input_datum *input);
    void snap_facing(real_vector3d *forward, real_vector3d *up, const s_character_physics_update_input_datum *input);

    // March: physics\character_physics.cpp:53 (ASSERT m_object_index!=NONE after the stores)
    __forceinline void initialize(long object_index) {
        m_object_index = object_index;
        m_mode = _mode_none;
        m_flags = 0;
    }

    // March: physics\character_physics.cpp:137
    __forceinline e_mode get_mode() const { return (e_mode)m_mode; }

    __forceinline bool is_sentinel_mode() const {
        e_mode mode = get_mode();
        return mode == _mode_sentinel || mode == _mode_sentinel_b;
    }

    // March: lbz 3(this); != 0
    __forceinline bool is_immune_to_collision_damage() const { return m_collision_damage_immunity_counter != 0; }

    __forceinline c_character_physics_mode_ground_datum *get_mode_ground()
    {
        return reinterpret_cast<c_character_physics_mode_ground_datum *>(m_storage);
    }
    __forceinline c_character_physics_mode_flying_datum *get_mode_flying() { return &m_flying; }
    __forceinline c_character_physics_mode_dead_datum *get_mode_dead() { return &m_dead; }
    __forceinline c_character_physics_mode_sentinel_datum *get_mode_sentinel()
    {
        return reinterpret_cast<c_character_physics_mode_sentinel_datum *>(m_storage);
    }
    __forceinline c_character_physics_mode_melee_datum *get_mode_melee() { return &m_melee; }

private:
    void update_animation(s_character_physics_update_output_datum *output,
        const s_character_physics_update_input_datum *input);
    // March: physics\character_physics.cpp:431 (counter = game_seconds_to_ticks_round(k_collision_damage_immunity_duration))
    void ping_collision_damage_immunity_counter();

public:
    short m_flags;                                  // plain integer: March initialize stores it with sth, no c_flags call
    unsigned char m_mode;                           // e_mode
    unsigned char m_collision_damage_immunity_counter; // ticks; March asserts the start value fits UNSIGNED_CHAR_MAX
    long m_object_index;
    union
    {
        c_character_physics_mode_flying_datum m_flying;
        c_character_physics_mode_dead_datum m_dead;
        c_character_physics_mode_melee_datum m_melee;
        // Ground and sentinel need raw storage: their datums have non-trivial constructors.
        char m_storage[0x7c - 0x08];
    };
};

typedef char char_phys_comp_size[(sizeof(c_character_physics_component) == 0x7c) ? 1 : -1];
typedef char char_phys_comp_mode_offset[(offsetof(c_character_physics_component, m_flying) == 0x08) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_COMPONENT_H
