#ifndef CAMERA_CAMERA_TRACK_H
#define CAMERA_CAMERA_TRACK_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// The trak tag's control points, interpolated by camera_track_splut.
struct camera_track_point
{
    real_point3d position;
    real_quaternion orientation;
};

struct camera_track_definition
{
    long flags;
    c_typed_tag_block<camera_track_point> control_points;
};

typedef char camera_track_point_size[(sizeof(camera_track_point) == 0x1c) ? 1 : -1];
typedef char camera_track_definition_size[(sizeof(void *) != 4 || sizeof(camera_track_definition) == 0x10) ? 1 : -1];

#endif // CAMERA_CAMERA_TRACK_H
