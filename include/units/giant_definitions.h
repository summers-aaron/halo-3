#ifndef UNITS_GIANT_DEFINITIONS_H
#define UNITS_GIANT_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "units/unit_definitions.h"

struct s_giant_variant
{
    union { float volume_scale; float buckle_duration; };
    union { long level; long buckle_movement_pattern; };
    float rise_duration;
    long rise_movement_pattern;
    union { float difficulty_scales[4]; float buckled_durations[4]; };
    float settle_scale;
    long buckle_marker_name;
    float buckle_width;
    float buckle_length;
    long buckle_width_steps;
    long buckle_length_steps;
    float buckle_pitch_minimum;
    float buckle_pitch_maximum;
    float buckle_roll_minimum;
    float buckle_roll_maximum;
    union { long value; long animation; };
    long impulse;
    long buckled_impulse;
    float position;
    float buckled_position;
};

struct s_giant_definition : unit_definition
{
    unsigned long giant_flags;
    float accelerate;
    float decelerate;
    float minimum_playback_rate;
    float elevation_scale;
    float ground_range;
    long variant_count;
    TAG_POINTER(s_giant_variant) variants;
    TAG_POINTER(void) variant_definition;
    float ik_scale;
};

typedef char giant_variant_size[(sizeof(s_giant_variant) == 0x5c) ? 1 : -1];
typedef char giant_variant_animation_offset[(offsetof(s_giant_variant, animation) == 0x48) ? 1 : -1];
typedef char giant_variant_impulse_offset[(offsetof(s_giant_variant, impulse) == 0x4c) ? 1 : -1];
typedef char giant_tag_accelerate_offset[(offsetof(s_giant_definition, accelerate) == 0x310) ? 1 : -1];
typedef char giant_tag_flags_offset[(offsetof(s_giant_definition, giant_flags) == 0x30c) ? 1 : -1];
typedef char giant_tag_variants_offset[(offsetof(s_giant_definition, variants) == 0x328) ? 1 : -1];

typedef char giant_tag_variant_definition_offset[(offsetof(s_giant_definition, variant_definition) == 0x32c) ? 1 : -1];
typedef char giant_tag_playback_rate_offset[(offsetof(s_giant_definition, minimum_playback_rate) == 0x318) ? 1 : -1];
typedef char giant_tag_ik_scale_offset[(offsetof(s_giant_definition, ik_scale) == 0x330) ? 1 : -1];
typedef char giant_tag_size[(sizeof(s_giant_definition) == 0x334) ? 1 : -1];

#endif // UNITS_GIANT_DEFINITIONS_H
