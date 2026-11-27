#ifndef CAMERA_OBSERVER_COMMAND_H
#define CAMERA_OBSERVER_COMMAND_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "camera/observer_depth_of_field_types.h"

struct s_observer_command
{
    unsigned long flags;
    real_point3d position;
    real_point3d focus_position;
    real_vector2d focus_offset;
    float focus_distance;
    float field_of_view;
    real_vector3d forward;
    real_vector3d up;
    // Point view of the velocity for the point validation helpers.
    union { real_vector3d velocity; real_point3d velocity_as_point; };
    real_matrix4x3 focus_matrix;
    long crosshair_type;
    real_point3d parent_position;
    float timer;
    long ignore_object_indices[2];
    long ignore_object_count;
    real_point3d pill_base;
    float pill_height;
    float pill_width;
    unsigned char parameter_flags[8];
    float parameter_timers[6];
    s_observer_depth_of_field depth_of_field;
};

typedef char observer_command_size[(sizeof(s_observer_command) == 236) ? 1 : -1];
typedef char observer_command_focus_distance_offset[(offsetof(s_observer_command, focus_distance) == 36) ? 1 : -1];
typedef char observer_command_fov_offset[(offsetof(s_observer_command, field_of_view) == 40) ? 1 : -1];
typedef char observer_command_forward_offset[(offsetof(s_observer_command, forward) == 44) ? 1 : -1];
typedef char observer_command_up_offset[(offsetof(s_observer_command, up) == 56) ? 1 : -1];

typedef char observer_command_velocity_offset[(offsetof(s_observer_command, velocity) == 68) ? 1 : -1];
typedef char observer_command_parameter_flags_offset[(offsetof(s_observer_command, parameter_flags) == 184) ? 1 : -1];
typedef char observer_command_parameter_timers_offset[(offsetof(s_observer_command, parameter_timers) == 192) ? 1 : -1];

#endif // CAMERA_OBSERVER_COMMAND_H
