// blamlibxbox/sound_xaudio_xenon_mix_master.obj: from the records 0x288c88 0x288e88 0x289a10 0x289b90 0x289c58 0x289d20
// @compiler xdk6130
// @flags /TP /O2 /Oc
#define NOD3D

#include "sound/sound_xaudio_xenon_source_manager_types.h"
#include "sound/sound_manager_types.h"
#include "memory/static_arrays.h"
#include "cseries/cseries.h"
#include <math.h>
#include <ppcintrinsics.h>
#include <xaudio.h>
#include "sound/sound_xaudio_xenon.h"
#include "sound/game_sound_definitions.h"
#include "sound/sound_xaudio_xenon_3d_voice.h"
#include "sound/sound_xaudio_xenon_source_state.h"
#include "sound/sound_definition_permutation.h"
#include "sound/sound_manager_runtime_types.h"
#include "sound/sound_xaudio_xenon_output_types.h"
#include <xutility>
#include "sound/sound_xaudio_xenon_3d_voice_types.h"
#include "sound/sound_xaudio_xenon_voice_manager.h"
#include "sound/sound_manager.h"
#include "sound/sound_xaudio_xenon_mix_master.h"


class c_sound_speaker_mixer
 {
    c_static_array<float, 6> m_speaker_volumes;
    c_static_array<c_static_array<float, 2>, 2> m_stereo_volumes;
public:
    __forceinline c_sound_speaker_mixer() {
        m_speaker_volumes.set_all(1.0f);
        m_stereo_volumes[0][0] = 1.0f;
        m_stereo_volumes[0][1] = 0.0f;
        m_stereo_volumes[1][0] = 0.0f;
        m_stereo_volumes[1][1] = 1.0f;
    }
    __forceinline void attenuate_all_speakers(float volume) {
        e_xaudio_output_speaker speakers[] = {_xaudio_output_speaker_front_left, _xaudio_output_speaker_front_right,
            _xaudio_output_speaker_front_center, _xaudio_output_speaker_low_frequency, _xaudio_output_speaker_back_left,
            _xaudio_output_speaker_back_right};
        update_volumes(speakers, volume);
    }
    __forceinline void attenuate_front_speakers(float volume) {
        e_xaudio_output_speaker speakers[] = { _xaudio_output_speaker_front_left, _xaudio_output_speaker_front_right };
        update_volumes(speakers, volume);
    }
    __forceinline void attenuate_rear_speakers(float volume) {
        e_xaudio_output_speaker speakers[] = { _xaudio_output_speaker_back_left, _xaudio_output_speaker_back_right };
        update_volumes(speakers, volume);
    }
    __forceinline void attenuate_center_speakers(float volume) {
        e_xaudio_output_speaker speakers[] = { _xaudio_output_speaker_front_center, _xaudio_output_speaker_low_frequency };
        update_volumes(speakers, volume);
    }
    __forceinline void attenuate_lfe_channel(float volume) {
        e_xaudio_output_speaker speakers[] = { _xaudio_output_speaker_low_frequency };
        update_volumes(speakers, volume);
    }
    __forceinline void set_stereo_pan(float left, float right) {
        e_xaudio_output_speaker left_speakers[] = { _xaudio_output_speaker_front_right, _xaudio_output_speaker_back_right };
        update_volumes(left_speakers, left);
        e_xaudio_output_speaker right_speakers[] = { _xaudio_output_speaker_front_left, _xaudio_output_speaker_back_left };
        update_volumes(right_speakers, right);
    }
    __forceinline void set_left_stereo_pan(float left, float right) {
        m_stereo_volumes[0][0] = left;
        m_stereo_volumes[0][1] = right;
    }
    __forceinline void set_right_stereo_pan(float left, float right) {
        m_stereo_volumes[1][0] = left;
        m_stereo_volumes[1][1] = right;
    }
    __forceinline void set_equal_power_stereo_pan(float left, float right);
    void set_speaker_volumes(long channel_count, s_xaudio_platform_output *output);
private:
    void update_volumes(c_wrapped_array<e_xaudio_output_speaker> speakers, float volume);
    void combine_speaker_output(s_xaudio_platform_output *output, unsigned long speaker_offset, unsigned long speaker_stride, float volume_a, float volume_b);
};
// @address 0x332c0 k_mono_spatial_attenuation
// @rdata 0x820332c0..0x820332c4
const real_decibel k_mono_spatial_attenuation = { 0.0f };
// @rdata-end


