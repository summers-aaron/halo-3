#ifndef SOUND_SOUND_IMPULSE_DEFINITION_H
#define SOUND_SOUND_IMPULSE_DEFINITION_H

#include <stddef.h>
#include "sound/sound_source_types.h"
#include "game/sound_tracking_data_types.h"

struct s_sound_effect_track_data
{
    long effect_index;
    const void *callback;
};

struct s_sound_impulse_definition
{
    unsigned long flags;
    unsigned long playback_flags;
    unsigned long datum_flags;
    s_sound_source source;
    long object_index;
    long controller_index;
    const void *callbacks;
    union
    {
#ifdef HALO3_64BIT
        char event_parameters[56];
#else
        char event_parameters[48];
#endif
        struct
        {
            s_sound_tracking_data tracking_data;
            s_sound_effect_track_data track_data;
        };
    };
    short event_size;
    short hardware_voice;
    signed char voice_state;
    real_decibel gain;
    long playback_index;
    short pitch_range;
    short permutation;
};

typedef char sound_impulse_definition_size[(sizeof(void *) != 4 || ((sizeof(s_sound_impulse_definition) == 0xa8))) ? 1 : -1];
typedef char sound_impulse_gain_offset[(sizeof(void *) != 4 || ((offsetof(s_sound_impulse_definition, gain) == 0x9c))) ? 1 : -1];
typedef char sound_impulse_track_data_offset[(sizeof(void *) != 4 || ((offsetof(s_sound_impulse_definition, track_data) == 0x8c))) ? 1 : -1];

#endif
