// blamlibxbox/orbiting_camera.obj: from the records 0x296408 0x296410 0x296418 0x296420 0x2964a8 0x296900
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include <math.h>
#include "cseries/cseries.h"
#include "shell/shell.h"
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "objects/object_header.h"
#include "objects/object_type_access.h"
#include "objects/objects.h"
#include "units/unit_types.h"
#include "camera/observer.h"
#include "camera/camera_globals.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "interface/user_interface_controller.h"
#include "camera/orbiting_camera.h"
#include "camera/camera.h"
#include "memory/datum_index.h"

// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// @address 0x761454 ?g_observer_default_field_of_view@@3MA

// @address 0x296408 ?set_forward@c_orbiting_camera@@UAAXPBTreal_vector3d@@@Z
void c_orbiting_camera::set_forward(const real_vector3d *forward)
{
    euler_angles2d_from_vector3d(&m_angles, forward);
}

void c_orbiting_camera::enable_orientation(bool enabled) {
    m_orientation_enabled = enabled;
}
void c_orbiting_camera::enable_movement(bool enabled) {
    m_movement_enabled = enabled;
}

// @address 0x296420 ?make_valid_real_euler_angles2d@@YAXPATreal_euler_angles2d@@@Z
void make_valid_real_euler_angles2d(union real_euler_angles2d * angles)
{
    while (angles->yaw < 0.0f)
        angles->yaw += 6.2831855f;
    while (angles->yaw > 6.2831855f)
        angles->yaw -= 6.2831855f;
    if (angles->pitch < -1.5707964f)
        angles->pitch = -1.5707964f;
    if (angles->pitch > 1.5707964f)
        angles->pitch = 1.5707964f;
}

// match: 0x296900: the aiming vector is read through the object header array as a typed pointer and indexed with
// the handle mask. Shared accessors (object_header_get, datum_get, datum_index_to_absolute_index) let set_target
// reuse the header address computed for object_get_type; the inlined March unit_get_aiming_vector recomputes it.
static __forceinline s_object_header *object_headers_get()
{
    return (s_object_header *)tls_object_header_data()->data;
}

// @address 0x296900 ?set_target@c_orbiting_camera@@UAAXJ@Z
void c_orbiting_camera::set_target(long target)
{
    m_target = target;
    if (m_target != -1)
    {
        real_vector3d forward;
        if (((1 << object_get_type(m_target)) & _object_mask_unit) != 0)
            forward = ((unit_datum *)object_headers_get()[datum_index_to_absolute_index(m_target)].datum)->unit.aiming_vector;
        else
            ::object_get_orientation(m_target, &forward, 0);
        euler_angles2d_from_vector3d(&m_angles, &forward);
    }
    else
    {
        euler_angles2d_from_vector3d(&m_angles, &global_identity4x3->forward);
    }
}

