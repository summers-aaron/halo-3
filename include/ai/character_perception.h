#ifndef AI_CHARACTER_PERCEPTION_H
#define AI_CHARACTER_PERCEPTION_H

#include <stddef.h>

// The selected perception block has a 0x2c stride in character_definitions.cpp.
// These fields combine the distance, frustum, hearing and awareness readers.
struct s_mode_entry
{
    long flags;
    float maximum_distance;
    float near_angle;
    float far_angle;
    float maximum_angle;
    float maximum_vision_distance;
    float hearing_distance;
    unsigned long : 32;
    unsigned long : 32;
    float rate;
    float surprise_distance; // actor_stimulus_surprise reads +0x28
};
typedef char character_perception_size[(sizeof(s_mode_entry) == 0x2c) ? 1 : -1];
typedef char character_perception_rate_offset[(offsetof(s_mode_entry, rate) == 0x24) ? 1 : -1];

// Character perception block selected by the actor's perception mode.
// Defined in tu/ailibxbox/character_definitions.cpp.
extern "C" s_mode_entry *actor_perception_properties_get(int actor_index);

#endif // AI_CHARACTER_PERCEPTION_H
