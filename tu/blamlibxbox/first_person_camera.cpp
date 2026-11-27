// blamlibxbox/first_person_camera.obj: from the records 0x9a590 0x12af98 0x12b9d0 0x2026e8
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
#include "game/player_control_types.h"
#include "memory/datum_index.h"
#include "units/unit_seat.h"
#include "math/matrix_math_types.h"
#include "objects/object_placement.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "cseries/cseries.h"
#include "game/players.h"
#include "objects/objects.h"
#include "units/units.h"
#include "camera/first_person_camera.h"
#include "game/player_control_camera.h"
#include "camera/observer.h"
#include "units/bipeds.h"
#include "interface/chud/chud.h"
#include "interface/user_interface_controller.h"
#include "math/matrix_math.h"
#include "units/unit_types.h"
#include "units/unit_definitions.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "math/real_math_globals.h"
#include "camera/camera_types.h"
#include "camera/camera.h"
#include "camera/camera_globals.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "scenario/scenario.h"
#include "shell/shell_types.h"
#include "tag_files/tag_types.h"
#include "camera/first_person_camera_types.h"
#include "game/player_mapping_types.h"
#include "game/game_globals.h"
#include "game/player_control_constants.h"
#include "game/player_interaction.h"
#include "items/weapon_definitions.h"
// @address 0x9a590 ?first_person_camera_deterministic@@YAXJPATreal_point3d@@PATreal_vector3d@@@Z
// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// @address 0x761454 ?g_observer_default_field_of_view@@3MA

// 0x9a590 natural source
void first_person_camera_deterministic(long unit_index, real_point3d *position, real_vector3d *forward)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    unit_get_camera_position(unit_index, position);
    *forward = unit->unit.aiming_vector;
    if (unit->object.parent_object_index != -1)
    {
        s_object_header *parent_header = object_header_get(unit->object.parent_object_index);
        if (((1 << parent_header->object_type) & _object_mask_unit) != 0)
        {
            unit_datum *parent = (unit_datum *)parent_header->datum;
            unit_definition *definition = TAG_GET('unit', unit_definition, parent->object.definition_index);
            unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
            object_marker marker;
            if ((unsigned char)(((unsigned long)seat->flags >> 7) & 1) != 0 &&
                internal_object_get_markers_by_string_id(unit->object.parent_object_index, 202, &marker, 1, false) != 0)
            {
                *position = marker.matrix.position;
                *forward = marker.matrix.forward;
            }
        }
    }
}

extern "C" {
// @address 0x587d4 k_first_person_zoom_distance_min
const float k_first_person_zoom_distance_min = 0.03141593f;
// @address 0x587d0 k_first_person_zoom_distance_max
const float k_first_person_zoom_distance_max = 3.1101768f;
}

// @address 0x12b9d0 ?first_person_camera_zoom_scaled_distance@@YAMJMF@Z
float first_person_camera_zoom_scaled_distance(long unit_index, float distance, short zoom_level)
{
    float zoom = ::unit_get_zoom_magnification(unit_index, zoom_level);
    if (zoom != 1.0f)
    {
        float scaled = distance / zoom;
        if (scaled > k_first_person_zoom_distance_min && scaled < k_first_person_zoom_distance_max)
            return scaled;
    }
    return distance;
}

