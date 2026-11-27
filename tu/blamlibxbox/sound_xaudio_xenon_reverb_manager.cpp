// blamlibxbox/sound_xaudio_xenon_reverb_manager.obj: from the records 0x139a40 0x139d38 0x288660 0x288690 0x288730
// @flags /TP /O2 /Oc
// @address 0x139a40 ?update_reverb@c_xenon_audio_reverb_manager@@AAAXJPBUs_platform_sound_listener_reverb@@@Z
// @address 0x139d38 ?reverb_calculate_speaker_volume@@YAMW4e_reverb_speaker@@PBUs_sound_azimuth_image@@@Z
#include "math/real_math_types.h"
#include "sound/sound_manager_types.h"
#include "memory/static_arrays.h"
#include "sound/sound_xaudio_xenon_dsp_manager_types.h"
#include "sound/sound_xaudio_xenon_reverb_manager.h"
#include "sound/sound_xaudio_reverb_voice_types.h"
#include "sound/sound_xaudio_effects.h"
#include <xtl.h>
#include <xaudio.h>
#include <string.h>
#include "math/real_math.h"
#include "sound/xenon/dsp/xaudio_output_ring_buffer.h"

// @rdata 0x82033310..0x82033380
static const real_bounds k_reverb_front_left_azimuth_bounds[3] = {
    { -0.785398185f, 2.3561945f }, { 5.49778748f, 6.28318548f }, { -6.28318548f, -3.92699099f }
};
static const real_bounds k_reverb_front_right_azimuth_bounds[3] = {
    { -2.3561945f, 0.785398185f }, { 3.92699099f, 6.28318548f }, { -6.28318548f, -5.49778748f }
};
static const real_bounds k_reverb_rear_left_azimuth_bounds[3] = {
    { 0.785398185f, 3.92699099f }, { -5.49778748f, -2.3561945f }, { 0.0f, 0.0f }
};
static const real_bounds k_reverb_rear_right_azimuth_bounds[3] = {
    { 2.3561945f, 5.49778748f }, { -3.92699099f, -0.785398185f }, { 0.0f, 0.0f }
};
static const real_bounds *const k_reverb_speaker_azimuth_bounds[k_reverb_speaker_count] = {
    k_reverb_front_left_azimuth_bounds,
    k_reverb_front_right_azimuth_bounds,
    k_reverb_rear_left_azimuth_bounds,
    k_reverb_rear_right_azimuth_bounds
};
// @rdata-end

static __forceinline float azimuth_intersection(
    float a_lower, float a_upper, float b_lower, float b_upper)
{
    float intersection = (a_upper > b_upper ? b_upper : a_upper) - (a_lower > b_lower ? a_lower : b_lower);
    return intersection > 0.0f ? intersection : 0.0f;
}


void c_xenon_audio_reverb_manager::update_reverb(long index, const s_platform_sound_listener_reverb *reverb)
{
    c_xenon_audio_reverb_manager *manager = &g_xaudio_reverb_manager;
    XAUDIOFXPARAM block;
    bool changed = reverb->environment &&
        (manager->dirty[index] || memcmp(&manager->environments[index], reverb->environment, sizeof(sound_environment)) != 0);
    if (changed) {
        const sound_environment *environment = reverb->environment;
        XAUDIOREVERBI3DL2SETTINGS params;
        params.lRoom = (long)(environment->room * 100.0f);
        params.lRoomHF = (long)(environment->room_hf * 100.0f);
        params.flRoomRolloffFactor = environment->room_rolloff;
        params.flDecayTime = environment->decay_time;
        params.flDecayHFRatio = environment->decay_hf_ratio;
        params.lReflections = (long)(environment->reflections * 100.0f);
        params.flReflectionsDelay = environment->reflections_delay;
        params.lReverb = (long)(environment->reverb * 100.0f);
        params.flReverbDelay = environment->reverb_delay;
        params.flDiffusion = 100.0f * environment->diffusion;
        params.flDensity = 100.0f * environment->density;
        params.flHFReference = environment->hf_reference;
        block.Data.pBuffer = &params; block.Data.BufferSize = sizeof(params);
        XAudioVoice_SetEffectParam((IXAudioVoice *)manager->voices[index], 0, 1, 3, &block);
        memcpy(&manager->environments[index], reverb->environment, sizeof(sound_environment));
    }
    {
        XAUDIOCHANNELVOLUMEENTRY channels[5];
        channels[0].EntryIndex = 0;
        channels[0].Volume = reverb->wet_gain * reverb_calculate_speaker_volume(_reverb_speaker_front_left, &reverb->azimuth_image);
        channels[1].EntryIndex = 1;
        channels[1].Volume = reverb->wet_gain * reverb_calculate_speaker_volume(_reverb_speaker_front_right, &reverb->azimuth_image);
        channels[2].EntryIndex = 4;
        channels[2].Volume = reverb->wet_gain * reverb_calculate_speaker_volume(_reverb_speaker_rear_left, &reverb->azimuth_image);
        channels[3].EntryIndex = 5;
        channels[3].Volume = reverb->wet_gain * reverb_calculate_speaker_volume(_reverb_speaker_rear_right, &reverb->azimuth_image);
        channels[4].EntryIndex = 3;
        channels[4].Volume = reverb->wet_gain;
        XAUDIOCHANNELVOLUME destination = { 5, channels };
        XAUDIOVOICEOUTPUTVOLUMEENTRY output = { 0, &destination };
        XAUDIOVOICEOUTPUTVOLUME output_volume = { 1, &output };
        XAudioVoice_SetVoiceOutputVolume((IXAudioVoice *)manager->voices[index], &output_volume);
    }
    manager->wet[index] = reverb->wet_gain;
}

