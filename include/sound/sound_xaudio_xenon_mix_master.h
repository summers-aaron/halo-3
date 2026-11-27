#ifndef SOUND_SOUND_XAUDIO_XENON_MIX_MASTER_H
#define SOUND_SOUND_XAUDIO_XENON_MIX_MASTER_H

#include "sound/sound_xaudio_xenon.h"
#include "sound/sound_xaudio_xenon_mix_master_globals.h"
#include "cseries/cseries.h"
#include <math.h>
#include <string.h>

struct s_platform_sound_source_properties;
struct s_platform_sound_playback_submit_parameters;
struct s_xaudio_platform_output;
class c_xenon_audio_source_manager;
class c_xenon_audio_voice_manager;

class c_sound_xaudio_xenon_mix_master {
    friend bool ::sound_xaudio_xenon_run_source_update_loop(void);
public:
    void set_global_mixes(const s_sound_surround_mix_definition *first_person_mix,
        const s_sound_surround_mix_definition *ambient_surround_mix, const s_sound_global_mix_configuration *global_mix,
        bool surround_enabled);
    // match: 0x1a55c0: noalias keeps fr31 and gestalt live across the call; without it the caller spills fr31 (stfs/tlfs) and reloads gestalt (lwz).
    __declspec(noalias) void source_update_properties(long source, const s_platform_sound_source_properties *properties,
        const s_platform_sound_playback_submit_parameters *submit_parameters, bool force);
    s_xaudio_source_state *get_mutable_source_state(long source);
    const s_xaudio_source_state *get_source_state(long source) const;
private:
    void update_source_hardware_output(c_xenon_audio_source_manager *source_manager,
        c_xenon_audio_voice_manager *voice_manager, long source);
    void source_get_output(long source, long channel_count, s_xaudio_platform_output *output);

    s_xaudio_source_state states[196];
    s_xaudio_speaker_cross_mix cross_mix;
    s_sound_surround_mix_definition surround[2];
    s_sound_global_mix_configuration global_mix;
    bool surround_enabled;
};

__forceinline void c_sound_xaudio_xenon_mix_master::set_global_mixes(const s_sound_surround_mix_definition *first_person_mix,
    const s_sound_surround_mix_definition *ambient_surround_mix, const s_sound_global_mix_configuration *global_mix,
    bool surround_enabled)
{
    surround[0] = *first_person_mix;
    surround[1] = *ambient_surround_mix;
    memcpy(&this->global_mix, global_mix, sizeof(*global_mix));
    float blend = PIN(global_mix->blend, 0.0f, 1.0f);
    this->surround_enabled = surround_enabled;
    cross_mix.direct = sqrtf(1.0f - blend);
    cross_mix.cross = sqrtf(blend);
}

// Defined in tu/blamlibxbox/sound_xaudio_xenon.cpp.
extern c_sound_xaudio_xenon_mix_master g_xaudio_mix_master;

#endif // SOUND_SOUND_XAUDIO_XENON_MIX_MASTER_H
