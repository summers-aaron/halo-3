// blamlibxbox/sound_xaudio_xenon_3d_voice.obj: from the records 0x13bb10 0x2fa418
// @flags /TP /O2 /Oc
#include "memory/static_arrays.h"
#include "memory/wrapped_arrays.h"
#include <string.h>
#include <float.h>
#include <math.h>
#include "sound/game_sound_definitions.h"
#include "math/real_math_types.h"
#include "sound/sound_math.h"
#include <xtl.h>
#include <x3daudio.h>
#include "sound/sound_xaudio_xenon_3d_voice.h"

enum
{
    _platform_sound_voice_update_immediate_bit = 2,
};

#include "sound/sound_xaudio_xenon_3d_voice_types.h"

// @address 0xf0608 ?interpolate_scalar@@YAXPAMMM@Z
#include "render/render_objects_static_lighting.h"

#include "sound/sound_xaudio_xenon_output_types.h"

static __forceinline void copy_to_xaudio_vector(X3DAUDIO_VECTOR *out, const real_vector3d *vector)
{
    out->x = vector->i;
    out->y = vector->j;
    out->z = vector->k;
}

static __forceinline void copy_to_xaudio_position(X3DAUDIO_VECTOR *out, const real_point3d *point)
{
    out->x = point->i;
    out->y = point->j;
    out->z = point->k;
}

void c_xaudio_voice_speaker_datum::calculate(const X3DAUDIO_HANDLE &handle, long channel_count, const X3DAUDIO_LISTENER *listener,
    const s_platform_sound_voice_update_definition *update, float unused)
{
    __declspec(align(8)) X3DAUDIO_EMITTER emitter;
    memset(&emitter, 0, sizeof(emitter));
    copy_to_xaudio_vector(&emitter.OrientFront, &update->forward);
    copy_to_xaudio_vector(&emitter.OrientTop, &update->up);
    copy_to_xaudio_position(&emitter.Position, &update->position);
    copy_to_xaudio_vector(&emitter.Velocity, &update->velocity);
    float azimuths[6] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    emitter.ChannelCount = channel_count;
    emitter.ChannelRadius = 0.0f;
    emitter.pChannelAzimuths = azimuths;
    X3DAUDIO_DISTANCE_CURVE_POINT points[2] = { { 0.0f, 1.0f }, { 1.0f, 1.0f } };
    X3DAUDIO_DISTANCE_CURVE distance_curve = { points, 2 };
    emitter.pVolumeCurve = &distance_curve;
    emitter.pReverbCurve = &distance_curve;
    emitter.CurveDistanceScaler = FLT_MAX;

    __declspec(align(8)) X3DAUDIO_DSP_SETTINGS settings;
    memset(&settings, 0, sizeof(settings));
    settings.pMatrixCoefficients = m_matrix.get_elements();
    settings.SrcChannelCount = emitter.ChannelCount;
    settings.DstChannelCount = 6;
    X3DAudioCalculate(handle, listener, &emitter,
        X3DAUDIO_CALCULATE_ZEROCENTER | X3DAUDIO_CALCULATE_DOPPLER | X3DAUDIO_CALCULATE_REVERB | X3DAUDIO_CALCULATE_LPF_REVERB |
        X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_MATRIX, &settings);
    m_reverb_level = settings.ReverbLevel;
    m_distance = settings.EmitterToListenerDistance;
    float direct_lpf = update->direct_lpf * settings.LPFDirectCoefficient;
    float reverb_lpf = update->reverb_lpf * settings.LPFReverbCoefficient;
    if (update->flags & (1 << _platform_sound_voice_update_immediate_bit))
    {
        m_direct_lpf = direct_lpf;
        m_reverb_lpf = reverb_lpf;
    }
    else
    {
        interpolate_scalar(&m_direct_lpf, direct_lpf, update->reverb_rate);
        interpolate_scalar(&m_reverb_lpf, reverb_lpf, update->direct_rate);
    }
    m_doppler = settings.DopplerFactor;
}

// @address 0x205b58 ?calculate_simple_distance_attenuation@@YAMMMM@Z

static __forceinline float sound_xaudio_xenon_3d_voice_calculate_fancy_distance_attenuation(const s_sound_transmission_transform *transform, float distance)
{
    if (distance >= transform->attack_distance)
        return calculate_simple_distance_attenuation(transform->minimum_distance, transform->maximum_distance, distance);
    return calculate_simple_distance_attenuation(transform->maximum_distance - transform->attack_distance,
        transform->maximum_distance - transform->silence_distance, transform->maximum_distance - distance);
}

static __forceinline double sound_xaudio_xenon_3d_voice_bend_to_octaves(double bend)
{
    return log(bend) * 1.442695036924675;
}