// @address 0x2026e8 ?first_person_camera_for_unit_and_vector@@YAXJPBTreal_vector3d@@PAUs_observer_command@@@Z
void first_person_camera_for_unit_and_vector(long unit_index, const real_vector3d *forward, s_observer_command *result)
{
    result->timer = 0.0f;
    result->flags = 0;
    result->focus_position = *global_origin3d;
    result->focus_distance = 0.0f;
    result->forward = *forward;
    result->field_of_view = g_observer_default_field_of_view;
    result->position = *global_origin3d;
    result->velocity = *global_zero_vector3d;
    generate_up_vector3d(&result->forward, &result->up);
    if (unit_index != -1)
    {
        unit_datum *unit = (unit_datum *)object_get(unit_index);
        unit_get_camera_position(unit_index, &result->position);
        object_get_velocities(unit_index, &result->velocity, 0);
        long parent_index = unit->object.parent_object_index;
        if (parent_index != -1)
        {
            s_object_header *parent_header = object_header_get(parent_index);
            if (((1 << parent_header->object_type) & _object_mask_unit) != 0)
            {
                unit_datum *parent = (unit_datum *)parent_header->datum;
                unit_definition *definition = TAG_GET('unit', unit_definition, parent->object.definition_index);
                unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
                if ((unsigned char)(((unsigned long)seat->flags >> 7) & 1) != 0)
                {
                    object_marker marker;
                    if (internal_object_get_markers_by_string_id(parent_index, 202, &marker, 1, false) != 0)
                    {
                        result->position = marker.matrix.position;
                        result->forward = marker.matrix.forward;
                        result->up = marker.matrix.up;
                    }
                }
                else
                {
                    float inverse_i = 0.0f, inverse_j = 0.0f, inverse_k = 0.0f;
                    float forward_i = 0.0f, forward_j = 0.0f, forward_k = 0.0f;
                    float up_i = 0.0f, up_j = 0.0f, up_k = 0.0f;
                    real_matrix4x3 matrix;
                    real_vector3d *camera_forward = &result->forward;
                    real_vector3d *camera_up = &result->up;
                    matrix4x3_rotation_from_vectors(&matrix, &parent->object.forward, &parent->object.up);
                    inverse_i = camera_forward->i;
                    inverse_j = camera_forward->j;
                    inverse_k = camera_forward->k;
                    camera_forward->i = matrix.forward.i * inverse_i + (matrix.forward.k * inverse_k + matrix.forward.j * inverse_j);
                    camera_forward->j = matrix.left.i * inverse_i + (matrix.left.k * inverse_k + matrix.left.j * inverse_j);
                    camera_forward->k = matrix.up.i * inverse_i + (matrix.up.k * inverse_k + matrix.up.j * inverse_j);
                    generate_up_vector3d(camera_forward, camera_up);
                    forward_i = camera_forward->i;
                    forward_j = camera_forward->j;
                    forward_k = camera_forward->k;
                    camera_forward->i = matrix.forward.i * forward_i + (matrix.up.i * forward_k + matrix.left.i * forward_j);
                    camera_forward->j = matrix.forward.j * forward_i + (matrix.up.j * forward_k + matrix.left.j * forward_j);
                    camera_forward->k = matrix.forward.k * forward_i + (matrix.up.k * forward_k + matrix.left.k * forward_j);
                    up_i = camera_up->i;
                    up_j = camera_up->j;
                    up_k = camera_up->k;
                    camera_up->i = matrix.forward.i * up_i + (matrix.up.i * up_k + matrix.left.i * up_j);
                    camera_up->j = matrix.forward.j * up_i + (matrix.up.j * up_k + matrix.left.j * up_j);
                    camera_up->k = matrix.forward.k * up_i + (matrix.up.k * up_k + matrix.left.k * up_j);
                }
            }
        }
        else if (((1 << unit->object.object_type) & 1) != 0)
        {
            real_point3d camera_position = result->position;
            biped_compute_lean(unit_index, &camera_position, &result->forward, &result->up);
        }
        result->flags = 1;
    }
    observer_validate_camera_command(result);
}

