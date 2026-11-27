#ifndef SOUND_SOUND_XAUDIO_XENON_3D_VOICE_TYPES_H
#define SOUND_SOUND_XAUDIO_XENON_3D_VOICE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct s_platform_sound_voice_update_definition
{
    long listener;
    unsigned long flags;
    real_point3d position;
    real_vector3d velocity;
    real_vector3d forward;
    real_vector3d up;
    float reverb_lpf;
    float direct_lpf;
    float gain;
    float direct_rate;
    float reverb_rate;
};

struct s_sound_transmission_transform
{
    float silence_distance;
    float attack_distance;
    float minimum_distance;
    float maximum_distance;
};

struct s_xaudio_speaker_cross_mix
{
    float direct;
    float cross;
};

typedef char platform_sound_voice_update_size[(sizeof(s_platform_sound_voice_update_definition) == 0x4c) ? 1 : -1];
typedef char platform_sound_voice_update_flags_offset[(offsetof(s_platform_sound_voice_update_definition, flags) == 0x4) ? 1 : -1];
typedef char platform_sound_voice_update_position_offset[(offsetof(s_platform_sound_voice_update_definition, position) == 0x8) ? 1 : -1];
typedef char platform_sound_voice_update_reverb_lpf_offset[(offsetof(s_platform_sound_voice_update_definition, reverb_lpf) == 0x38) ? 1 : -1];
typedef char platform_sound_voice_update_gain_offset[(offsetof(s_platform_sound_voice_update_definition, gain) == 0x40) ? 1 : -1];
typedef char platform_sound_voice_update_direct_rate_offset[(offsetof(s_platform_sound_voice_update_definition, direct_rate) == 0x44) ? 1 : -1];
typedef char platform_sound_voice_update_reverb_rate_offset[(offsetof(s_platform_sound_voice_update_definition, reverb_rate) == 0x48) ? 1 : -1];
typedef char sound_transmission_transform_size[(sizeof(s_sound_transmission_transform) == 16) ? 1 : -1];
typedef char xaudio_speaker_cross_mix_size[(sizeof(s_xaudio_speaker_cross_mix) == 8) ? 1 : -1];

#endif // SOUND_SOUND_XAUDIO_XENON_3D_VOICE_TYPES_H