// @address 0xf9528 ?real_gain_to_decibels@@YA?AUreal_decibel@@M@Z
__forceinline float sound_xaudio_real_decibels_to_gain(real_decibel decibels) {
    return (float)exp(decibels.value * 0.115129255f);
}
__forceinline void sound_xaudio_set_flag(unsigned short *flags, unsigned short bit, bool value) {
    if (value)
        *flags |= bit;
    else
        *flags &= ~bit;
}
__forceinline bool sound_xaudio_test_flag(unsigned long flags, long bit) {
    bool result = (flags >> bit) & 1;
    return result;
}


template<typename t_derived, typename t_element>
__forceinline void c_array_operator_interface<t_derived, t_element>::set_all(const t_element &value) {
    std::fill(begin(), end(), value);
}

s_xaudio_source_state *c_sound_xaudio_xenon_mix_master::get_mutable_source_state(long source) {
    return &g_xaudio_mix_master.states[source];
}
const s_xaudio_source_state *c_sound_xaudio_xenon_mix_master::get_source_state(long source) const {
    return &g_xaudio_mix_master.states[source];
}
// @address 0x288c88 ?source_update_properties@c_sound_xaudio_xenon_mix_master@@QAAXJPBUs_platform_sound_source_properties@@PBUs_platform_sound_playback_submit_parameters@@_N@Z
void c_sound_xaudio_xenon_mix_master::source_update_properties(long source, const s_platform_sound_source_properties *properties,
    const s_platform_sound_playback_submit_parameters *submit_parameters, bool force)
{
    s_xaudio_source_state *state = get_mutable_source_state(source);
    state->flags = (unsigned short)properties->flags.get_unsafe();
    state->status = 0;
    sound_xaudio_set_flag(&state->status, 1, (unsigned short)properties->hardware_voice_index != 0xffff);
    state->effect = properties->effect;
    state->volume = properties->gain_db;
    state->speaker_gain = properties->reverb_gain;
    state->reverb = properties->gain;
    state->unspatialized_gain = properties->distance;
    state->spatialized_gain = properties->output_gain;
    state->left_pan = properties->lower;
    state->right_pan = properties->upper;
    state->samples_per_second = properties->samples_per_second;
    state->pitch = properties->pitch;
    state->gain = properties->cone_attenuation;
    state->transform = properties->transform;
    state->outputs = properties->output_gains;
    if (submit_parameters->output != -1) {
        state->mask = 1 << submit_parameters->output;
        state->flags |= 512;
        state->g = 0xc2800000;
        state->f = 0xc2800000;
        state->e = 0xc2800000;
    } else {
        state->mask = 0;
    }
    sound_xaudio_set_flag(&state->flags, 2048, submit_parameters->special);
    state->channel = submit_parameters->channel;
    if (state->channel == -1 && (bool)((state->flags >> 11) & 1))
        state->gain = 1.0f;
    if (sound_xaudio_test_flag(properties->flags.get_unsafe(), 5)) {
        float total = 0.0f;
        for (long i = 0; i < 4; i++) {
            float gain = sound_xaudio_real_decibels_to_gain(properties->output_gains.v[i]);
            total += gain * gain;
        }
        state->unspatialized_gain = real_gain_to_decibels(sqrtf(total));
    }
}
void c_sound_speaker_mixer::set_speaker_volumes(long channel_count, s_xaudio_platform_output *output) {
    output->resize(channel_count * 6);
    output->set_all(0.0f);
    switch (channel_count) {
    case 1: {
        const float one = 1.0f;
        combine_speaker_output(output, 0, channel_count, one, one);
        break;
    }
    case 2:
        combine_speaker_output(output, 0, channel_count, m_stereo_volumes[0][0], m_stereo_volumes[0][1]);
        combine_speaker_output(output, 1, channel_count, m_stereo_volumes[1][0], m_stereo_volumes[1][1]);
        break;
    case 4: {
        const float one = 1.0f;
        output->get_channel_speaker(0, channel_count, _xaudio_output_speaker_front_right) = one;
        output->get_channel_speaker(1, channel_count, _xaudio_output_speaker_front_left) = one;
        output->get_channel_speaker(2, channel_count, _xaudio_output_speaker_back_right) = one;
        output->get_channel_speaker(3, channel_count, _xaudio_output_speaker_back_left) = one;
        break;
    }
    case 6: {
        const float one = 1.0f;
        output->get_channel_speaker(0, channel_count, _xaudio_output_speaker_front_right) = one;
        output->get_channel_speaker(1, channel_count, _xaudio_output_speaker_front_left) = one;
        output->get_channel_speaker(2, channel_count, _xaudio_output_speaker_back_right) = one;
        output->get_channel_speaker(3, channel_count, _xaudio_output_speaker_back_left) = one;
        output->get_channel_speaker(4, channel_count, _xaudio_output_speaker_front_center) = one;
        output->get_channel_speaker(5, channel_count, _xaudio_output_speaker_low_frequency) = one;
        break;
    }
    default:
        // match: 0x289d20: channel counts are restricted to the supported cases
        __assume(0);
    }
}
inline void c_sound_speaker_mixer::update_volumes(c_wrapped_array<e_xaudio_output_speaker> speakers, float volume) {
    long speaker_index = 0;
    long speaker_count = speakers.count();
    for (; speaker_index < speaker_count; ++speaker_index) {
        m_speaker_volumes[speakers[speaker_index]] *= volume;
    }
}
inline void c_sound_speaker_mixer::combine_speaker_output(
    s_xaudio_platform_output *output, unsigned long speaker_offset, unsigned long speaker_stride, float volume_a, float volume_b)
{
    for (long speaker_index = 0; speaker_index < 6; ++speaker_index)
        (*output)[speaker_offset + speaker_index * speaker_stride] = m_speaker_volumes[speaker_index];
    (*output)[speaker_offset + 1 * speaker_stride] *= volume_a;
    (*output)[speaker_offset + 5 * speaker_stride] *= volume_a;
    (*output)[speaker_offset + 2 * speaker_stride] *= volume_a;
    (*output)[speaker_offset + 0 * speaker_stride] *= volume_b;
    (*output)[speaker_offset + 4 * speaker_stride] *= volume_b;
    (*output)[speaker_offset + 3 * speaker_stride] *= volume_b;
}

