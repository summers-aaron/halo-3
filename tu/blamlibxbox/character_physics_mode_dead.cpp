// blamlibxbox/character_physics_mode_dead.obj: from the records 0x2dea38 0x2df170 0x2df388
// @flags /TP /O2 /Oc
#include <stddef.h>
#include "physics/havok_group_types.h"
#include "cseries/cseries.h"
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "phys/phys_base.h"
#include "physics/character_physics_output.h"
#include "phys/phys_collide.h"
#include "physics/havok.h"
#include "render/render_objects_static_lighting.h"
#include "physics/character_physics_input.h"
#include "physics/havok_contact_points.h"
#include "physics/havok_component.h"
#include "physics/character_physics_shape_types.h"
#include "physics/character_physics_definition_types.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "physics/havok_constants_types.h"
#include "physics/character_physics_ground_helpers.h"
#include "physics/physics_constants_globals.h"
#include "physics/havok_contact_point.h"
#include "physics/havok_component_class.h"
#include "physics/havok_contact_point_iterator.h"
#include "physics/character_physics_move_input.h"
#include "physics/character_physics_move_output.h"
#include "physics/character_physics_mode_dead.h"
#include "physics/havok_math.h"
#include "game/game_time_inline.h"

struct s_body_world;

// match: 0x2dea38, 0x2df388: old inline accessor; the const-short owner at 0x1ba838 is held by AN2.
static __forceinline short character_physics_mode_dead_controlling_body(const c_havok_component *component)
{
    long index = component->m_object_postion_controlling_rigid_body_index;
    return (short)(VALID_INDEX(index, component->get_rigid_body_count()) ? index : NONE);
}


const float k_dead_resting_seconds = 0.2f;

static __forceinline unsigned char character_physics_mode_dead_increment_ticks(unsigned char ticks)
{
    return (unsigned char)MIN(ticks + 1, 254);
}

