#ifndef EFFECTS_EFFECT_EVENT_DEFINITIONS_H
#define EFFECTS_EFFECT_EVENT_DEFINITIONS_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct effect_acceleration_definition {
    short environment;
    short disposition;
    short location_index;
    float magnitude;
    float minimum;
    float maximum;
};

struct effect_event_definition {
    long name;
    unsigned long flags;
    unsigned char limit_type;
    float skip_fraction;
    real_bounds delay_bounds;
    real_bounds duration_bounds;
    s_tag_block parts;
    s_tag_block accelerations;
    s_tag_block particle_systems;
};

struct effect_location_definition {
    long marker_name;
    unsigned long flags;
    unsigned char limit_type;
};

struct effect_part_definition {
    short environment;
    short disposition;
    short location_index;
    short secondary_location_index;
    unsigned short flags;
    unsigned char limit_type;
    unsigned char render_type;
    unsigned long type;
    s_tag_reference definition;
    float minimum_velocity;
    float maximum_velocity;
    real_euler_angles2d velocity_orientation;
    float velocity_angle;
    float minimum_angular_velocity;
    float maximum_angular_velocity;
    float minimum_radius;
    float maximum_radius;
    real_point3d relative_offset;
    struct { float yaw, pitch; } relative_orientation;
    unsigned long scale_a_mask;
    unsigned long scale_b_mask;
};

typedef char effect_acceleration_size[(sizeof(effect_acceleration_definition) == 0x14) ? 1 : -1];
typedef char effect_event_parts_offset[(offsetof(effect_event_definition, parts) == 0x20) ? 1 : -1];
typedef char effect_location_size[(sizeof(effect_location_definition) == 0xc) ? 1 : -1];
typedef char effect_location_flags_offset[(offsetof(effect_location_definition, flags) == 4) ? 1 : -1];
typedef char effect_part_tag_offset[(offsetof(effect_part_definition, definition) + offsetof(s_tag_reference, index) == 0x1c) ? 1 : -1];

#endif
