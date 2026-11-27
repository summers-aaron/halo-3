#ifndef SOUND_SOUND_MANAGER_RUNTIME_TYPES_H
#define SOUND_SOUND_MANAGER_RUNTIME_TYPES_H
#include <stddef.h>
#include "sound/sound_source_types.h"
#include "objects/object_header_data.h"
#include "sound/sound_xaudio_xenon_3d_voice_types.h"
#include "sound/sound_event_callbacks.h"
struct s_cache_file_sound_permutation;

struct s_platform_sound_playback_submit_parameters {
    long random_seed;
    float effect_scale;
    char pad8[1048];
    long output;
    long channel;
    bool special;
};

struct sound_manager_output_quad { float n[4]; };

#include "sound/sound_impulse_definition.h"

struct s_sound_output_parameters
{
    unsigned long flags;
    union { real_decibel gain; long gain_bits; };
    union { real_decibel distance; long distance_bits; };
    union
    {
        struct { real_decibel lower; real_decibel upper; };
        real_bounds range;
    };
    float doppler;
    float effect_fraction;
    float cone_attenuation;
    real_point3d position;
    real_decibel_volumes output_gains;
};

struct s_platform_sound_effect_input_parameters
{
    float values[4];
    real_point3d position;
};

typedef char sound_output_parameters_size[(sizeof(s_sound_output_parameters) == 0x3c) ? 1 : -1];
typedef char sound_output_position_offset[(offsetof(s_sound_output_parameters, position) == 0x20) ? 1 : -1];
typedef char sound_impulse_source_offset[(offsetof(s_sound_impulse_definition, source) == 0xc) ? 1 : -1];
enum e_sound_manager_channel_flag
{
    _sound_manager_channel_primary_flag,
    _sound_manager_channel_secondary_flag,
    k_sound_manager_channel_flag_count
};
struct sound_channel_datum {    /* 36 bytes */
    long sound_index;
    long voice_index;
    unsigned char channel_type;
    bool paused;
    c_flags_no_init<e_sound_manager_channel_flag, unsigned char, k_sound_manager_channel_flag_count> playback_flags;
    c_flags_no_init<e_sound_manager_channel_flag, unsigned char, k_sound_manager_channel_flag_count> previous_flags;
    short source_index;
    short hardware_voice_index;
    float position;
    long transition_start;
    short queued_chunk;
    short next_chunk;
    s_cache_file_sound_permutation *queued;
    s_cache_file_sound_permutation *next;
};

#include "sound/sound_datum.h"

enum e_platform_sound_source_bit { _platform_sound_source_bit_first };
struct sound_manager_channel_unknown28 { long values[6]; };
struct s_platform_sound_source_properties {
    long unknown0;
    long samples_per_second;
    float pitch;
    real_decibel gain_db;
    short sound_class;
    short voice_state;
    real_decibel reverb_gain;
    real_decibel gain;
    real_decibel distance;
    real_decibel lower;
    real_decibel upper;
    sound_manager_channel_unknown28 unknown28;
    float cone_attenuation;
    s_sound_transmission_transform transform;
    short hardware_voice_index;
    real_decibel output_gain;
    long channel;
    c_flags<e_platform_sound_source_bit, unsigned long, 10> flags;
    long effect;
    real_decibel_volumes output_gains;
};

typedef char sound_source_properties_size[(sizeof(s_platform_sound_source_properties) == 0x78) ? 1 : -1];

struct s_sound_channel_properties {
    s_platform_sound_source_properties source_properties;
    s_platform_sound_playback_submit_parameters submit_parameters;
    sound_manager_output_quad tail;
};
typedef char sound_channel_submit_offset[(offsetof(s_sound_channel_properties, submit_parameters) == 0x78) ? 1 : -1];

typedef char sound_submit_seed_offset[
    (offsetof(s_platform_sound_playback_submit_parameters, random_seed) == 0) ? 1 : -1];
typedef char sound_output_cone_offset[
    (offsetof(s_sound_output_parameters, cone_attenuation) == 0x1c) ? 1 : -1];
typedef char sound_output_gains_offset[
    (offsetof(s_sound_output_parameters, output_gains) == 0x2c) ? 1 : -1];
typedef char sound_channel_hardware_voice_offset[
    (offsetof(sound_channel_datum, hardware_voice_index) == 0xe) ? 1 : -1];
typedef char sound_channel_reverb_gain_offset[
    (offsetof(s_platform_sound_source_properties, reverb_gain) == 0x14) ? 1 : -1];
typedef char sound_channel_cone_offset[
    (offsetof(s_platform_sound_source_properties, cone_attenuation) == 0x40) ? 1 : -1];
typedef char sound_channel_properties_hardware_voice_offset[
    (offsetof(s_platform_sound_source_properties, hardware_voice_index) == 0x54) ? 1 : -1];
typedef char sound_channel_effect_offset[
    (offsetof(s_platform_sound_source_properties, effect) == 0x64) ? 1 : -1];
typedef char sound_channel_output_gains_offset[
    (offsetof(s_platform_sound_source_properties, output_gains) == 0x68) ? 1 : -1];

struct sound_idle_geometry_globals;
struct s_player_globals;
struct s_data_array;
#endif
