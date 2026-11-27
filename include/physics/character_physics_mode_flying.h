#ifndef PHYSICS_CHARACTER_PHYSICS_MODE_FLYING_H
#define PHYSICS_CHARACTER_PHYSICS_MODE_FLYING_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_character_physics_move_output_datum;
struct s_character_physics_move_input_datum;
struct s_character_physics_update_output_datum;
struct s_character_physics_update_input_datum;

// March physics/character_physics_mode_flying.cpp:213 names the counter.
// initialize (82c4ef10) clears the counter and the vector at +4.
class c_character_physics_mode_flying_datum
{
public:
    // March 82c4ef10, no named locals.
    __forceinline void initialize()
    {
        extern const real_vector3d *global_zero_vector3d;
        m_turning_disabled_counter = 0;
        m_axis = *global_zero_vector3d;
    }
    // March 82c4ef70.
    __forceinline void dispose() {}
    void update(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input);
    void update_animation(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input);
    void snap_facing(real_vector3d *forward, real_vector3d *up, const s_character_physics_update_input_datum *input);
    // March 82c4fa20.
    __forceinline void move(s_character_physics_move_output_datum *output, const s_character_physics_move_input_datum *input) {}
    void disable_turning();
    // March 82c4fb40: the signed counter is strictly positive.
    bool is_turning_disabled() const { return m_turning_disabled_counter > 0; }

    long m_turning_disabled_counter;
    real_vector3d m_axis;
};

typedef char character_physics_flying_size[(sizeof(c_character_physics_mode_flying_datum) == 0x10) ? 1 : -1];
typedef char character_physics_flying_axis_offset[(offsetof(c_character_physics_mode_flying_datum, m_axis) == 4) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_MODE_FLYING_H
