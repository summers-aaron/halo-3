#ifndef AI_AI_GLOBALS_DEFINITION_H
#define AI_AI_GLOBALS_DEFINITION_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct s_ai_globals_gravemind_definition
{
    float minimum_retreat_time;
    float ideal_retreat_time;
    float maximum_retreat_time;
};

// AI damage scales, danger weights, movement limits and resurrection settings in the globals tag.
struct s_ai_globals_definition
{
    float infantry_scale;
    float vehicle_scale;
    float occupied_vehicle_scale;
    float danger_broadly_facing;
    long pad10;
    float danger_shooting_near;
    long pad18;
    float danger_shooting_at;
    long pad20;
    float danger_extremely_close;
    long pad28;
    float danger_shield_damage;
    float danger_extended_shield_damage;
    float danger_body_damage;
    float danger_extended_body_damage;
    char pad3c[0x6c - 0x3c];
    union
    {
        s_tag_reference global_dialogue;
        struct
        {
            unsigned long dialogue_globals_group_tag;
            TAG_POINTER(const char) dialogue_globals_name;
            long dialogue_globals_name_length;
            long dialogue_globals;
        };
    };
    long mission_dialogue_playback_label;         // mission_dialogue_play_line default, March 82f9c070 reads +0x7c
    char pad80[0x94 - 0x80];
    float jump_velocities[6];                   // by hint jump type
    float max_jump_down_heights[6];             // by hint jump type
    real_bounds hoist_heights[3];               // step, crouch, stand
    char pad_dc[0xf4 - 0xdc];
    real_bounds vault_heights[2];               // step, crouch
    char pad_104[0x134 - 0x104];
    c_typed_tag_block<s_ai_globals_gravemind_definition> gravemind_properties;
    char pad_140[0x170 - 0x140];
    float scariness_threshold;                    // actor_is_majorly_scary
    float weapon_scariness_threshold;             // unit_has_scary_weapon
    float player_scariness;
    float berserk_scariness;
    float ability_scariness;
    float equipment_scariness;
    float morph_delays[3];
    float delay;
    float pill_radius;
    float clump_radius;
    float combat_radius;
    long styles_count;
    TAG_POINTER(s_tag_reference) styles;
};

typedef char ai_globals_mission_dialogue_playback_offset[(offsetof(s_ai_globals_definition, mission_dialogue_playback_label) == 0x7c) ? 1 : -1];

typedef char ai_globals_definition_size[(sizeof(s_ai_globals_definition) == 0x1ac) ? 1 : -1];
typedef char ai_globals_gravemind_size[(sizeof(s_ai_globals_gravemind_definition) == 0xc) ? 1 : -1];
typedef char ai_globals_dialogue_reference_offset[(offsetof(s_ai_globals_definition, global_dialogue) == 0x6c) ? 1 : -1];
typedef char ai_globals_gravemind_offset[(offsetof(s_ai_globals_definition, gravemind_properties) == 0x134) ? 1 : -1];
typedef char ai_globals_morph_delays_offset[(offsetof(s_ai_globals_definition, morph_delays) == 0x188) ? 1 : -1];
typedef char ai_globals_scariness_threshold_offset[(offsetof(s_ai_globals_definition, scariness_threshold) == 0x170) ? 1 : -1];
typedef char ai_globals_player_scariness_offset[(offsetof(s_ai_globals_definition, player_scariness) == 0x178) ? 1 : -1];
typedef char ai_globals_equipment_scariness_offset[(offsetof(s_ai_globals_definition, equipment_scariness) == 0x184) ? 1 : -1];
typedef char ai_globals_definition_offset_dialogue_globals[(offsetof(s_ai_globals_definition, dialogue_globals) == 0x78) ? 1 : -1];
typedef char ai_globals_definition_offset_jump_velocities[(offsetof(s_ai_globals_definition, jump_velocities) == 0x94) ? 1 : -1];
typedef char ai_globals_definition_offset_max_jump_down_heights[(offsetof(s_ai_globals_definition, max_jump_down_heights) == 0xac) ? 1 : -1];
typedef char ai_globals_definition_offset_hoist_heights[(offsetof(s_ai_globals_definition, hoist_heights) == 0xc4) ? 1 : -1];
typedef char ai_globals_definition_offset_vault_heights[(offsetof(s_ai_globals_definition, vault_heights) == 0xf4) ? 1 : -1];

typedef char ai_globals_danger_offset[(offsetof(s_ai_globals_definition, danger_broadly_facing) == 0x0c) ? 1 : -1];
typedef char ai_globals_body_danger_offset[(offsetof(s_ai_globals_definition, danger_extended_body_damage) == 0x38) ? 1 : -1];

typedef char ai_globals_infantry_scale_offset[(offsetof(s_ai_globals_definition, infantry_scale) == 0) ? 1 : -1];
typedef char ai_globals_vehicle_scale_offset[(offsetof(s_ai_globals_definition, vehicle_scale) == 4) ? 1 : -1];
typedef char ai_globals_occupied_scale_offset[(offsetof(s_ai_globals_definition, occupied_vehicle_scale) == 8) ? 1 : -1];
typedef char ai_globals_resurrection_delay_offset[(offsetof(s_ai_globals_definition, delay) == 0x194) ? 1 : -1];
typedef char ai_globals_resurrection_combat_offset[(offsetof(s_ai_globals_definition, combat_radius) == 0x1a0) ? 1 : -1];

typedef char ai_globals_styles_count_offset[(offsetof(s_ai_globals_definition, styles_count) == 0x1a4) ? 1 : -1];
typedef char ai_globals_styles_offset[(offsetof(s_ai_globals_definition, styles) == 0x1a8) ? 1 : -1];

#endif // AI_AI_GLOBALS_DEFINITION_H
