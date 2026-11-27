#ifndef SOUND_SOUND_MATH_H
#define SOUND_SOUND_MATH_H

#include "math/real_math_types.h"
#include "memory/static_arrays.h"

// Functions defined in tu/blamlibxbox/sound_math.cpp that other units call.

float calculate_simple_distance_attenuation(float minimum, float maximum, float distance);
void calculate_positional_speaker_gain_internal(const real_vector3d *localized_and_normalized_sound_position,
    c_static_array<float, 5> *out_speaker_gains, bool squared, long speaker_count);

enum e_sound_speaker
{
    _sound_speaker_front_left = 0,
    _sound_speaker_front_right,
    _sound_speaker_back_left,
    _sound_speaker_back_right,
    _sound_speaker_center,
    k_number_of_positional_speakers_plus_center
};

// Unit vectors toward each positional speaker (.rdata 0x82035cfc).
extern "C" {
extern const real_vector3d k_positional_speaker_positions[k_number_of_positional_speakers_plus_center];
}

#endif // SOUND_SOUND_MATH_H