extern "C" {
// @address 0x76144c g_first_person_camera_fov_min
float g_first_person_camera_fov_min = 0.017453292f;
// @address 0x761450 g_first_person_camera_fov_max
float g_first_person_camera_fov_max = 2.6179938f;
}
static __forceinline long first_person_camera_player_get_unit(long user_index)
{
    long result = -1;
    if (user_index != -1)
        result = tls_player_mapping_globals()->unit_indices[user_index];
    return result;
}
static __forceinline long first_person_camera_player_index(long user_index)
{
    long result = -1;
    if (user_index != -1)
        result = tls_player_mapping_globals()->player_indices[user_index];
    return result;
}
static __forceinline void first_person_camera_input_state_get(long controller_index, const s_game_input_state **state)
{
    *state = &input_abstraction_globals.input_states[controller_index];
}
static __forceinline void first_person_camera_look_velocity_get(long controller_index, real_euler_angles2d *velocity)
{
    velocity->yaw = input_abstraction_globals.look_settings[controller_index].preferences.horizontal_look_sensitivity * 0.017453292f;
    velocity->pitch = input_abstraction_globals.look_settings[controller_index].preferences.vertical_look_sensitivity * 0.017453292f;
}
static __forceinline long first_person_camera_root_object(long object_index)
{
    long result = -1;
    while (object_index != -1) {
        result = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    return result;
}
static __forceinline unit_datum *first_person_camera_unit_get_absolute(long object_index)
{
    s_object_header *headers = (s_object_header *)tls_object_header_data()->data;
    return (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
}
// @address 0x12af98 ?update@c_first_person_camera@@UAAXJMPAUs_observer_command@@@Z
void c_first_person_camera::update(long user_index, float elapsed_time, s_observer_command *command)
{
    long target = first_person_camera_player_get_unit(user_index);
    if (m_target != target)
        set_target(target);
    if (m_orientation_enabled) {
        long controller_index = user_interface_controller_get_rumble_enabled(user_index);
        if (controller_index != -1 && !camera_responding_gate((e_controller_index)controller_index)) {
            const s_game_input_state *input;
            real_euler_angles2d angular_velocity;
            first_person_camera_input_state_get(controller_index, &input);
            first_person_camera_look_velocity_get(controller_index, &angular_velocity);
            m_facing.yaw += input->look.i * angular_velocity.yaw * elapsed_time;
            m_facing.pitch += input->look.j * angular_velocity.pitch * elapsed_time;
            m_facing.pitch = PIN(m_facing.pitch, -1.4922565f, 1.4922565f);
        }
    }
    if (m_target != -1) {
        real_vector3d forward;
        if (m_orientation_enabled)
            vector3d_from_euler_angles2d(&forward, &m_facing);
        else
            player_build_aiming_vector_from_facing(first_person_camera_player_index(user_index),
                &player_control_globals->states[user_index].action.look, &forward);
        first_person_camera_for_unit_and_vector(m_target, &forward, command);
        real_point2d crosshair = global_game_globals->player_control[0].crosshair_location;
        set_real_vector2d(&command->focus_offset, 1.0f, crosshair.y);
        long object_index = m_target;
        unit_datum *unit = first_person_camera_unit_get_absolute(object_index);
        short zoom_level = unit->unit.zoom_level;
        unit_definition *definition = TAG_GET('unit', unit_definition, unit->object.definition_index);
        command->field_of_view = first_person_camera_zoom_scaled_distance(
            object_index, definition->camera_field_of_view, zoom_level) * g_camera_globals.field_of_view_scale;
        command->field_of_view *= chud_get_cinematic_fov_scale();
        command->field_of_view = PIN(command->field_of_view, g_first_person_camera_fov_min, g_first_person_camera_fov_max);
        if (m_field_of_view != command->field_of_view) {
            unit = first_person_camera_unit_get_absolute(object_index);
            long weapon_index = -1;
            signed char active_weapon_index = unit->unit.current_weapon_index;
            if (active_weapon_index != -1)
                weapon_index = unit->unit.weapon_object_indices[active_weapon_index];
            float change_time = 0.18f;
            if (weapon_index != -1) {
                object_datum *weapon = object_get(weapon_index);
                weapon_definition *weapon_def =
                    TAG_GET('weap', weapon_definition, weapon->definition_index);
                if (weapon_def->field_of_view_change_time > 0.0f)
                    change_time = weapon_def->field_of_view_change_time;
            }
            command->parameter_timers[4] = change_time;
            command->parameter_flags[4] = true;
            m_field_of_view = command->field_of_view;
        }
        if (((1 << object_header_get(object_index)->object_type) & 1) != 0) {
            command->flags |= 0x2000;
            biped_get_physics_pill_from_definition(object_index, &command->pill_base,
                &command->pill_height, &command->pill_width);
        }
        object_get_center_of_mass(first_person_camera_root_object(m_target), &command->parent_position);
        command->flags |= 0xc01;
    }
    observer_validate_camera_command(command);
}
