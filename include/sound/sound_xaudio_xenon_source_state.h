#ifndef SOUND_SOUND_XAUDIO_XENON_SOURCE_STATE_H
#define SOUND_SOUND_XAUDIO_XENON_SOURCE_STATE_H

#include "sound/game_sound_definitions.h"
#include "sound/sound_xaudio_xenon_3d_voice_types.h"

enum e_xaudio_output_speaker {
    _xaudio_output_speaker_front_left,
    _xaudio_output_speaker_front_right,
    _xaudio_output_speaker_front_center,
    _xaudio_output_speaker_low_frequency,
    _xaudio_output_speaker_back_left,
    _xaudio_output_speaker_back_right
};

struct s_xaudio_source_state {
    unsigned short flags;
    unsigned short status;
    unsigned long mask;
    long effect;
    long samples_per_second;
    float pitch;
    float gain;
    s_sound_transmission_transform transform;
    union { long c; real_decibel volume; };
    union { long g; real_decibel unspatialized_gain; };
    union { long j; real_decibel spatialized_gain; };
    union { long e; real_decibel speaker_gain; };
    union { long f; real_decibel reverb; };
    union { long h; real_decibel left_pan; };
    union { long i; real_decibel right_pan; };
    long channel;
    real_decibel_volumes outputs;
};
typedef char xaudio_source_state_size[(sizeof(s_xaudio_source_state) == 0x58) ? 1 : -1];

#endif // SOUND_SOUND_XAUDIO_XENON_SOURCE_STATE_H