// @address 0x2dea38 ?update@c_character_physics_mode_dead_datum@@QAAXPAUs_character_physics_update_output_datum@@PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_dead_datum::update(s_character_physics_update_output_datum *output, const s_character_physics_update_input_datum *input)
{
    bool apply_support_velocity = false;
    real_vector3d support_velocity_delta;
    c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, input->havok_component_index);
    output->velocity = input->external_velocity;
    if (input->accepted_physics != NONE) {
        object_datum *support_object = (object_datum *)object_get_and_verify_type(input->accepted_physics, 0xffffffff);
        if (support_object->havok_component_index != NONE) {
            c_havok_component *support_component = datum_get<c_havok_component>(g_havok_component_data, support_object->havok_component_index);
            if (character_physics_mode_dead_controlling_body(support_component) != NONE) {
                real_vector3d support_velocity;
                support_component->get_rigid_body_linear_velocity_at_point(
                    character_physics_mode_dead_controlling_body(support_component), &support_velocity, &input->current_position);
                subtract_vectors3d(&support_velocity, &m_support_velocity, &support_velocity_delta);
                apply_support_velocity = magnitude_squared3d(&m_support_velocity) > 0.0f;
                m_support_velocity = support_velocity;
            }
        }
    } else {
        m_support_velocity = *global_zero_vector3d;
    }

    if (m_ground_ticks && (output->velocity.k < 0.01f || input->external_velocity.k < 0.01f)) {
        float speed = square_root(output->velocity.i * output->velocity.i + output->velocity.j * output->velocity.j);
        float deceleration = 2.0f * game_tick_length();
        if (speed > deceleration) {
            float scale = 1.0f - deceleration / speed;
            output->velocity.i *= scale;
            output->velocity.j *= scale;
        } else {
            output->velocity.i = 0.0f;
            output->velocity.j = 0.0f;
        }
    }

    if (magnitude_squared3d(&input->acceleration) > 0.000000009999999f) {
        if (TEST_BIT(output->flags, 0))
            output->flags |= FLAG(0);
        else
            output->flags &= ~FLAG(0);
        real_point2d acceleration;
        acceleration.x = input->acceleration.i * input->forward.i - input->acceleration.j * input->forward.j;
        acceleration.y = input->acceleration.i * input->forward.j + input->acceleration.j * input->forward.i;
        real_vector2d delta;
        delta.i = acceleration.x - input->external_velocity.i;
        delta.j = acceleration.y - input->external_velocity.j;
        float magnitude = magnitude2d(&delta);
        float maximum;
        if (TEST_BIT(input->flags, 2))
            maximum = input->maximum_slope;
        else
            maximum = input->maximum_velocity;
        float limit = game_tick_length() * maximum;
        if (magnitude > limit)
            scale_vector2d(&delta, limit / magnitude, &delta);
        if (!TEST_BIT(input->flags, 2)) {
            output->velocity.i += delta.i;
            output->velocity.j += delta.j;
        }
    }

    const float k_slow_speed = 1.5f;
    if (TEST_BIT(input->flags, 1) || magnitude_squared3d(&output->velocity) < k_slow_speed * k_slow_speed) {
        m_slow_ticks = character_physics_mode_dead_increment_ticks(m_slow_ticks);
        m_fast_ticks = 0;
    } else {
        m_slow_ticks = 0;
        m_fast_ticks = character_physics_mode_dead_increment_ticks(m_fast_ticks);
    }

    if (input->dead_size_reset) {
        m_dead_size = 0;
    } else if (input->dead_size_increase && get_dead_size() < 2) {
        float radius_offset = g_havok_constants.minimum_radius + 0.001f;
        long new_dead_size = NONE;
        phys_transform transform;
        transform.get_rotation().set_to_identity();
        for (long dead_size = m_dead_size + 1; dead_size < 3; dead_size++) {
            const s_character_physics_dead_sphere *sphere = TAG_BLOCK_GET_ELEMENT(
                &input->definition->dead_spheres, dead_size, const s_character_physics_dead_sphere);
            phys_sphere_shape shape(MAX(sphere->shape.get_convex_radius() - radius_offset, g_havok_constants.minimum_radius));
            phys_vec4_from_real_point3d(&transform.get_translation(), &input->current_position);
            transform.get_translation().add4(sphere->transform.get_translation());
            real_matrix4x3 matrix;
            matrix4x3_from_hkTransform(&matrix, &transform);
            if (!havok_world_shape_collides(&shape, input->havok_component_index, &matrix, input->havok_group))
                new_dead_size = dead_size;
        }
        if (new_dead_size != NONE) {
            m_dead_size = (unsigned char)new_dead_size;
            havok_object_rebuild(component->get_object_index(), 0);
        }
    }

    c_havok_contact_point_iterator contact_points(component->get_object_index(), false);
    if (contact_points.valid()) {
        long rigid_body_index = character_physics_mode_dead_controlling_body(component);
        bool touching_other = false;
        c_havok_contact_point_iterator iterator(component->get_object_index(), false);
        while (iterator.valid()) {
            c_havok_contact_point *contact_point = iterator.get_contact_point();
            if (contact_point->get_contact_havok_component_index() != NONE && contact_point->get_contact_point_object_index() != input->accepted_physics) {
                touching_other = true;
                break;
            }
            iterator.next();
        }
        if (!touching_other && rigid_body_index != NONE) {
            const float k_resting_speed = 0.05f;
            if (-k_resting_speed <= output->velocity.i && output->velocity.i <= k_resting_speed &&
                -k_resting_speed <= output->velocity.j && output->velocity.j <= k_resting_speed &&
                output->velocity.k >= -global_gravity_get() - k_resting_speed && output->velocity.k <= k_resting_speed) {
                m_resting_ticks = character_physics_mode_dead_increment_ticks(m_resting_ticks);
            }
        } else {
            m_resting_ticks = 0;
        }
    } else {
        m_resting_ticks = 0;
    }

    if (apply_support_velocity)
        add_vectors3d(&output->velocity, &support_velocity_delta, &output->velocity);
}

