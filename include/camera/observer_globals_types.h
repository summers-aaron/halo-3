#ifndef CAMERA_OBSERVER_GLOBALS_TYPES_H
#define CAMERA_OBSERVER_GLOBALS_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "camera/observer_command.h"
#include "math/matrix_math_types.h"
#include "render/render_player_view_observer_flags_types.h"

struct s_observer_view_offset { float horizontal, vertical; };
struct s_observer_result
{
    real_point3d position;
    s_location location;
    real_vector3d velocity, angular_velocity;
    union { real_vector3d forward; real_point3d focus; };
    real_vector3d up;
    float horizontal_field_of_view;
    union { s_observer_depth_of_field depth_of_field; render_player_view_observer_flags flags; };
    float aspect_ratio;
    s_observer_view_offset view_offset;
    float direction_error, vertical_field_of_view, field_of_view_scale;
};
typedef char observer_result_size[(sizeof(s_observer_result) == 0x70) ? 1 : -1];
typedef char observer_result_forward_offset[(offsetof(s_observer_result, forward) == 0x28) ? 1 : -1];
typedef char observer_result_flags_offset[(offsetof(s_observer_result, flags) == 0x44) ? 1 : -1];
typedef char observer_result_vertical_fov_offset[(offsetof(s_observer_result, vertical_field_of_view) == 0x68) ? 1 : -1];
struct s_observer_result_fields
{
    real_point3d position;
    union { long location; s_location bsp_location; };
    real_vector3d velocity, angular_velocity;
    union { real_vector3d forward; real_point3d forward_as_point; };
    union { real_vector3d up; real_point3d up_as_point; };
    float horizontal_field_of_view;
    s_observer_depth_of_field depth_of_field;
    float aspect_ratio;
    s_observer_view_offset view_offset;
    float direction_error, vertical_field_of_view, field_of_view_scale;
};
struct s_observer_camera_result
{
    real_point3d position;
    s_location location;
    real_vector3d velocity, angular_velocity;
    real_vector3d forward;
    real_vector3d up;
    float horizontal_field_of_view;
    s_observer_depth_of_field depth_of_field;
    float aspect_ratio;
    float view_value;
};
// One float per motion parameter (focus_point[3], focus_offset[3], view_offset[2],
// focus_distance, field_of_view, motion_forward[3]); the first and last triples
// are transformed as vectors, and update_result reads them as linear/angular rates.
struct s_observer_motion_values
{
    union
    {
        float all[13];
        struct
        {
            real_vector3d linear;
            real_vector3d focus_offset;
            float view_offset[2];
            float focus_distance;
            float field_of_view;
            real_vector3d angular;
        };
    };
};
struct s_player_block
{
    long header_signature;
    s_observer_command *command;
    s_observer_command previous;
    union { unsigned char flags; bool updated; };
    union { bool pending; unsigned char reset; };
    union { bool using_matrix; unsigned char command_id; };
    bool focus_offset_in_view_space;
    float interpolation, interpolation_rate;
    long parent_object_index, parent_unit_index;
    bool result_valid;
    union
    {
        s_observer_result camera_result;
        s_observer_result_fields result;
        s_observer_camera_result camera;
    };
    real_point3d focus_point;
    real_vector3d focus_offset;
    s_observer_view_offset view_offset;
    float focus_distance;
    float field_of_view;
    union { real_point3d position; real_vector3d motion_forward; };
    union { real_vector3d motion_up; real_vector3d forward; };
    real_matrix4x3 matrix;
    s_observer_motion_values velocities;
    s_observer_motion_values accelerations;
    float coefficients_a[13], coefficients_b[13], coefficients_c[13];
    float coefficients_d[13], coefficients_e[13], coefficients_f[13];
    s_observer_motion_values displacements;
    long trailer_signature;
};
struct s_player_globals
{
    float phase;
    s_player_block blocks[4];
    bool initialized;
    bool flag;
    bool block_one_frame;
};

typedef char observer_camera_forward_offset[(offsetof(s_observer_camera_result, forward) == 0x28) ? 1 : -1];
typedef char observer_camera_velocity_offset[(offsetof(s_observer_camera_result, velocity) == 0x10) ? 1 : -1];
typedef char observer_camera_depth_of_field_offset[(offsetof(s_observer_camera_result, depth_of_field) == 0x44) ? 1 : -1];
typedef char observer_camera_aspect_ratio_offset[(offsetof(s_observer_camera_result, aspect_ratio) == 0x58) ? 1 : -1];
typedef char observer_camera_view_offset[(offsetof(s_observer_camera_result, view_value) == 0x5c) ? 1 : -1];
typedef char observer_camera_size[(sizeof(s_observer_camera_result) == 0x60) ? 1 : -1];
typedef char observer_player_block_size[(sizeof(void *) != 4 || ((sizeof(s_player_block) == 0x3c8))) ? 1 : -1];
typedef char observer_player_result_offset[(sizeof(void *) != 4 || ((offsetof(s_player_block, camera_result) == 0x10c))) ? 1 : -1];
typedef char observer_player_distance_scale_offset[
    (sizeof(void *) != 4 || ((offsetof(s_player_block, camera_result) +
    offsetof(s_observer_result, field_of_view_scale) == 0x178))) ? 1 : -1];
typedef char observer_result_location_offset[(offsetof(s_observer_result, location) == 0xc) ? 1 : -1];
typedef char observer_player_location_offset[
    (sizeof(void *) != 4 || ((offsetof(s_player_block, camera_result) +
    offsetof(s_observer_result, location) == 0x118))) ? 1 : -1];
typedef char observer_player_command_offset[(sizeof(void *) != 4 || ((offsetof(s_player_block, previous) == 8))) ? 1 : -1];
typedef char observer_player_focus_offset[(sizeof(void *) != 4 || ((offsetof(s_player_block, focus_point) == 0x17c))) ? 1 : -1];
typedef char observer_player_matrix_offset[(sizeof(void *) != 4 || ((offsetof(s_player_block, matrix) == 0x1bc))) ? 1 : -1];
typedef char observer_motion_values_size[(sizeof(s_observer_motion_values) == 13 * sizeof(float)) ? 1 : -1];
typedef char observer_player_velocity_offset[(sizeof(void *) != 4 || ((offsetof(s_player_block, velocities) == 0x1f0))) ? 1 : -1];
typedef char observer_player_displacement_offset[(sizeof(void *) != 4 || ((offsetof(s_player_block, displacements) == 0x390))) ? 1 : -1];
extern __declspec(thread) s_player_globals *g_observer_globals;

static __forceinline s_player_globals *observer_get_globals() { return g_observer_globals; }
static __forceinline const s_observer_result *observer_get_camera(long user_index)
{
    return &g_observer_globals->blocks[user_index].camera_result;
}
#endif // CAMERA_OBSERVER_GLOBALS_TYPES_H
