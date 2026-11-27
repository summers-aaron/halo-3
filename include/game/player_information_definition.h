#ifndef GAME_PLAYER_INFORMATION_DEFINITION_H
#define GAME_PLAYER_INFORMATION_DEFINITION_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"

// Element of the globals tag's player information block. Field names follow the tag layout
// (https://github.com/TheGuardians/TagTool/blob/master/TagTool/Tags/Definitions/Globals.cs).
struct s_player_information_definition
{
    float walking_speed;
    float run_forward;
    float run_backward;
    float run_sideways;
    float run_acceleration;
    float sneak_forward;
    float sneak_backward;
    float sneak_sideways;
    float sneak_acceleration;
    float airborne_acceleration;
    real_point3d grenade_origin;
    float stun_movement_penalty;
    float stun_turning_penalty;
    float stun_jumping_penalty;
    float stun_penalty_minimum_time;
    float stun_penalty_maximum_time;
    real_bounds first_person_idle_time;
    float first_person_idle_skip_fraction;
    s_tag_reference respawn_countdown_sound;
    s_tag_reference respawn_final_sound;
    s_tag_reference respawn_effect;
    long zoom_level_count;
    float zoom_magnification_min;
    float zoom_magnification_max;
    s_tag_reference active_camouflage_sounds[2];
    s_tag_reference damage_response;
};

typedef char player_information_run_forward_offset[
    (offsetof(s_player_information_definition, run_forward) == 4) ? 1 : -1];
typedef char player_information_grenade_origin_offset[
    (offsetof(s_player_information_definition, grenade_origin) == 0x28) ? 1 : -1];
typedef char player_information_stun_movement_offset[
    (offsetof(s_player_information_definition, stun_movement_penalty) == 0x34) ? 1 : -1];
typedef char player_information_stun_turning_offset[
    (offsetof(s_player_information_definition, stun_turning_penalty) == 0x38) ? 1 : -1];

typedef char player_information_countdown_offset[
    (offsetof(s_player_information_definition, respawn_countdown_sound) + offsetof(s_tag_reference, index) == 0x60) ? 1 : -1];
typedef char player_information_final_offset[
    (offsetof(s_player_information_definition, respawn_final_sound) + offsetof(s_tag_reference, index) == 0x70) ? 1 : -1];
typedef char player_information_effect_offset[
    (offsetof(s_player_information_definition, respawn_effect) + offsetof(s_tag_reference, index) == 0x80) ? 1 : -1];
typedef char player_information_damage_response_offset[
    (offsetof(s_player_information_definition, damage_response) + offsetof(s_tag_reference, index) == 0xbc) ? 1 : -1];
typedef char player_information_size[(sizeof(s_player_information_definition) == 0xc0) ? 1 : -1];

typedef char player_information_stun_minimum_offset[
    (offsetof(s_player_information_definition, stun_penalty_minimum_time) == 0x40) ? 1 : -1];
typedef char player_information_stun_maximum_offset[
    (offsetof(s_player_information_definition, stun_penalty_maximum_time) == 0x44) ? 1 : -1];
typedef char player_information_zoom_offset[
    (offsetof(s_player_information_definition, zoom_level_count) == 0x84) ? 1 : -1];

typedef char player_information_camouflage_sound_offset[
    (offsetof(s_player_information_definition, active_camouflage_sounds) + offsetof(s_tag_reference, index) == 0x9c) ? 1 : -1];

#endif // GAME_PLAYER_INFORMATION_DEFINITION_H
