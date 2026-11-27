#ifndef SOUND_GAME_SOUND_PLAYER_EFFECTS_DEFINITIONS_H
#define SOUND_GAME_SOUND_PLAYER_EFFECTS_DEFINITIONS_H

#include "cseries/cseries_types.h"
#include "objects/damage_response_definition.h"

struct s_damage_effect_player_response_sound
{
    string_id playback_label;
    float duration;
    c_function_definition function;
};

struct s_damage_effect_player_response
{
    short response_type;
    unsigned short : 16;
    s_damage_response_screen_flash_definition screen_flash;
    s_damage_response_rumble_frequency rumble[2];
    s_damage_effect_player_response_sound sound_effect;
};

typedef char damage_effect_player_response_size[(sizeof(s_damage_effect_player_response) == 0x70) ? 1 : -1];
typedef char damage_effect_player_response_sound_offset[(offsetof(s_damage_effect_player_response, sound_effect) == 0x54) ? 1 : -1];

#endif
