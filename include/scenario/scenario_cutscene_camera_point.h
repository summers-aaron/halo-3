#ifndef SCENARIO_SCENARIO_CUTSCENE_CAMERA_POINT_H
#define SCENARIO_SCENARIO_CUTSCENE_CAMERA_POINT_H

#include <stddef.h>
#include "math/real_math_types.h"

// Xbox 360 scenario camera-point record; the four bytes after name are unused.
struct scenario_cutscene_camera_point
{
    short flags;
    short type;
    char name[32];
    long pad24;
    real_point3d position;
    real_euler_angles3d orientation;
};
typedef char scenario_camera_point_size[(sizeof(scenario_cutscene_camera_point) == 0x40) ? 1 : -1];
typedef char scenario_camera_point_position_offset[(offsetof(scenario_cutscene_camera_point, position) == 0x28) ? 1 : -1];
typedef char scenario_camera_point_orientation_offset[(offsetof(scenario_cutscene_camera_point, orientation) == 0x34) ? 1 : -1];

#endif
