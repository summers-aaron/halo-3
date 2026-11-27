#ifndef SOUND_SOUND_XAUDIO_XENON_OUTPUT_TYPES_H
#define SOUND_SOUND_XAUDIO_XENON_OUTPUT_TYPES_H

#include "memory/wrapped_arrays.h"
#include "sound/game_sound_definitions.h"

#include "sound/sound_xaudio_xenon_source_state.h"

struct s_xaudio_platform_output : c_static_sized_dynamic_array<float, 36> {
    float reverb_level;
    float pitch_octaves;
    float direct_lpf;
    float reverb_lpf;
    float attenuation_loss;
    // March 82d24500: speaker-major matrix indexing.
    __forceinline float &get_channel_speaker(long channel, long channel_count, e_xaudio_output_speaker speaker)
    {
        return (*this)[speaker * channel_count + channel];
    }
};

typedef char xaudio_platform_output_size[(sizeof(s_xaudio_platform_output) == 0xa8) ? 1 : -1];

#endif // SOUND_SOUND_XAUDIO_XENON_OUTPUT_TYPES_H
