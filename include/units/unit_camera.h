#ifndef UNITS_UNIT_CAMERA_H
#define UNITS_UNIT_CAMERA_H

#include <stddef.h>
#include "cseries/cseries_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// The camera block of a unit or seat definition, and what unit_get_camera_info (players.cpp) reports from it.
// Field names follow the readers: following_camera.cpp (tracks, acceleration) and player_control.cpp (pitch).

struct s_unit_camera_track
{
    s_tag_reference track;
};

struct s_unit_camera_acceleration_axis
{
    float scale;
    float magnitude;
    float exponent;
    float maximum;
    float aligned_scale;
    float unaligned_scale;
};

struct s_unit_camera_acceleration
{
    s_unit_camera_acceleration_axis axes[3];
    float rate;
};

struct s_unit_camera
{
    unsigned short flags;
    string_id camera_marker_name;
    string_id camera_submerged_marker_name;
    float pitch_auto_level;
    real_bounds pitch_range;
    c_typed_tag_block<s_unit_camera_track> camera_tracks;
    float pitch_lower_margin;
    float pitch_upper_margin;
    float pitch_recenter_rate;
    c_typed_tag_block<s_unit_camera_acceleration> camera_acceleration;
};

struct s_unit_camera_info
{
    long unit_index;
    short seat_index;
    const s_unit_camera *camera;
    real_point3d position;
    long weapon_index;
};

typedef char unit_camera_acceleration_size[(sizeof(s_unit_camera_acceleration) == 76) ? 1 : -1];
typedef char unit_camera_tracks_offset[(offsetof(s_unit_camera, camera_tracks) == 24) ? 1 : -1];
typedef char unit_camera_acceleration_offset[(offsetof(s_unit_camera, camera_acceleration) == 48) ? 1 : -1];
typedef char unit_camera_size[(sizeof(s_unit_camera) == 60) ? 1 : -1];
typedef char unit_camera_info_size[(sizeof(void *) != 4 || (sizeof(s_unit_camera_info) == 28)) ? 1 : -1];

#endif // UNITS_UNIT_CAMERA_H