// @address 0x2964a8 ?update@c_orbiting_camera@@UAAXJMPAUs_observer_command@@@Z
void c_orbiting_camera::update(long user_index, float seconds, s_observer_command *observer_command)
{
    if (m_target != -1)
    {
        const s_camera_globals *camera_globals = &g_camera_globals;
        object_datum *target = object_get(m_target);
        real_matrix4x3 *node_matrix = (real_matrix4x3 *)object_header_block_get(target, &target->node_matrices);
        observer_command->position = node_matrix->position;
        object_get_velocities(m_target, &observer_command->velocity, NULL);
        observer_command->position.z += m_height;
        observer_command->focus_distance = m_distance;
        if (m_distance_overridden)
        {
            observer_command->timer = camera_globals->orbiting_camera_transition_time;
            m_distance_overridden = false;
        }
        else
        {
            observer_command->timer = 0.0f;
        }
        observer_command->field_of_view = g_observer_default_field_of_view;
        observer_command->flags = 1;
        long controller_index = user_interface_controller_get_rumble_enabled(user_index);
        if (controller_index != -1 && camera_responding_gate((e_controller_index)controller_index) == 0)
        {
            float h_sens = input_abstraction_globals.look_settings[controller_index].preferences.horizontal_look_sensitivity;
            float h_scale = h_sens * 0.017453292f;
            s_game_input_state *input_state = &input_abstraction_globals.input_states[controller_index];
            float v_sens = input_abstraction_globals.look_settings[controller_index].preferences.vertical_look_sensitivity;
            float v_scale = v_sens * 0.017453292f;
            float scale = (float)pow((double)camera_globals->boost_scale, (double)input_state->buttons[0].down_amount());
            if (m_orientation_enabled)
            {
                m_angles.yaw += input_state->look.i * h_scale * seconds;
                m_angles.pitch += input_state->look.j * v_scale * seconds;
                make_valid_real_euler_angles2d(&m_angles);
            }
            if (m_movement_enabled)
            {
                float focus = m_distance;
                if (focus != 0.0f)
                {
                    float adjust = input_state->throttle.j / focus * camera_globals->side_scale * scale * seconds;
                    if (adjust <= -1.5707964f)
                        adjust = -1.5707964f;
                    else if (adjust >= 1.5707964f)
                        adjust = 1.5707964f;
                    m_angles.yaw -= adjust;
                }
                m_distance = focus - input_state->throttle.i * camera_globals->forward_scale * scale * seconds;
                if (input_state->buttons[15].down_amount() == 1.0f)
                    m_angles.pitch -= camera_globals->up_scale / m_distance * scale * seconds;
                else if (input_state->buttons[14].down_amount() == 1.0f)
                    m_angles.pitch += camera_globals->up_scale / m_distance * scale * seconds;
                make_valid_real_euler_angles2d(&m_angles);
            }
        }
        float low_pitch = -camera_globals->orbiting_camera_minimum_pitch;
        float tmp_pitch;
        if (m_angles.pitch > low_pitch)
            tmp_pitch = m_angles.pitch;
        else
            tmp_pitch = low_pitch;
        float high_pitch = -camera_globals->orbiting_camera_maximum_pitch;
        float new_pitch = m_angles.pitch;
        if (tmp_pitch < high_pitch)
        {
            if (m_angles.pitch <= low_pitch)
                new_pitch = low_pitch;
        }
        else
        {
            new_pitch = high_pitch;
        }
        m_angles.pitch = new_pitch;
        float height_scale = 1.0f;
        float minimum_distance = camera_globals->orbiting_camera_minimum_distance;
        float maximum_distance = camera_globals->orbiting_camera_maximum_distance;
        long root_index = -1;
        if (m_target != -1)
        {
            long object_index = m_target;
            do
            {
                root_index = object_index;
                object_index = object_get(object_index)->parent_object_index;
            } while (object_index != -1);
        }
        s_object_header *root_header = object_header_get(root_index);
        float root_radius = root_header->datum->bounding_radius;
        if (root_header->object_type == _object_type_vehicle)
        {
            height_scale = MAX(root_radius * 1.5f, 1.0f);
        }
        float min_focus = MAX(minimum_distance * height_scale, root_radius * 1.1f);
        float max_focus = maximum_distance * height_scale;
        if (min_focus > max_focus)
            max_focus = min_focus;
        m_distance = PIN(m_distance, min_focus, max_focus);
        make_valid_real_euler_angles2d(&m_angles);
        vector3d_from_euler_angles2d(&observer_command->forward, &m_angles);
        generate_up_vector3d(&observer_command->forward, &observer_command->up);
        long ignore_index = -1;
        if (m_target != -1)
        {
            long object_index = m_target;
            do
            {
                ignore_index = object_index;
                object_index = object_get(object_index)->parent_object_index;
            } while (object_index != -1);
        }
        observer_command->ignore_object_indices[0] = ignore_index;
        observer_command->ignore_object_count = 1;
        observer_command->flags |= 0x20;
        long center_index = -1;
        if (m_target != -1)
        {
            long object_index = m_target;
            do
            {
                center_index = object_index;
                object_index = object_get(object_index)->parent_object_index;
            } while (object_index != -1);
        }
        object_get_center_of_mass(center_index, &observer_command->parent_position);
        observer_command->flags |= 0x400;
    }
    observer_validate_camera_command(observer_command);
}
