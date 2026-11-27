#ifndef SOUND_SOUND_MIX_DEFINITION_H
#define SOUND_SOUND_MIX_DEFINITION_H

#include <stddef.h>
#include "sound/game_sound_definitions.h"

struct s_sound_surround_mix_definition {
    real_decibel front;
    real_decibel rear;
};

struct s_sound_center_mix_definition {
    real_decibel front;
    real_decibel center;
};

struct s_sound_global_mix_configuration {
    real_decibel mono_attenuation;
    real_decibel spatial_attenuation;
    real_decibel front_attenuation;
    s_sound_center_mix_definition center[2];
    real_decibel stereo_attenuation;
    float solo_player_fade_out_delay;
    float solo_player_fade_out_time;
    float solo_player_fade_in_time;
    float default_looping_fade_time;
    float global_mix_values[4];
    float blend;
    float gain;
};

struct sound_manager_mix_definition {
    // Opaque leading mix settings, untouched by this unit.
    char mix_settings_storage[24];
    s_sound_surround_mix_definition first;
    s_sound_surround_mix_definition ambient;
    s_sound_global_mix_configuration global;
};

typedef char sound_global_mix_configuration_size[(sizeof(s_sound_global_mix_configuration) == 0x48) ? 1 : -1];
typedef char sound_mix_default_looping_fade_offset[(offsetof(sound_manager_mix_definition, global.default_looping_fade_time) == 0x54) ? 1 : -1];

#endif