// match: 0x289d20: float_control preserves the stereo-pan helper's single-precision rounding
#pragma float_control(precise, on, push)
// match: 0x289d20: fenv_access preserves the stereo-pan helper's call-site expansion
#pragma fenv_access(on)
static __forceinline float sound_xaudio_xenon_mix_master_complement_pan(float gain, float zero, float one) {
    float square = gain * gain;
    return __fsqrts(PIN(one - square, zero, one));
}
// match: 0x289d20: fenv_access preserves the stereo-pan helper's call-site expansion
#pragma fenv_access(off)
// match: 0x289d20: float_control preserves the stereo-pan helper's single-precision rounding
#pragma float_control(pop)
__forceinline void c_sound_speaker_mixer::set_equal_power_stereo_pan(float left, float right) {
    m_stereo_volumes[0][1] = sound_xaudio_xenon_mix_master_complement_pan(left, 0.0f, 1.0f);
    m_stereo_volumes[0][0] = left;
    set_right_stereo_pan(sound_xaudio_xenon_mix_master_complement_pan(right, 0.0f, 1.0f), right);
}

static __forceinline float sound_xaudio_xenon_mix_master_gain(real_decibel decibels) {
    return exp(decibels.to_real() * 0.115129255f);
}

static __forceinline float sound_xaudio_xenon_mix_master_hardware_gain(real_decibel decibels) {
    return exp(decibels.to_real() * 0.115129255f);
}

