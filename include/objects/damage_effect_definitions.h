#ifndef OBJECTS_DAMAGE_EFFECT_DEFINITIONS_H
#define OBJECTS_DAMAGE_EFFECT_DEFINITIONS_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "sound/game_sound_player_effects_definitions.h"
#include "tag_files/tag_types.h"

struct damage_definition
{
    unsigned short side_effect;
    short category;
    unsigned long flags;
    float aoe_core_radius;
    float damage_lower_bound;
    real_bounds damage_upper_bound;
    float dmg_inner_cone_angle;
    float dmg_outer_cone_angle;
    float active_camouflage_damage;
    float stun;
    float maximum_stun;
    float stun_time;
    float instantaneous_acceleration;
    float rider_direct_damage_scale;
    float rider_maximum_transfer_damage_scale;
    float rider_minimum_transfer_damage_scale;
    long general_damage;
    long specific_damage;
    long custom_response_label;
};

struct s_damage_effect_definition
{
    real_bounds radius;
    float cutoff_scale;
    unsigned long effect_flags;
    damage_definition damage;
    float ai_stun_radius;
    real_bounds ai_stun_bounds;
    float shake_radius;
    float emp_radius;
    float aoe_spike_radius;
    float aoe_spike_damage_bump;
    c_typed_tag_block<s_damage_effect_player_response> player_responses;
    s_tag_reference damage_response;
    float impulse_duration;
    short fade_function;
    float rotation;
    float pushback;
    real_bounds jitter;
    float shake_duration;
    short falloff_function;
    float random_translation;
    float random_rotation;
    short wobble_function;
    float wobble_function_period;
    float wobble_weight;
    s_tag_reference sound;
    float forward_velocity;
    float forward_radius;
    float forward_exponent;
    float outward_velocity;
    float outward_radius;
    float outward_exponent;
};

typedef char damage_definition_size[(sizeof(damage_definition) == 0x4c) ? 1 : -1];
typedef char damage_effect_damage_offset[(offsetof(s_damage_effect_definition, damage) == 0x10) ? 1 : -1];
typedef char damage_effect_definition_size[(sizeof(s_damage_effect_definition) == 0xf0) ? 1 : -1];
typedef char damage_effect_category_offset[(offsetof(s_damage_effect_definition, damage.category) == 0x12) ? 1 : -1];
typedef char damage_effect_player_responses_offset[(offsetof(s_damage_effect_definition, player_responses) == 0x78) ? 1 : -1];
typedef char damage_effect_sound_index_offset[(offsetof(s_damage_effect_definition, sound.index) == 0xd4) ? 1 : -1];

#endif // OBJECTS_DAMAGE_EFFECT_DEFINITIONS_H
