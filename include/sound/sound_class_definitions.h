#ifndef SOUND_SOUND_CLASS_DEFINITIONS_H
#define SOUND_SOUND_CLASS_DEFINITIONS_H

#include <stddef.h>

#include "sound/game_sound_definitions.h"

struct sound_class_ducker
{
    real_decibel gain_db;
    float fade_in_time;
    float hold_time;
    float fade_out_time;
};

// One sound class of the sound classes tag ('sncl', sound_classes::classes; 0x98 bytes).
// Gain, ducker and transmission fields are read by the sound manager.
// March: sound_class_definition::is_deterministic() const (828a1d28) is inline.

struct sound_class_definition
{
    short maximum_sounds_per_tag;
    short maximum_sounds_per_object;
    long preemption_time;
    unsigned short internal_flags;
    unsigned short flags;                       // bit 5: deterministic
    short priority;
    short cache_miss_mode;
    union
    {
        struct { real_decibel gain_db; float reserved14; float direct_lpf; float reverb_lpf; };
        float normal[4];
    };
    union
    {
        struct { real_decibel alternate_gain_db; float reserved24; float alternate_direct_lpf; float alternate_reverb_lpf; };
        float alternate[4];
    };
    real_decibel distance_offset;
    float silence_distance;
    float attack_distance;
    float minimum_distance;
    float maximum_distance;
    real_decibel scale_gain_lower;
    real_decibel scale_gain_upper;
    sound_class_ducker duckers[3];
    float environment_floor;
    float doppler;
    signed char stereo_playback_type;
    union { float transmission_multiplier; float exponent; };
    float direct_rate;
    float reverb_rate;
    float reserved94;

    __forceinline bool loop_fade_out_linear() const { return (flags & (1 << 10)) != 0; }

    __forceinline bool is_deterministic() const
    {
        unsigned char result = (flags >> 5) & 1;
        return result;
    }
};

typedef char sound_class_scale_gain_offset[(offsetof(sound_class_definition, scale_gain_lower) == 0x44) ? 1 : -1];
typedef char sound_class_environment_floor_offset[(offsetof(sound_class_definition, environment_floor) == 0x7c) ? 1 : -1];
typedef char sound_class_reverb_rate_offset[(offsetof(sound_class_definition, reverb_rate) == 0x90) ? 1 : -1];
typedef char sound_class_silence_offset[(offsetof(sound_class_definition, silence_distance) == 0x34) ? 1 : -1];
typedef char sound_class_minimum_offset[(offsetof(sound_class_definition, minimum_distance) == 0x3c) ? 1 : -1];
typedef char sound_class_maximum_offset[(offsetof(sound_class_definition, maximum_distance) == 0x40) ? 1 : -1];
typedef char sound_class_definition_size[(sizeof(sound_class_definition) == 0x98) ? 1 : -1];
typedef char sound_class_ducker_size[(sizeof(sound_class_ducker) == 16) ? 1 : -1];
typedef char sound_class_duckers_offset[(offsetof(sound_class_definition, duckers) == 0x4c) ? 1 : -1];
typedef char sound_class_doppler_offset[(offsetof(sound_class_definition, doppler) == 0x80) ? 1 : -1];
typedef char sound_class_transmission_offset[(offsetof(sound_class_definition, transmission_multiplier) == 0x88) ? 1 : -1];

#endif // SOUND_SOUND_CLASS_DEFINITIONS_H