float reverb_calculate_speaker_volume(e_reverb_speaker speaker, const s_sound_azimuth_image *image)
{
    float volume = azimuth_intersection(
        image->azimuth.lower, image->azimuth.upper, k_reverb_speaker_azimuth_bounds[speaker][0].lower, k_reverb_speaker_azimuth_bounds[speaker][0].upper)
        + azimuth_intersection(
            image->azimuth.lower, image->azimuth.upper, k_reverb_speaker_azimuth_bounds[speaker][1].lower, k_reverb_speaker_azimuth_bounds[speaker][1].upper)
        + azimuth_intersection(
            image->azimuth.lower, image->azimuth.upper, k_reverb_speaker_azimuth_bounds[speaker][2].lower, k_reverb_speaker_azimuth_bounds[speaker][2].upper);
    volume /= k_pi;
    volume = interpolate_linear(image->base_volume, volume, image->spread);
    return volume;
}

// @address 0x6c0738 XAudioCreateSubmixVoice

// @address 0x288660 reverb_effect_get_size
extern "C" HRESULT reverb_effect_get_size(const XAUDIOFXINIT *init, DWORD *effect_size)
{
    HRESULT result = S_OK;
    if (init == NULL)
        result = E_INVALIDARG;
    else if (effect_size == NULL)
        result = E_INVALIDARG;
    else
        *effect_size = sizeof(c_xaudio_reverb_voice_effect);
    return result;
}

// @address 0x288690 reverb_effect_create
extern "C" HRESULT reverb_effect_create(LPCXAUDIOFXINIT init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    HRESULT result = S_OK;
    if (init == NULL)
        result = E_INVALIDARG;
    else if (allocator == NULL)
        result = E_INVALIDARG;
    else if (effect == NULL)
        result = E_INVALIDARG;
    else {
        c_xaudio_reverb_voice_effect *object = new(allocator) c_xaudio_reverb_voice_effect;
        object->Initialize(init, allocator);
        *effect = object;
    }
    return result;
}

// @address 0x288730 ?initialize@c_xenon_audio_reverb_manager@@QAAXPAVc_xenon_audio_dsp_manager@@@Z
void c_xenon_audio_reverb_manager::initialize(c_xenon_audio_dsp_manager *dsp_manager)
{
    // match: 0x288730: value range assumption guides branch lowering
    __assume(this == &g_xaudio_reverb_manager);
    XAUDIOFXINIT reverb_effect = { 3, 0 };
    LPCXAUDIOFXINIT effects[] = { &reverb_effect };
    XAUDIOVOICEFXCHAIN effect_chain = { 1, effects };
    XAUDIOCHANNELMAPENTRY channel_entries[] = {
        { 0, 0, 1.0f },
        { 1, 1, 1.0f },
        { 2, 2, 0.0f },
        { 3, 3, 1.0f },
        { 4, 4, 1.0f },
        { 5, 5, 1.0f }
    };
    XAUDIOVOICEOUTPUTENTRY output_entry;
    XAUDIOCHANNELMAP channel_map;
    XAUDIOVOICEOUTPUT voice_output = { 1, &output_entry };
    output_entry.pDestVoice = g_xaudio_dsp_manager.m_remote_voice_output_destination;
    output_entry.pChannelMap = &channel_map;
    channel_map.EntryCount = 6;
    channel_map.paEntries = channel_entries;
    XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
    voice_init.pEffectChain = &effect_chain;
    voice_init.MaxChannelMapEntryCount = 6;
    voice_init.Format.SampleType = 0;
    voice_init.Format.SampleRate = 48000;
    voice_init.Format.ChannelCount = 1;
    voice_init.pVoiceOutput = &voice_output;
    voice_init.SubmixStage = 2;
    for (long index = 0; index < voices.get_count(); ++index) {
        XAudioCreateSubmixVoice(&voice_init, &voices[index]);
    }
    dirty.set_all(true);
}
