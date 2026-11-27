#ifndef SOUND_SOUND_EFFECT_MANAGER_INTERNALS_H
#define SOUND_SOUND_EFFECT_MANAGER_INTERNALS_H

#include <stddef.h>
#include "cseries/cseries_wrapped.h"
#include "memory/data_types.h"
#include "sound/sound_source_types.h"
#include "sound/sound_tracker.h"
#include "tag_files/tag_types.h"

struct s_cache_file_sound_permutation;
struct s_data_array;
struct s_data_iterator;
struct s_sound_output_parameters;
struct sound_effect_manager_definition;

extern "C"
{
}

enum e_sound_effect_source_type
{
    _sound_effect_source_manual = 0,
    _sound_effect_source_sound = 1,
    _sound_effect_source_impulse = 2
};

enum e_sound_identifier_type
{
    _sound_identifier_type_effect = 2
};

static __forceinline unsigned short generate_sound_identifier_salt(e_sound_identifier_type type)
{
    return (unsigned short)((type << 13) & 0xe000);
}

static __forceinline long create_sound_identifier(long index, e_sound_identifier_type type)
{
    return index | generate_sound_identifier_salt(type);
}

struct s_sound_effect_datum
{
    short identifier;
    char effect_state;
    c_enum<e_sound_effect_source_type, char, 0, 3> source_type;
    unsigned short flags;
    short source_data;
    long controller_index;
    union
    {
        long sound_index;
        float manual_volume;
    };
    const sound_effect_manager_definition *definition;

    long get_identifier(long index) const;
    __forceinline bool source_stopped() const { return (flags >> 4) & 1; }
    __forceinline bool stopping_abnormally() const { return (flags >> 1) & 1; }
};

inline long s_sound_effect_datum::get_identifier(long index) const
{
    if (source_type == _sound_effect_source_manual)
        return create_sound_identifier(index, _sound_identifier_type_effect);
    else
        return sound_index;
}

struct s_sound_event_callbacks;
struct sound_effect_manager_origin
{
    char origin_context[40];
    long effect_index;
    const s_sound_event_callbacks *handler;
};

template <typename t_type>
struct sound_effect_manager_block
{
    long count;
    TAG_POINTER(t_type) data;
    TAG_POINTER(void) definition;
};

struct sound_effect_manager_component
{
    unsigned long group_tag;
    char tag_reference_storage[8];
    long definition_index;
    real_decibel gain;
    unsigned long flags;
};

struct sound_effect_manager_definition
{
    char header_storage[16];
    sound_effect_manager_block<sound_effect_manager_component> components;
    char parameter_storage[44];
};

struct sound_effect_manager_iterator
{
    void *datum;
    s_data_array *array;
    int index;
    int next_index;
};

struct sound_effect_manager_sound_datum
{
    short identifier;
    signed char channel_state;
    unsigned char synchronized_playback;
    unsigned short flags;
    unsigned short playback_flags;
    long playback_index;
    long definition_index;
    long object_index;
    const s_sound_tracker *callbacks;
    s_sound_source source;
    union
    {
#ifdef HALO3_64BIT
        char event_parameters[56];
#else
        char event_parameters[48];
#endif
        struct
        {
            char effect_parameters[40];
            long effect_index;
            const s_sound_tracker *tracker;
        };
    };
    long start_time;
    float pitch;
    real_decibel gain;
    unsigned long random_seed;
    signed char pitch_range_index;
    signed char permutation_index;
    short chunk_index;
    signed char priority;
    signed char listener_index;
    unsigned char hardware_voice_index;
    signed char voice_state;
    long playback_controller_index;
    short channel_index;
    signed char fade_type;
    char stop_reason;
    real_decibel fade_gain_db;
    long fade_start_time;
    long fade_end_time;
};

typedef char sound_effect_sound_datum_size[
    (sizeof(void *) != 4 || sizeof(sound_effect_manager_sound_datum) == 0xc0) ? 1 : -1];
typedef char sound_effect_sound_datum_effect_offset[
    (sizeof(void *) != 4 || offsetof(sound_effect_manager_sound_datum, effect_index) == 0x8c) ? 1 : -1];

enum e_sound_datum_flag_bits
{
    _sound_datum_flag_bit5 = 5
};

enum e_sound_impulse_bit
{
    _sound_impulse_playback_valid_bit = 4,
    _sound_impulse_tracking_valid_bit = 5
};

#include "sound/sound_impulse_definition.h"

struct sound_effect_manager_looping_parameters
{
    long loop_state;
    bool alternate;
};

struct sound_effect_manager_component_callbacks
{
    bool (*track)(long, long, const void *, s_sound_source *);
    long extra_callbacks[7];
};

#endif // SOUND_SOUND_EFFECT_MANAGER_INTERNALS_H