static __forceinline void sound_xaudio_xenon_mix_master_initialize_channel_volume(XAUDIOCHANNELVOLUME *volume,
    unsigned char count, XAUDIOCHANNELVOLUMEENTRY *entries) {
    volume->EntryCount = count;
    volume->paEntries = entries;
}

static __forceinline void sound_xaudio_xenon_mix_master_mono_speakers(c_wrapped_array<float> speakers) {
    speakers[_xaudio_output_speaker_front_right] = 0.5f;
    speakers[_xaudio_output_speaker_front_left] = 0.5f;
    speakers[_xaudio_output_speaker_front_center] = 0.0f;
    speakers[_xaudio_output_speaker_low_frequency] = 0.0f;
    speakers[_xaudio_output_speaker_back_right] = 0.0f;
    speakers[_xaudio_output_speaker_back_left] = 0.0f;
}

// @address 0x288e88
void c_sound_xaudio_xenon_mix_master::update_source_hardware_output(
    c_xenon_audio_source_manager *source_manager, c_xenon_audio_voice_manager *voice_manager, long source)
{
    // match: 0x288e88: value range assumption guides branch lowering
    __assume(this == &g_xaudio_mix_master);
    // match: 0x288e88: value range assumption guides branch lowering
    __assume(source_manager == &g_xaudio_source_manager);
    // match: 0x288e88: value range assumption guides branch lowering
    __assume(voice_manager == &g_xaudio_voice_manager);
    c_platform_source_datum *source_datum = source_manager->get_source(source);
    // match: 0x288e88: test playing directly; a named predicate shifts the unrolled sample-field boundary.
    if (source_datum->is_playing())
    {
        long channel_count = source_datum->get_sound_channel_count();
        const s_xaudio_source_state *state = get_source_state(source);
        float sample_rate_pitch = XAudioSampleRateToPitch(44100, state->samples_per_second);
        sample_rate_pitch = -sample_rate_pitch;
        s_xaudio_platform_output final_output;
        final_output.attenuation_loss = 1.0f;
        float unspatialized_gain = sound_xaudio_xenon_mix_master_hardware_gain(state->unspatialized_gain);
        float spatialized_gain = 0.0f;
        long output_mode;
        // match: 0x288e88: keep this predicate named so the third spatial sample ends its symbol chunk.
        if (bool special_output = sound_xaudio_test_flag(state->flags, 11))
            output_mode = state->channel == -1 ? 3 : 1;
        else
            output_mode = state->channel == -1 ? 0 : 2;
        float mono_spatialized_gain = 0.0f;
        // match: 0x288e88: shared counters preserve the blend field boundary and scale-loop operand ranks.
        long sample, count;

        // match: 0x288e88: omit the unused predicate local to preserve the sample-field boundary.
        if (sound_xaudio_test_flag(state->flags, 9))
        {
            source_get_output(source, source_datum->get_sound_channel_count(), &final_output);
        }
        // match: 0x288e88: omit the unused predicate local to preserve the sample-field boundary.
        else if (sound_xaudio_test_flag(state->flags, 5))
        {
            final_output.resize(channel_count * 6);
            for (long channel = 0; channel < channel_count; ++channel)
            {
                final_output[channel + channel_count] = sound_xaudio_xenon_mix_master_hardware_gain(state->outputs.v[0]) * unspatialized_gain;
                final_output[channel] = sound_xaudio_xenon_mix_master_hardware_gain(state->outputs.v[1]) * unspatialized_gain;
                final_output[channel + channel_count * 5] = sound_xaudio_xenon_mix_master_hardware_gain(state->outputs.v[2]) * unspatialized_gain;
                final_output[channel + channel_count * 4] = sound_xaudio_xenon_mix_master_hardware_gain(state->outputs.v[3]) * unspatialized_gain;
                final_output[channel + channel_count * 2] = 0.0f;
                final_output[channel + channel_count * 3] = 0.0f;
            }
            final_output.direct_lpf = 0.0f;
            final_output.reverb_lpf = 0.0f;
            final_output.pitch_octaves = state->pitch / 1200.0f;
            final_output.reverb_level = sound_xaudio_xenon_mix_master_hardware_gain(state->reverb) * unspatialized_gain;
        }
        else if (sound_xaudio_test_flag(state->status, 0) && !sound_xaudio_test_flag(state->flags, 7))
        {
            real_decibel attenuation = source_datum->get_sound_channel_count() == 2
                ? g_xaudio_mix_master.global_mix.spatial_attenuation : k_mono_spatial_attenuation;
            spatialized_gain = sound_xaudio_xenon_mix_master_hardware_gain(state->spatialized_gain + attenuation);
            // match: 0x288e88: construct both temporary outputs only in the branch that fills them.
            s_xaudio_platform_output unspatialized_output;
            s_xaudio_platform_output spatial_output;
            source_get_output(source, source_datum->get_sound_channel_count(), &unspatialized_output);
            voice_manager->speakers[source].get_platform_output(
                source_datum->get_sound_channel_count(), state, &g_xaudio_mix_master.cross_mix, &spatial_output);
            final_output.direct_lpf = spatial_output.direct_lpf;
            final_output.reverb_lpf = spatial_output.reverb_lpf;
            final_output.pitch_octaves = unspatialized_output.pitch_octaves + spatial_output.pitch_octaves * spatialized_gain;
            final_output.reverb_level = unspatialized_output.reverb_level * (unspatialized_gain + spatial_output.reverb_level * spatialized_gain);
            final_output.resize(source_datum->get_sound_channel_count() * 6);
            for (sample = 0, count = spatial_output.count(); sample < count; ++sample)
                final_output[sample] = unspatialized_output[sample] * unspatialized_gain + spatial_output[sample] * spatialized_gain;
            // match: 0x288e88: omit the unused predicate local to preserve the sample-field boundary.
            if (sound_xaudio_test_flag(state->flags, 11))
                final_output.attenuation_loss = spatial_output.attenuation_loss;
        }
        else if (sound_xaudio_test_flag(state->flags, 13) && channel_count == 1)
        {
            real_decibel attenuation = source_datum->get_sound_channel_count() == 2
                ? g_xaudio_mix_master.global_mix.spatial_attenuation : k_mono_spatial_attenuation;
            mono_spatialized_gain = sound_xaudio_xenon_mix_master_hardware_gain(attenuation);
            // match: 0x288e88: construct both temporary outputs only in the branch that fills them.
            s_xaudio_platform_output unspatialized_output;
            s_xaudio_platform_output spatial_output;
            source_get_output(source, source_datum->get_sound_channel_count(), &unspatialized_output);
            if (output_mode == 1 || output_mode == 2)
                mono_spatialized_gain = 0.0f;
            spatial_output.resize(6);
            sound_xaudio_xenon_mix_master_mono_speakers(make_wrapped_array(spatial_output.begin(), spatial_output.count()));
            final_output.pitch_octaves = unspatialized_output.pitch_octaves;
            final_output.direct_lpf = 0.0f;
            final_output.reverb_lpf = 0.0f;
            final_output.reverb_level = unspatialized_output.reverb_level * (unspatialized_gain + mono_spatialized_gain);
            final_output.resize(source_datum->get_sound_channel_count() * 6);
            for (sample = 0, count = spatial_output.count(); sample < count; ++sample)
                final_output[sample] = unspatialized_output[sample] * unspatialized_gain + spatial_output[sample] * mono_spatialized_gain;
        }
        else
        {
            source_get_output(source, source_datum->get_sound_channel_count(), &final_output);
            final_output.reverb_level *= unspatialized_gain;
            for (sample = 0, count = final_output.count(); sample < count; ++sample)
            {
                // match: 0x288e88: snapshot each gain before applying the channel scale.
                float &gain = final_output[sample];
                const float &value = +gain;
                gain = value * unspatialized_gain;
            }
        }
        final_output.pitch_octaves += sample_rate_pitch;
        float speaker_gain = sound_xaudio_xenon_mix_master_hardware_gain(state->speaker_gain);
        final_output.attenuation_loss *= sound_xaudio_xenon_mix_master_hardware_gain(state->volume);

        {
            c_static_array<XAUDIOCHANNELVOLUMEENTRY, 36> direct_entries;
            c_static_array<XAUDIOCHANNELVOLUMEENTRY, 36> direct_lpf_entries;
            for (long sample = 0; sample < final_output.count(); ++sample)
            {
                direct_entries[sample].EntryIndex = (unsigned char)sample;
                direct_lpf_entries[sample].EntryIndex = (unsigned char)sample;
                if (output_mode == 3)
                {
                    direct_entries[sample].Volume = final_output.attenuation_loss;
                    direct_lpf_entries[sample].Volume = 0.0f;
                }
                else if (output_mode == 2)
                {
                    direct_entries[sample].Volume = 0.0f;
                    direct_lpf_entries[sample].Volume = 0.0f;
                }
                else
                {
                    direct_entries[sample].Volume = (1.0f - final_output.direct_lpf) * final_output[sample] * speaker_gain;
                    direct_lpf_entries[sample].Volume = final_output[sample] * speaker_gain * final_output.direct_lpf;
                }
            }
            XAUDIOCHANNELVOLUME direct_volume;
            direct_volume.EntryCount = (unsigned char)final_output.count();
            direct_volume.paEntries = direct_entries.get_elements();
            XAUDIOCHANNELVOLUME direct_lpf_volume;
            direct_lpf_volume.EntryCount = (unsigned char)final_output.count();
            direct_lpf_volume.paEntries = direct_lpf_entries.get_elements();
            c_static_array<XAUDIOVOICEOUTPUTVOLUMEENTRY, 7> output_entries;
            output_entries[0].OutputVoiceIndex = 0;
            output_entries[0].pVolume = &direct_volume;
            output_entries[4].OutputVoiceIndex = 4;
            output_entries[4].pVolume = &direct_lpf_volume;

            c_static_array<XAUDIOCHANNELVOLUMEENTRY, 6> reverb_entries;
            c_static_array<XAUDIOCHANNELVOLUMEENTRY, 6> reverb_lpf_entries;
            for (unsigned long channel = 0; channel < 6; ++channel)
            {
                reverb_entries[channel].EntryIndex = (unsigned char)channel;
                reverb_lpf_entries[channel].EntryIndex = (unsigned char)channel;
                if (output_mode != 2 && output_mode != 3)
                {
                    reverb_entries[channel].Volume = (1.0f - final_output.reverb_lpf) * final_output.reverb_level;
                    reverb_lpf_entries[channel].Volume = final_output.reverb_level * final_output.reverb_lpf;
                }
                else
                {
                    reverb_entries[channel].Volume = 0.0f;
                    reverb_lpf_entries[channel].Volume = 0.0f;
                }
            }
            XAUDIOCHANNELVOLUME reverb_volume;
            reverb_volume.EntryCount = (unsigned char)source_datum->get_sound_channel_count();
            reverb_volume.paEntries = reverb_entries.get_elements();
            XAUDIOCHANNELVOLUME reverb_lpf_volume;
            reverb_lpf_volume.EntryCount = (unsigned char)source_datum->get_sound_channel_count();
            reverb_lpf_volume.paEntries = reverb_lpf_entries.get_elements();
            output_entries[1].OutputVoiceIndex = 1;
            output_entries[1].pVolume = &reverb_volume;
            output_entries[2].OutputVoiceIndex = 2;
            output_entries[2].pVolume = &reverb_volume;
            output_entries[5].OutputVoiceIndex = 5;
            output_entries[5].pVolume = &reverb_lpf_volume;

            c_static_array<XAUDIOCHANNELVOLUMEENTRY, 24> auxiliary_entries;
            for (long channel = 0; channel < channel_count; ++channel)
            {
                auxiliary_entries[channel].EntryIndex = (unsigned char)channel;
                auxiliary_entries[channel].Volume = output_mode != 2 ? (TEST_BIT(state->mask, 0) ? 1.0f : 0.0f) : 0.0f;
                auxiliary_entries[channel + channel_count].EntryIndex = (unsigned char)(channel + channel_count);
                auxiliary_entries[channel + channel_count].Volume = output_mode != 2 ? (TEST_BIT(state->mask, 1) ? 1.0f : 0.0f) : 0.0f;
                auxiliary_entries[channel + channel_count * 2].EntryIndex = (unsigned char)(channel + channel_count * 2);
                auxiliary_entries[channel + channel_count * 2].Volume = output_mode != 2 ? (TEST_BIT(state->mask, 2) ? 1.0f : 0.0f) : 0.0f;
                auxiliary_entries[channel + channel_count * 3].EntryIndex = (unsigned char)(channel + channel_count * 3);
                auxiliary_entries[channel + channel_count * 3].Volume = output_mode != 2 ? (TEST_BIT(state->mask, 3) ? 1.0f : 0.0f) : 0.0f;
            }
            XAUDIOCHANNELVOLUME auxiliary_volume;
            auxiliary_volume.EntryCount = (unsigned char)(channel_count * 4);
            auxiliary_volume.paEntries = auxiliary_entries.get_elements();
            output_entries[3].OutputVoiceIndex = 3;
            output_entries[3].pVolume = &auxiliary_volume;

            c_static_array<XAUDIOCHANNELVOLUMEENTRY, 6> attenuation_entries;
            long attenuation_channel = 0;
            if (attenuation_channel < channel_count)
            {
                do
                {
                    // match: 0x288e88: inline both single-use locals to preserve the sample-field boundary.
                    attenuation_entries[attenuation_channel].EntryIndex = (unsigned char)attenuation_channel;
                    attenuation_entries[attenuation_channel].Volume = state->channel == 0 ? final_output.attenuation_loss : 0.0f;
                } while (++attenuation_channel < channel_count);
            }
            XAUDIOCHANNELVOLUME attenuation_volume;
            sound_xaudio_xenon_mix_master_initialize_channel_volume(&attenuation_volume,
                                                                    (unsigned char)channel_count, attenuation_entries.get_elements());
            output_entries[6].OutputVoiceIndex = 6;
            output_entries[6].pVolume = &attenuation_volume;
            XAUDIOVOICEOUTPUTVOLUME output_volume;
            output_volume.EntryCount = (unsigned char)output_entries.get_count();
            output_volume.paEntries = output_entries.get_elements();
            if (g_sound_manager_globals->bink_playing && sound_xaudio_test_flag(state->flags, 12))
            {
                for (long output = 0; output < output_volume.EntryCount; ++output)
                {
                    XAUDIOCHANNELVOLUME *volume = output_volume.paEntries[output].pVolume;
                    for (long channel = 0; channel < volume->EntryCount; ++channel)
                        volume->paEntries[channel].Volume = 0.0f;
                }
            }
            source_datum->voice->SetVolume(sound_xaudio_xenon_mix_master_hardware_gain(state->volume));
            source_datum->voice->SetVoiceOutputVolume(&output_volume);
            source_datum->voice->SetPitch(PIN(final_output.pitch_octaves, -12.0f, 5.0f));
        }
    }
}

