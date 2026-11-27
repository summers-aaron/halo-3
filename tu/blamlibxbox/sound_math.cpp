// blamlibxbox/sound_math.obj: from the records 0x205b58 0x205bf8
// @flags /TP /O2 /Oc
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "memory/static_arrays.h"
// @address 0x205bf8 ?calculate_positional_speaker_gain_internal@@YAXPBTreal_vector3d@@PAV?$c_static_array@M$04@@_NJ@Z

#include "cseries/cseries.h"
#include "sound/sound_manager.h"
#include "sound/sound_math.h"

// @address 0x35cfc k_positional_speaker_positions
// @rdata 0x82035cfc..0x82035d38
extern "C" {
const real_vector3d k_positional_speaker_positions[k_number_of_positional_speakers_plus_center] =
{
    { 0.70710677f, 0.70710677f, 0.0f },
    { 0.70710677f, -0.70710677f, 0.0f },
    { -0.70710677f, 0.70710677f, 0.0f },
    { -0.70710677f, -0.70710677f, 0.0f },
    { 0.70710677f, 0.0f, 0.0f }
};
}
// @rdata-end

static __forceinline float calculate_individual_positional_speaker_gain(
    const real_vector3d *localized_and_normalized_sound_position, e_sound_speaker speaker_index, bool squared)
{
    float attenuation = 0.0f;
    if (VALID_INDEX(speaker_index, k_number_of_positional_speakers_plus_center))
    {
        if (squared)
        {
            real_vector3d offset;
            offset.i = (localized_and_normalized_sound_position->i - k_positional_speaker_positions[speaker_index].i) / 2.0f;
            offset.j = (localized_and_normalized_sound_position->j - k_positional_speaker_positions[speaker_index].j) / 2.0f;
            offset.k = (localized_and_normalized_sound_position->k - k_positional_speaker_positions[speaker_index].k) / 2.0f;
            attenuation = magnitude_squared3d(&offset);
            attenuation = PIN(attenuation, 0.0f, 1.0f);
        }
    }
    return 1.0f - attenuation;
}

void calculate_positional_speaker_gain_internal(const real_vector3d *localized_and_normalized_sound_position,
    c_static_array<float, 5> *out_speaker_gains, bool squared, long speaker_count)
{
    for (long speaker_index = 0; speaker_index < speaker_count; speaker_index++)
        (*out_speaker_gains)[speaker_index] = calculate_individual_positional_speaker_gain(
            localized_and_normalized_sound_position, (e_sound_speaker)speaker_index, true);
}

// @address 0x205b58 ?calculate_simple_distance_attenuation@@YAMMMM@Z
float calculate_simple_distance_attenuation(float minimum_distance, float maximum_distance, float distance)
{
    float scale = normalize_and_clamp_linear(maximum_distance, minimum_distance, distance, false);
    float minimum = MAX(minimum_distance, 0.0001f);
    return minimum / MAX(minimum_distance, MAX(distance, 0.0001f)) * scale;
}
