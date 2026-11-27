#ifndef SOUND_SOUND_XAUDIO_XENON_REVERB_MANAGER_H
#define SOUND_SOUND_XAUDIO_XENON_REVERB_MANAGER_H

#include "sound/platform_sound_listener_reverb_types.h"
#include "memory/static_arrays.h"
#include <xtl.h>
#include <xaudio.h>

enum e_reverb_speaker
{
    _reverb_speaker_front_left,
    _reverb_speaker_front_right,
    _reverb_speaker_rear_left,
    _reverb_speaker_rear_right,
    k_reverb_speaker_count
};

class c_xenon_audio_reverb_manager {
public:
    friend void platform_sound_listener_set_properties(const struct platform_sound_listener_properties *properties);
private:
    void update_reverb(long index, const s_platform_sound_listener_reverb *reverb);
public:
    void initialize(class c_xenon_audio_dsp_manager *dsp_manager);
    __forceinline void dispose()
    {
        for (long voice_index = 0; voice_index < voices.get_count(); voice_index++)
            voices[voice_index]->Release();
    }
    c_static_array<IXAudioSubmixVoice *, 2> voices;
    sound_environment environments[2];
    float wet[2];
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    unsigned int : 32;
    c_static_array<bool, 2> dirty;
};

extern c_xenon_audio_reverb_manager g_xaudio_reverb_manager;
extern class c_xenon_audio_dsp_manager g_xaudio_dsp_manager;
float reverb_calculate_speaker_volume(e_reverb_speaker speaker, const s_sound_azimuth_image *image);
#endif
