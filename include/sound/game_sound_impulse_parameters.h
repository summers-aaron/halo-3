#ifndef SOUND_GAME_SOUND_IMPULSE_PARAMETERS_H
#define SOUND_GAME_SOUND_IMPULSE_PARAMETERS_H

#include "sound/sound_source_types.h"

struct s_sound_tracker;
struct s_sound_tracking_data;
struct s_game_sound_deterministic_permutation;

// Parameters describing a single game sound impulse.
struct s_game_sound_impulse_internal_parameters
{
    s_sound_source source;
    long playback_label;
    long object_index;
    const s_sound_tracker *tracking;
    const s_sound_tracking_data *tracking_data;
    const s_game_sound_deterministic_permutation *permutation;
    unsigned long additional_override_flags;
    long source_type;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char game_sound_impulse_parameters_size[(sizeof(void *) != 4 || sizeof(s_game_sound_impulse_internal_parameters) == 0x68) ? 1 : -1];
#endif

#endif
