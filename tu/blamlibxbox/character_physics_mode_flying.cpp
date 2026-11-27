// blamlibxbox/character_physics_mode_flying.obj: from the records 0xe2808 0x300158
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/matrix_math.h"
#include "memory/tls_globals.h"
#include "physics/character_physics_output.h"
#include "math/real_math_trigonometry.h"
#include "game/game_time_inline.h"
#include "game/game_time_types.h"
#include "units/units.h"
#include "physics/character_physics_input.h"

extern "C" {
}


#include "physics/character_physics_definition_types.h"



#include "physics/character_physics_mode_flying.h"

// @address 0xe2808 ?update@c_character_physics_mode_flying_datum@@QAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_flying_datum::update(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
{
    real_vector3d left, up;
    {
        real_vector3d forward;
        real_vector3d_build_axes_from_vectors(&input->forward, &global_identity4x3->up, &forward, &left, &up);
    }

    real_vector3d throttle;
    scale_vector3d(&input->flying.throttle, input->speed, &throttle);
    if (input->definition->flying.flags & 1)
        up = global_identity4x3->up;

    real_vector3d desired;
    scale_vector3d(&input->forward, throttle.i, &desired);
    point_from_line3d(&desired, &left, throttle.j, &desired);
    point_from_line3d(&desired, &up, throttle.k, &desired);

    real_vector3d delta;
    subtract_vectors3d(&desired, &input->external_velocity, &delta);

    real_vector3d direction = delta;
    float max_delta = input->flying.acceleration * game_tick_length();
    float distance = normalize3d(&direction);

    real_vector3d change;
    if (distance > max_delta)
        scale_vector3d(&direction, max_delta, &change);
    else
        change = delta;

    add_vectors3d(&input->external_velocity, &change, &output->velocity);
    m_turning_disabled_counter = (m_turning_disabled_counter - 1) > 0 ? m_turning_disabled_counter - 1 : 0;
}

// @address 0x300158 ?snap_facing@c_character_physics_mode_flying_datum@@QAAXPATreal_vector3d@@0PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_flying_datum::snap_facing(real_vector3d *forward, real_vector3d *up, const s_character_physics_update_input_datum *input)
{
    if (is_turning_disabled())
    {
        *up = input->up;
        *forward = input->forward;
        {
            real_rectangle2d rect;
            rect.x0 = -3.1415927f;
            rect.x1 = 3.1415927f;
            rect.y0 = -3.1415927f;
            rect.y1 = 3.1415927f;
            unit_euler_aiming_update(0, 0.0f, up, global_up3d, game_tick_length(), &m_axis, &rect,
                input->definition->flying.angular_velocity_maximum, input->definition->flying.angular_acceleration_maximum, 0);
        }
        {
            real_vector3d axis;
            real_vector3d temp;
            axis = m_axis;
            float angle = normalize3d(&axis) * game_tick_length();
            float cosine = ::cosine(angle);
            float sine = ::sine(angle);
            rotate_vector_about_axis(up, &axis, sine, cosine);
            float dot = up->j * forward->j + up->i * forward->i + up->k * forward->k;
            scale_vector3d(up, dot, &temp);
            subtract_vectors3d(forward, &temp, forward);
            if (normalize3d(forward) == 0.0f)
            {
                *up = *global_up3d;
                *forward = *global_forward3d;
            }
            if (angle == 0.0f)
                m_turning_disabled_counter = 0;
        }
    }
    else
    {
        real_vector3d axis;
        real_vector3d rotated;
        real_vector3d temp;
        cross_product3d(forward, global_up3d, &axis);
        cross_product3d(&axis, forward, &rotated);
        if (normalize3d(&rotated) == 0.0f)
        {
            rotated = *global_forward3d;
            axis = *global_left3d;
        }
        float cosine = ::cosine(input->flying_friction);
        float sine = ::sine(input->flying_friction);
        scale_vector3d(&rotated, cosine, &rotated);
        normalize3d(&axis);
        point_from_line3d(&rotated, &axis, sine, up);
        normalize3d(forward);
        float dot = up->j * forward->j + up->i * forward->i + up->k * forward->k;
        scale_vector3d(forward, dot, &temp);
        subtract_vectors3d(up, &temp, up);
        normalize3d(up);
    }
}
