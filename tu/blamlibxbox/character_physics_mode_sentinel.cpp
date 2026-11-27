// OBJ: from the records 0x2df868 0x2df9e0 0x2dfb90
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "objects/object_reference_types.h"
#include "memory/data.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "objects/object_datum.h"
#include "physics/character_physics_output.h"
#include <math.h>
#include "game/game_time_types.h"
#include "objects/objects.h"
#include "physics/havok_component.h"

#include "animations/animation_id.h"
#include "game/game_time_inline.h"
#include "physics/character_physics_input.h"
#include "physics/character_physics_mode_sentinel.h"

#include "memory/tls_globals.h"

struct s_render_list;

#include "physics/havok_component_class.h"

// @address 0x2df868 ?dispose@c_character_physics_mode_sentinel_datum@@QAAXJ@Z
void c_character_physics_mode_sentinel_datum::dispose(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);

    if (object->havok_component_index != -1)
    {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
        real_matrix4x3 matrix;
        component->calculate_object_transform(&matrix);

        if (((object_datum *)object_get_and_verify_type(object_index, _object_mask_all))->parent_object_index == -1)
        {
            object_set_position_direct(object_index, &matrix.position, 0, 0, 0, false);
        }
    }

    if (object->havok_component_index != -1)
    {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
        float magnitude = ::normalize3d(&m_old_velocity);

        if (magnitude > 1.0f)
        {
            real_vector3d velocity;
            component->get_rigid_body_linear_velocity( 0, &velocity);
            float dot = dot_product3d(&m_old_velocity, &velocity);

            if (dot > 1.0f)
            {
                float maximum = magnitude - 1.0f;
                if (dot > maximum)
                {
                    dot = maximum;
                }

                real_vector3d correction;
                scale_vector3d(&m_old_velocity, dot, &correction);
                subtract_vectors3d(&velocity, &correction, &velocity);
                component->set_rigid_body_linear_velocity(0, &velocity);
            }
        }
    }
}

// @address 0x2df9e0 ?update@c_character_physics_mode_sentinel_datum@@QAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_sentinel_datum::update(
    s_character_physics_update_output_datum *physics_output, const s_character_physics_update_input_datum *physics_input)
{
    update_origin(physics_output, physics_input);

    real_point3d future_position;
    point_from_line3d(&physics_input->position, &physics_input->velocity, game_tick_length(), &future_position);

    real_vector3d displacement;
    vector_from_points3d(&physics_input->previous_position, &future_position, &displacement);

    real_vector3d velocity;
    scale_vector3d(&displacement, (float)game_tick_rate(), &velocity);

    real_vector3d acceleration;
    subtract_vectors3d(&velocity, &physics_input->velocity, &acceleration);

    float acceleration_squared = magnitude_squared3d(&acceleration);
    float maximum_instantaneous_acceleration = 0.5f * game_tick_length();
    if (acceleration_squared < maximum_instantaneous_acceleration * maximum_instantaneous_acceleration || m_old_animation != physics_input->animation)
    {
        physics_output->velocity = velocity;
    }
    else
    {
        real_vector3d limited_acceleration;
        scale_vector3d(&acceleration, maximum_instantaneous_acceleration / (sqrtf(acceleration_squared) * game_tick_length()), &limited_acceleration);
        add_vectors3d(&physics_input->velocity, &limited_acceleration, &physics_output->velocity);
    }

    m_old_velocity = physics_input->velocity;
    m_old_animation = physics_input->animation;
}

// @address 0x2dfb90 ?update_origin@c_character_physics_mode_sentinel_datum@@AAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_sentinel_datum::update_origin(
    s_character_physics_update_output_datum *physics_output, const s_character_physics_update_input_datum *physics_input)
{
    if (!(m_old_animation != physics_input->animation))
    {
        if (distance_squared3d(&physics_input->position, &physics_input->previous_position) > 0.010000001f)
        {
            physics_output->flags |= FLAG(2);
        }
    }

    if (m_origin_pending)
    {
        point_from_line3d(&physics_input->origin, &m_origin_offset, 1.0f, &physics_output->origin);
        physics_output->origin_valid = true;
        m_origin_pending = false;
    }
}
