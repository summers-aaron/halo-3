#ifndef AI_CHARACTER_FIRING_PATTERN_PROPERTIES_H
#define AI_CHARACTER_FIRING_PATTERN_PROPERTIES_H

#include "tag_files/tag_types.h"

struct character_firing_pattern {
 float rate, burst_time, burst_separation, damage_modifier;
 float projectile_error, projectile_error_minimum, projectile_error_maximum, burst_error;
 float burst_error_minimum, burst_error_maximum, leading, leading_variance;
 float aiming, aiming_variance, tracking, tracking_variance;
};

struct character_firing_pattern_properties
{
    s_tag_reference weapon;
    c_typed_tag_block<character_firing_pattern> patterns;
};

typedef char character_firing_pattern_properties_size[(sizeof(character_firing_pattern_properties) == 0x1c) ? 1 : -1];

#endif