// @address 0x289d20
void c_sound_xaudio_xenon_mix_master::source_get_output(long source, long channel_count, s_xaudio_platform_output *output) {
    const c_sound_xaudio_xenon_mix_master *master = &g_xaudio_mix_master;
    const s_xaudio_source_state *state = &master->states[source];
    c_sound_speaker_mixer mixer;
    switch (state->effect) {
    case 1:
        mixer.attenuate_center_speakers(0.0f);
        mixer.attenuate_rear_speakers(0.0f);
        break;
    case 2:
        mixer.attenuate_center_speakers(0.0f);
        if (master->surround_enabled) {
            mixer.attenuate_front_speakers(0.0f);
        } else {
            mixer.attenuate_front_speakers(sound_xaudio_xenon_mix_master_gain(master->global_mix.front_attenuation));
            mixer.attenuate_rear_speakers(0.0f);
        }
        break;
    case 3:
        if (master->surround_enabled) {
            mixer.attenuate_front_speakers(0.0f);
            mixer.attenuate_rear_speakers(0.0f);
        } else {
            mixer.attenuate_center_speakers(0.0f);
            mixer.attenuate_rear_speakers(0.0f);
            mixer.set_left_stereo_pan(0.70710677f, 0.70710677f);
            mixer.set_right_stereo_pan(0.0f, 0.0f);
        }
        break;
    }
    if (sound_xaudio_test_flag(state->flags, 9)) {
        mixer.set_left_stereo_pan(1.0f, 0.0f);
        mixer.set_right_stereo_pan(0.0f, 1.0f);
        if (channel_count <= 2) {
            mixer.attenuate_rear_speakers(0.0f);
            mixer.attenuate_center_speakers(0.0f);
        }
    } else if (state->effect == 0 && !sound_xaudio_test_flag(state->flags, 0) && channel_count <= 2) {
        const real_decibel &attenuation = channel_count == 2 ? master->global_mix.stereo_attenuation : master->global_mix.mono_attenuation;
        mixer.attenuate_all_speakers(sound_xaudio_xenon_mix_master_gain(attenuation));
        const s_sound_surround_mix_definition *surround = sound_xaudio_test_flag(state->flags, 4) ? &master->surround[1] : &master->surround[0];
        mixer.attenuate_front_speakers(sound_xaudio_xenon_mix_master_gain(surround->front));
        mixer.attenuate_rear_speakers(sound_xaudio_xenon_mix_master_gain(surround->rear));
        if (sound_xaudio_test_flag(state->flags, 1)) {
            const s_sound_center_mix_definition *center = master->surround_enabled ? &master->global_mix.center[0] : &master->global_mix.center[1];
            mixer.attenuate_center_speakers(sound_xaudio_xenon_mix_master_gain(center->center));
            mixer.attenuate_lfe_channel(0.0f);
            mixer.attenuate_front_speakers(sound_xaudio_xenon_mix_master_gain(center->front));
        } else if (state->effect != 3) {
            mixer.attenuate_center_speakers(0.0f);
        }
        if (sound_xaudio_test_flag(state->flags, 10)) {
            mixer.set_equal_power_stereo_pan(sound_xaudio_xenon_mix_master_gain(state->left_pan), sound_xaudio_xenon_mix_master_gain(state->right_pan));
        } else {
            mixer.set_stereo_pan(sound_xaudio_xenon_mix_master_gain(state->left_pan), sound_xaudio_xenon_mix_master_gain(state->right_pan));
        }
    }
    output->pitch_octaves = state->pitch * 0.00083333335f;
    output->reverb_level = sound_xaudio_xenon_mix_master_gain(state->reverb);
    output->direct_lpf = 0.0f;
    output->reverb_lpf = 0.0f;
    mixer.set_speaker_volumes(channel_count, output);
}
