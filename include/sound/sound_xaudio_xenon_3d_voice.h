#ifndef SOUND_SOUND_XAUDIO_XENON_3D_VOICE_H
#define SOUND_SOUND_XAUDIO_XENON_3D_VOICE_H

// One speaker's 3D state of a voice; the methods are defined in tu/blamlibxbox/sound_xaudio_xenon_3d_voice.cpp.

#include "memory/static_arrays.h"

typedef unsigned char X3DAUDIO_HANDLE[20];
struct X3DAUDIO_LISTENER;
struct s_platform_sound_voice_update_definition;
struct s_xaudio_source_state;
struct s_xaudio_speaker_cross_mix;
struct s_xaudio_platform_output;

class c_xaudio_voice_speaker_datum
{
public:
    void calculate(const X3DAUDIO_HANDLE &handle, long channel_count, const X3DAUDIO_LISTENER *listener,
        const s_platform_sound_voice_update_definition *update, float unused);

    void get_platform_output(long channel_count, const s_xaudio_source_state *source_state,
        const s_xaudio_speaker_cross_mix *cross_mix, s_xaudio_platform_output *out_platform_output);

    c_static_array<float, 12> m_matrix;
    float m_doppler;
    float m_reverb_level;
    float m_distance;
    float m_direct_lpf;
    float m_reverb_lpf;
};
typedef char xaudio_voice_speaker_datum_size[(sizeof(c_xaudio_voice_speaker_datum) == 0x44) ? 1 : -1];

#endif // SOUND_SOUND_XAUDIO_XENON_3D_VOICE_H
