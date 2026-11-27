#ifndef SOUND_SOUND_XAUDIO_XENON_DSP_MANAGER_TYPES_H
#define SOUND_SOUND_XAUDIO_XENON_DSP_MANAGER_TYPES_H

#include <xtl.h>
#include <xaudio.h>
#include "sound/xenon/dsp/xaudio_output_ring_buffer.h"

// DSP storage is the accepted sound_xaudio_xenon.cpp layout. Its two untyped
// ranges stay explicitly unresolved; the source-manager user never reads them.
struct s_xaudio_obstruction_voices
{
    IXAudioSubmixVoice *voices[6];
};

enum e_xaudio_dsp_effect
{
    _xaudio_dsp_effect_headset_mixbin = 0,
    _xaudio_dsp_effect_master_filter,
    _xaudio_dsp_effect_occlusion,
    _xaudio_dsp_effect_obstruction,
    _xaudio_dsp_effect_environment,
    _xaudio_dsp_effect_radio,

    k_xaudio_dsp_effect_count
};

class c_xenon_audio_dsp_manager
{
public:
    IXAudioSubmixVoice *create_remote_voice_output(long ring_index);
    // March 82d222f0 returns no chain without reading this or channel_count.
    __forceinline const XAUDIOVOICEFXCHAIN *get_per_source_effect_chain(long channel_count)
    {
        return 0;
    }

    XAUDIOFXINIT m_effects[k_xaudio_dsp_effect_count];
    LPCXAUDIOFXINIT m_effect_pointers[k_xaudio_dsp_effect_count];
    XAUDIOVOICEFXCHAIN m_effect_chain;
    XAUDIOFXINIT m_output_effect;
    LPCXAUDIOFXINIT m_output_effect_pointer;
    XAUDIOVOICEFXCHAIN m_output_effect_chain;
    XAUDIOFXINIT m_remote_talker_effect;
    LPCXAUDIOFXINIT m_remote_talker_effect_pointer;
    XAUDIOVOICEFXCHAIN m_remote_talker_effect_chain;
    XAUDIOFXID m_ring_buffer_effect_id;
    char pad79[0x90 - 0x79];
    IXAudioSubmixVoice *m_source_voice;
    IXAudioSubmixVoice *m_remote_voice_output_destination;
    IXAudioSubmixVoice *m_occlusion_voice;
    IXAudioSubmixVoice *m_auxiliary_voice;
    IXAudioSubmixVoice *m_environment_voice;
    IXAudioSubmixVoice *m_radio_voice;
    char padA8[0xc0 - 0xa8];
    s_xaudio_obstruction_voices m_obstruction_voices;
    c_xaudio_output_ring_buffer m_headset_ring_buffer;
    c_xaudio_output_ring_buffer m_ring_buffer;
};

#endif // SOUND_SOUND_XAUDIO_XENON_DSP_MANAGER_TYPES_H
