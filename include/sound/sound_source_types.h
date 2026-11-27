#ifndef SOUND_SOUND_SOURCE_TYPES_H
#define SOUND_SOUND_SOURCE_TYPES_H

#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "sound/game_sound_definitions.h"
#include "cseries/cseries_wrapped.h"

enum e_sound_source_bit
{
    _sound_source_minimum_distance_valid_bit = 2,
    _sound_source_maximum_distance_valid_bit = 3,
    _sound_source_bit8 = 8,
    _sound_source_bit9 = 9
};

// Complete sound source layout, also read by the sound manager's source
// attenuation and listener-relative position routines. Spatialization packs
// the two mode nibbles into one byte; an unspatialized source clears both.
struct s_sound_source
{
    union
    {
        unsigned short flags;
        c_flags_no_init<long, unsigned short, 14> source_flags;
    };
    union
    {
        unsigned char volume_type;
        c_enum_no_init<long, char, 0, 2> volume_mode;
    };
    union
    {
        unsigned char spatialization;
        // The spatialization modes are signed four-bit values.
        struct
        {
            // current_spatialization is the high nibble: MSB-first on the console, LSB-first on the PC.
#if defined(_XBOX) && !defined(__clang__)
            signed char current_spatialization : 4;
            signed char initial_spatialization : 4;
#else
            signed char initial_spatialization : 4;
            signed char current_spatialization : 4;
#endif
        };
    };
    float volume;
    // The impulse setup writes the integer name slot; attenuation reads the
    // same word as a gain. Preserve both attested interpretations.
    union
    {
        long name;
        real_decibel gain;
    };
    real_point3d position;
    real_quaternion orientation;
    real_vector3d velocity;
    s_location location;
    float length;
    float minimum_distance;
    float maximum_distance;
    unsigned short inner_cone_angle;
    unsigned short outer_cone_angle;
    real_decibel outer_cone_gain;
};

typedef char sound_source_size[(sizeof(s_sound_source) == 0x4c) ? 1 : -1];

#endif
