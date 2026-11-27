#ifndef EFFECTS_EFFECT_DEFINITIONS_H
#define EFFECTS_EFFECT_DEFINITIONS_H

#include <stddef.h>

#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_types.h"
#include "effects/event_flags_types.h"

enum e_effect_definition_flags
{
    _effect_definition_force_use_own_lightprobe_bit = 7,
    _effect_definition_lightprobe_bit = 10,
    _effect_definition_lightprobe_always_bit = 11,
    _effect_definition_always_visible_bit = 5,
    _effect_definition_cannot_be_restarted_bit = 6,
    _effect_definition_force_looping_bit = 8,
    _effect_definition_first_person_visibility_bit = 9,
    _effect_definition_delete_immediately_bit = 0,
    _effect_definition_parallel_events_bit = 1,
    _effect_definition_recreate_systems_bit = 2,
    _effect_definition_ages_weapon_bit = 3,
    _effect_definition_node_position_only_bit = 4,
    _effect_definition_underwater_test_bit = 14,
    _effect_definition_lens_flares_bit = 15
};
struct s_effect_definition {
    c_flags_no_init<e_effect_definition_flags, unsigned long, 32> flags;
    unsigned long seed;
    float maximum_separation;
    float minimum_separation;
    float death_delay;
    unsigned char limit_type;
    char : 8;
    char : 8;
    char : 8;
    short loop_start_event;
    float danger_radius;
    s_tag_block locations;
    s_tag_block events;
    s_tag_reference sound;
    signed char location;
    unsigned char sound_scale_source;
    float minimum_distance;
    float maximum_distance;
    float lightprobe_fade_time;
    float local_space_death_delay;
    s_tag_block conical_distribution;
};
struct s_effect_conical_distribution
{
    short angular_count;
    short radial_count;
    float exponent;
    float maximum;
};
struct event_datum {            // 20 bytes
    short identifier;
    c_flags_no_init<e_event_flags, unsigned char, 4> flags;
    unsigned char event_block_index;
    unsigned short event_counter;
    int next_event_index;
    float time;
    float duration;
};

typedef char event_datum_next_event_index_offset[(offsetof(event_datum, next_event_index) == 8) ? 1 : -1];
typedef char event_datum_size[(sizeof(event_datum) == 0x14) ? 1 : -1];

#if defined(_XBOX)
typedef char s_effect_definition_sound_offset[(offsetof(s_effect_definition, sound) == 0x38) ? 1 : -1];
typedef char s_effect_definition_minimum_distance_offset[(offsetof(s_effect_definition, minimum_distance) == 0x4c) ? 1 : -1];
typedef char s_effect_definition_danger_radius_offset[(offsetof(s_effect_definition, danger_radius) == 0x1c) ? 1 : -1];
typedef char s_effect_definition_size[(sizeof(s_effect_definition) == 0x68) ? 1 : -1];
#endif

#endif // EFFECTS_EFFECT_DEFINITIONS_H
