#ifndef PHYSICS_CHARACTER_PHYSICS_MODE_SENTINEL_H
#define PHYSICS_CHARACTER_PHYSICS_MODE_SENTINEL_H

#include "animations/animation_id.h"
#include "physics/character_physics_move_input.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_output.h"

struct s_character_physics_move_output_datum;

// March physics/character_physics_mode_sentinel.cpp owns all ten methods.
class c_character_physics_mode_sentinel_datum
{
public:
    // March 82c3c788: no locals; clear the accumulated translation before position.
    __forceinline void initialize(bool climbing, const real_point3d *position)
    {
        extern const real_vector3d *global_zero_vector3d;
        m_origin_offset = *global_zero_vector3d;
        m_position = *position;
        m_origin_pending = false;
        m_old_velocity = *global_zero_vector3d;
        m_climbing = climbing;
        m_old_animation.clear();
    }
    void dispose(long object_index);
    void update(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input);
    __forceinline void move(s_character_physics_move_output_datum *output, const s_character_physics_move_input_datum *input)
    {
        m_position = input->sentinel_physics_position;
    }
    __forceinline void update_animation(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
    {
        if (is_climbing())
        {
            if (input->movement.k < 0.0f)
                output->animation_state = (s_character_physics_update_output_datum::e_animation_state)11;
            else if (input->movement.k > 0.0f)
                output->animation_state = (s_character_physics_update_output_datum::e_animation_state)10;
            else
                output->animation_state = (s_character_physics_update_output_datum::e_animation_state)1;
        }
    }
    // March 82c3d0c0.
    __forceinline void snap_facing(real_vector3d *forward, real_vector3d *up, const s_character_physics_update_input_datum *input) {}
    // March 82c3d0f8.
    __forceinline void set_accumulated_sentinel_physics_translation(const real_vector3d *translation)
    {
        m_origin_pending = true;
        m_origin_offset = *translation;
    }
    // March 82c3d1d8.
    __forceinline const real_point3d *get_sentinel_physics_position() const { return &m_position; }

    // March 82c3ce58 reads +0x28; +0x29 instead requests an origin update.
    __forceinline bool is_climbing() const { return m_climbing; }

private:
    void update_origin(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input);

    real_point3d m_position;
    real_vector3d m_origin_offset;
    real_vector3d m_old_velocity;
    c_animation_id m_old_animation;
    bool m_climbing;
    bool m_origin_pending;
};

typedef char character_physics_sentinel_size[(sizeof(c_character_physics_mode_sentinel_datum) == 0x2c) ? 1 : -1];

#endif // PHYSICS_CHARACTER_PHYSICS_MODE_SENTINEL_H
