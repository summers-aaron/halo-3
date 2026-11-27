#ifndef SCENARIO_SCENARIO_OBJECT_REFERENCE_FRAME_H
#define SCENARIO_SCENARIO_OBJECT_REFERENCE_FRAME_H

#include <stddef.h>
#include "objects/object_types.h"

// Elements of scenario::object_reference_frames. March object_refresh_reference_frames
// (82ec2d68) indexes 16-byte records and compares their object identifiers.
struct s_scenario_object_reference_frame
{
    c_object_identifier identifier;
    short node_index;
    short axis;
    unsigned short flags;
};

typedef char scenario_object_reference_frame_size[(sizeof(s_scenario_object_reference_frame) == 0x10) ? 1 : -1];
typedef char scenario_object_reference_frame_node_offset[(offsetof(s_scenario_object_reference_frame, node_index) == 8) ? 1 : -1];
typedef char scenario_object_reference_frame_axis_offset[(offsetof(s_scenario_object_reference_frame, axis) == 0xa) ? 1 : -1];
typedef char scenario_object_reference_frame_flags_offset[(offsetof(s_scenario_object_reference_frame, flags) == 0xc) ? 1 : -1];

#endif // SCENARIO_SCENARIO_OBJECT_REFERENCE_FRAME_H