// @address 0x2df170 ?snap_facing@c_character_physics_mode_dead_datum@@QAAXPATreal_vector3d@@0PBUs_character_physics_update_input_datum@@@Z
void c_character_physics_mode_dead_datum::snap_facing(real_vector3d *forward, real_vector3d *up, const s_character_physics_update_input_datum *input)
{
    if ((input->flags & (1 << 9)) || input->force_facing || m_contact_ticks <= 2 || m_ground_ticks <= 2) {
        *forward = input->forward;
        *up = input->up;
    } else {
        float dot = dot_product3d(&input->up, &input->desired_up);
        dot = PIN(dot, -1.0f, 1.0f);
        if (fabs(dot - 1.0f) >= 0.0001f) {
            float angle = ::arccosine(dot);
            if (angle != 0.0f) {
                real_vector3d axis;
                cross_product3d(&input->up, &input->desired_up, &axis);
                if (normalize3d(&axis) != 0.0f) {
                    *up = input->up;
                    *forward = input->forward;
                    rotate_vector_about_axis(up, &axis, (float)sin(angle), (float)cos(angle));
                    rotate_vector_about_axis(forward, &axis, (float)sin(angle), (float)cos(angle));
                    normalize3d(up);
                    normalize3d(forward);
                }
            }
        }
    }
}

// @address 0x2df388 ?move@c_character_physics_mode_dead_datum@@QAAXPAUs_character_physics_move_output_datum@@PBUs_character_physics_move_input_datum@@@Z
void c_character_physics_mode_dead_datum::move(s_character_physics_move_output_datum *output, const s_character_physics_move_input_datum *input)
{
    s_havok_contact_point_reference best = k_invalid_contact_point_reference;
    bool has_contact = false;
    m_contact_on_relaxable_surface = false;
    unsigned long flags = input->flags;
    if (!TEST_BIT(flags, 9)) {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, input->havok_component_index);
        float best_height = -3.402823466e+38F;
        c_havok_contact_point_iterator iterator(component->get_object_index(), false);
        while (iterator.valid()) {
            c_havok_contact_point *contact_point = iterator.get_contact_point();
            const real_vector3d *normal = contact_point->get_normal();
            if (normal->k > best_height) {
                best = *iterator.get_reference();
                best_height = normal->k;
            }
            iterator.next();
        }
        if (best.rigid_body_index != NONE) {
            c_havok_contact_point *contact_point = component->get_contact_point(best.rigid_body_index, best.contact_point_index);
            if (input->local_physics != NONE && contact_point->get_contact_point_object_index() == input->local_physics)
                output->object_index = input->local_physics;
            if (TEST_BIT(flags, 1))
                m_ground_plane = *contact_point->get_normal();
            else
                add_vectors3d(&m_ground_plane, contact_point->get_normal(), &m_ground_plane);
            normalize3d_with_default(&m_ground_plane, global_up3d);
            has_contact = true;
            if (contact_point->get_contact_point_object_index() == NONE || contact_point->get_contact_point_object_index() == input->accepted_physics)
                m_contact_on_relaxable_surface = true;
            else
                m_contact_on_relaxable_surface = false;
        } else {
            has_contact = false;
            m_ground_plane = *global_up3d;
        }
        if (!havok_component_has_active_entity(component))
            has_contact = true;
    }
    if (has_contact) {
        if (TEST_BIT(input->flags, 0))
            m_ground_ticks = MIN(m_ground_ticks + 1, 254);
        else
            m_ground_ticks = (dot_product3d(global_up3d, &m_ground_plane) > 0.5f) ? MIN(m_ground_ticks + 1, 254) : 0;
        m_contact_ticks = MIN(m_contact_ticks + 1, 254);
    } else {
        m_ground_ticks = 0;
        m_contact_ticks = 0;
    }
    if (m_ground_ticks == 0) {
        if (m_resting_ticks <= game_seconds_to_ticks_round(k_dead_resting_seconds)) {
            output->flags |= FLAG(0);
            return;
        }
    }
    output->flags &= ~FLAG(0);
}

#if !defined(_WIN64) && !defined(__LP64__)
typedef char contact_iterator_object_offset[(offsetof(c_havok_contact_point_iterator, object_index) == 4) ? 1 : -1];
typedef char contact_iterator_size[(sizeof(c_havok_contact_point_iterator) == 12) ? 1 : -1];
#endif