static __forceinline void sound_xaudio_xenon_3d_voice_cross_mix(float *right, float *left, const s_xaudio_speaker_cross_mix *cross_mix)
{
    float old_energy = (*left) * (*left) + (*right) * (*right);
    // match: 0x2fa418: retain the existing nonnegative-energy assumption for the inline sqrt path.
    __assume(old_energy >= 0.0f);
    float new_right = (*right) * cross_mix->direct;
    new_right += (*left) * cross_mix->cross;
    float new_left = (*left) * cross_mix->direct + new_right * cross_mix->cross;
    float new_energy = new_left * new_left + new_right * new_right;
    if (new_energy > 0.001f)
    {
        float scale = sqrtf(old_energy / new_energy);
        new_right *= scale;
        new_left *= scale;
    }
    *right = new_right;
    *left = new_left;
}
static __forceinline void sound_xaudio_xenon_3d_voice_cross_mix_b(float *right, float *left, const s_xaudio_speaker_cross_mix *cross_mix)
{
    float old_energy = (*left) * (*left) + (*right) * (*right);
    // match: 0x2fa418: retain the existing nonnegative-energy assumption for the second inline sqrt path.
    __assume(old_energy >= 0.0f);
    float cr = cross_mix->cross;
    float new_right = (*right) * cross_mix->direct;
    new_right += (*left) * cr;
    float new_left = (*left) * cross_mix->direct + new_right * cr;
    float new_energy = new_left * new_left + new_right * new_right;
    if (new_energy > 0.001f)
    {
        float scale = sqrtf(old_energy / new_energy);
        new_right *= scale;
        new_left *= scale;
    }
    *right = new_right;
    *left = new_left;
}

// symbol numbering pad (c2 orders commutative operands by symbol number)
#define SOUND_XAUDIO_3D_VOICE_PAD_CAT2(a, b) a##b
#define SOUND_XAUDIO_3D_VOICE_PAD_CAT(a, b) SOUND_XAUDIO_3D_VOICE_PAD_CAT2(a, b)
#define SOUND_XAUDIO_3D_VOICE_PAD1 long SOUND_XAUDIO_3D_VOICE_PAD_CAT(symbol_pad, __COUNTER__) = channel_count * (__COUNTER__ + 1);
#define \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD1 \
    SOUND_XAUDIO_3D_VOICE_PAD1 \
    SOUND_XAUDIO_3D_VOICE_PAD1 \
    SOUND_XAUDIO_3D_VOICE_PAD1 \
    SOUND_XAUDIO_3D_VOICE_PAD1 \
    SOUND_XAUDIO_3D_VOICE_PAD1 \
    SOUND_XAUDIO_3D_VOICE_PAD1 \
    SOUND_XAUDIO_3D_VOICE_PAD1
#define \
    SOUND_XAUDIO_3D_VOICE_PAD64 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8
#define \
    SOUND_XAUDIO_3D_VOICE_PAD312 \
    SOUND_XAUDIO_3D_VOICE_PAD64 \
    SOUND_XAUDIO_3D_VOICE_PAD64 \
    SOUND_XAUDIO_3D_VOICE_PAD64 \
    SOUND_XAUDIO_3D_VOICE_PAD64 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8 \
    SOUND_XAUDIO_3D_VOICE_PAD8
// @address 0x2fa418 ?get_platform_output@c_xaudio_voice_speaker_datum@@QAAXJPBUs_xaudio_source_state@@PBUs_xaudio_speaker_cross_mix@@PAUs_xaudio_platform_output@@@Z
void c_xaudio_voice_speaker_datum::get_platform_output(long channel_count, const s_xaudio_source_state *source_state,
    const s_xaudio_speaker_cross_mix *cross_mix, s_xaudio_platform_output *out_platform_output)
{
    // match: 0x2fa418: retain existing symbol-number padding until this tuned body is re-matched naturally.
    SOUND_XAUDIO_3D_VOICE_PAD312
    float volume = 1.0f;
    float attenuation = sound_xaudio_xenon_3d_voice_calculate_fancy_distance_attenuation(
        &source_state->transform, m_distance);
    volume = source_state->gain;
    out_platform_output->resize(channel_count * 6);
    out_platform_output->attenuation_loss = (float)(1.0 - attenuation * volume);
    out_platform_output->reverb_level = m_reverb_level * attenuation * volume;
    out_platform_output->pitch_octaves = (float)sound_xaudio_xenon_3d_voice_bend_to_octaves(m_doppler);
    out_platform_output->direct_lpf = m_direct_lpf;
    out_platform_output->reverb_lpf = m_reverb_lpf;
    long sample_count = out_platform_output->count();
    for (long sample_index = 0; sample_index < sample_count; sample_index++)
        (*out_platform_output)[sample_index] = m_matrix[sample_index] * attenuation * volume;
    for (long channel = 0; channel < channel_count; channel++)
    {
        sound_xaudio_xenon_3d_voice_cross_mix(&(*out_platform_output)[channel_count + channel],
            &(*out_platform_output)[channel],
            cross_mix);
        sound_xaudio_xenon_3d_voice_cross_mix_b(&(*out_platform_output)[5 * channel_count + channel],
            &(*out_platform_output)[4 * channel_count + channel],
            cross_mix);
    }
}
